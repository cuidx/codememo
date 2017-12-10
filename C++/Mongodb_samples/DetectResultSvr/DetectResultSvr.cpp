// DetectResultSvr.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <mongoc.h>
#include <pthread.h>
#include <stdio.h>
#include "mongoose.h"
#include <list>
#include <string>
#include <json.h>

#pragma comment(lib,"pthreadVC2.lib")
#pragma comment(lib,"lib_json.lib")
#pragma comment(lib,"mongoc-1.0.lib")
#pragma comment(lib,"bson-1.0.lib")

#define DEF_REQ_GETWANIP		"service=GetWanIP&params="
#define DEF_REQ_REPORT_RESULT	"service=ReportDetectResult&params="

static pthread_mutex_t mutex;
static bool in_shutdown = false;

static pthread_mutex_t mutext_user_data;
static std::list<std::string> user_data;

static const char *s_service_uri = "/DetectResultReport/callService";
static const char *s_http_port = "8000";
static struct mg_serve_http_opts s_http_server_opts;

bool UrlDecode(std::string &strin,std::string &strout)
{
	if (strin.length() > 0)
	{
		if (strin.length() < 1024)
		{
			char lszBuf[1024] = { 0 };
			mg_url_decode(strin.c_str(),strin.length(),lszBuf,sizeof(lszBuf) - 1,1);
			strout = lszBuf;
		}
		else
		{
			char *lpBuf = new char[strin.length() + 1];
			if (NULL == lpBuf)
			{
				return false;
			}
			memset(lpBuf,0,strin.length() + 1);
			mg_url_decode(strin.c_str(),strin.length(),lpBuf,strin.length(),1);
			strout = lpBuf;
			delete []lpBuf;
			lpBuf = NULL;
		}
	}
	return true;
}

bool GetHttpVar(std::string &data,std::string name,std::string &val)
{
	if (data.empty() || name.empty())
	{
		return false;
	}
	if (data.length() < 1024)
	{
		char lszBuf[1024] = { 0 };
		struct mg_str mg_req = mg_mk_str(data.c_str());
		int nGetLen = mg_get_http_var(&mg_req,name.c_str(),lszBuf,sizeof(lszBuf) - 1);
		if (nGetLen == 0)
		{
			printf("Invalid request, param %s not found\n",name.c_str());
			return false;
		}
		else if (nGetLen < 0)
		{
			printf("service name too long,invalid service name\n");
			return false;
		}
		val = lszBuf;
	}
	else
	{
		char *lpBuf = new char[data.length() + 1];
		if (NULL == lpBuf)
		{
			return false;
		}
		memset(lpBuf,0,data.length() + 1);
		struct mg_str mg_req = mg_mk_str(data.c_str());
		int nGetLen = mg_get_http_var(&mg_req,name.c_str(),lpBuf,data.length());
		if (nGetLen == 0)
		{
			printf("Invalid request, param %s not found\n",name.c_str());
			return false;
		}
		else if (nGetLen < 0)
		{
			printf("service name too long,invalid service name\n");
			return false;
		}
		val = lpBuf;
		delete []lpBuf;
		lpBuf = NULL;
	}
	return true;
}

static void Http_Request_Proc(struct mg_connection *nc, std::string &req_enc, std::string &content_type)
{
	std::string req;

	if (content_type.compare(0,strlen("application/x-www-form-urlencoded"),"application/x-www-form-urlencoded") == 0)
	{
		UrlDecode(req_enc,req);
	}

	std::string service_name;
	int nGetLen = 0;
#if 0
	struct mg_str mg_req = mg_mk_str(req.c_str());
	char lszServiceName[256] = { 0 };
	nGetLen = mg_get_http_var(&mg_req,"service",lszServiceName,sizeof(lszServiceName) - 1);
	if (nGetLen == 0)
	{
		printf("Invalid request, param service not found\n");
		return;
	}
	else if (nGetLen < 0)
	{
		printf("service name too long,invalid service name\n");
		return;
	}
	service_name = lszServiceName;
#else
	if (!GetHttpVar(req,"service",service_name))
	{
		printf("Invalid request, param service not found\n");
		return;
	}
#endif

	if (stricmp("GetWanIP",service_name.c_str()) == 0)
	{
#if 0
		char lszParams[1024] = { 0 };
		nGetLen = mg_get_http_var(&mg_req,"params",lszParams,sizeof(lszParams) - 1);
		printf("GetWanIP params=%s\n",lszParams);
#else
		std::string sParam;
		if (GetHttpVar(req,"params",sParam))
		{
			printf("GetWanIP params=%s\n",sParam.c_str());
		}
#endif
		
		char* rmtAddr = inet_ntoa(nc->sa.sin.sin_addr);
		if (NULL == rmtAddr)
		{
			rmtAddr = "nil";
		}
		mg_send_response_line(nc, 200, "Transfer-Encoding: chunked\r\n");
		mg_printf_http_chunk(nc, "{ \"response\":{\"ip\":\"%s\"},\"result\":{\"rc\":0,\"rd\":\"ok\"} }", rmtAddr);
		mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */
	}
	else if (stricmp("ReportDetectResult",service_name.c_str()) == 0)
	{
		std::string sParam;
		if (GetHttpVar(req,"params",sParam))
		{
			printf("ReportDetectResult params=%s\n",sParam.c_str());
		}
		std::string result = sParam;
		if (!result.empty())
		{
			Json::Reader reader;
			Json::Value root;
			if (reader.parse(result,root,false))
			{
				if (root["detectresult"].isString())
				{
					std::string detect_result = root["detectresult"].asString();
					if (!detect_result.empty())
					{
						pthread_mutex_lock(&mutext_user_data);
						user_data.push_back(detect_result);
						pthread_mutex_unlock(&mutext_user_data);
					}
				}
			}
			else
			{
				printf("解析json数据失败：%s\n",result.c_str());
			}
		}
		mg_send_response_line(nc, 200, "Transfer-Encoding: chunked\r\n");
		mg_printf_http_chunk(nc, "{ \"result\":{\"rc\":0,\"rd\":\"ok\"} }");
		mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */
	}
	else
	{
		mg_send_response_line(nc, 501, "\r\n");
	}

	return;
#if 0
	if (req.compare(0,strlen(DEF_REQ_GETWANIP),DEF_REQ_GETWANIP,strlen(DEF_REQ_GETWANIP)) == 0)
	{
		char* rmtAddr = inet_ntoa(nc->sa.sin.sin_addr);
		if (NULL == rmtAddr)
		{
			rmtAddr = "nil";
		}
		mg_send_response_line(nc, 200, "Transfer-Encoding: chunked\r\n");
		mg_printf_http_chunk(nc, "{ \"response\":{\"ip\":\"%s\"},\"result\":{\"rc\":0,\"rd\":\"ok\"} }", rmtAddr);
		mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */
	}
	else if (req.compare(0,strlen(DEF_REQ_REPORT_RESULT),DEF_REQ_REPORT_RESULT,strlen(DEF_REQ_REPORT_RESULT)) == 0)
	{
		std::string result = req.substr(strlen(DEF_REQ_REPORT_RESULT));
		if (!result.empty())
		{
			Json::Reader reader;
			Json::Value root;
			if (reader.parse(result,root,false))
			{
				if (root["detectresult"].isString())
				{
					std::string detect_result = root["detectresult"].asString();
					if (!detect_result.empty())
					{
						pthread_mutex_lock(&mutext_user_data);
						user_data.push_back(detect_result);
						pthread_mutex_unlock(&mutext_user_data);
					}
				}
			}
			else
			{
				printf("解析json数据失败：%s\n",result.c_str());
			}
		}
		mg_send_response_line(nc, 200, "Transfer-Encoding: chunked\r\n");
		mg_printf_http_chunk(nc, "{ \"result\":{\"rc\":0,\"rd\":\"ok\"} }");
		mg_send_http_chunk(nc, "", 0); /* Send empty chunk, the end of response */
	}
	else
	{
		mg_send_response_line(nc, 501, "\r\n");
	}
#endif
}

static void ev_handler(struct mg_connection *nc, int ev, void *p) {
  if (ev == MG_EV_HTTP_REQUEST) 
  {
    //mg_serve_http(nc, (struct http_message *) p, s_http_server_opts);

	struct http_message * msg = (struct http_message *)p;
	if (NULL != msg) 
	{
		if (msg->method.p != NULL && strncmp(msg->method.p,"POST",4) == 0)
		{
			std::string content_type;
			mg_str* h = NULL;
			if ((h = mg_get_http_header((struct http_message *) msg, "Content-Type")) !=
				  NULL) 
			{
				content_type.assign(h->p,h->len);
			}
			if (msg->uri.p != NULL && strncmp(msg->uri.p,s_service_uri,strlen(s_service_uri)) == 0)
			{
				std::string body;
				if (msg->body.p)
				{
					body.assign(msg->body.p,msg->body.len);
				}
				char* rmtAddr = inet_ntoa(nc->sa.sin.sin_addr);
				if (NULL == rmtAddr)
				{
					rmtAddr = "nil";
				}
				printf("收到POST请求：remoteIP:%s,内容：%s\n",rmtAddr,body.c_str());

				Http_Request_Proc(nc,body,content_type);				
			}
		}
	}
  }
}

static void * httpSvrProc (void *data)
{
  struct mg_mgr mgr;
  struct mg_connection *nc;

  pthread_mutex_init (&mutext_user_data, NULL);
  mg_mgr_init(&mgr, NULL);
  nc = mg_bind(&mgr, s_http_port, ev_handler);

  // Set up HTTP server parameters
  mg_set_protocol_http_websocket(nc);
  s_http_server_opts.document_root = ".";      // Serve current directory
  s_http_server_opts.dav_document_root = NULL;  // Allow access via WebDav
  s_http_server_opts.enable_directory_listing = "no";

  printf("Starting web server on port %s\n", s_http_port);
  for (;;) 
  {
    mg_mgr_poll(&mgr, 1000);
	pthread_mutex_lock (&mutex);
    if (in_shutdown) 
	{
        pthread_mutex_unlock (&mutex);
        break;
    }

    pthread_mutex_unlock (&mutex);
  }
  mg_mgr_free(&mgr);

  return NULL;
}

static void * worker (void *data)
{
   bson_t *doc;
   mongoc_collection_t *collection;
   mongoc_client_pool_t *pool = (mongoc_client_pool_t *)data;
   mongoc_client_t *client;
   bson_error_t error;
   bool r;

   static int index = 0;

   while (true) 
   {
      client = mongoc_client_pool_pop (pool);
      /* Do something with client. If you are writing an HTTP server, you
       * probably only want to hold onto the client for the portion of the
       * request performing database queries.
       */

	  std::string result;
	  pthread_mutex_lock(&mutext_user_data);
	  if (!user_data.empty())
	  {
		  result = user_data.front();
		  user_data.pop_front();
	  }
	  pthread_mutex_unlock(&mutext_user_data);

	  if (!result.empty())
	  {
		collection = mongoc_client_get_collection (client, "mydb", "mycoll");

		doc = NULL;

		doc = bson_new_from_json((const uint8_t*)(result.c_str()),result.length(),&error);

		if (doc != NULL && !mongoc_collection_insert (collection, MONGOC_INSERT_NONE, doc, NULL, &error)) 
		{
			fprintf (stderr, "%s\n", error.message);
		}
		else
		{
			printf("插入探测结果成功：%s\n",result.c_str());
		}

		if (doc != NULL)
		{
			bson_destroy (doc);
		}
		mongoc_collection_destroy (collection);
	  }

      mongoc_client_pool_push (pool, client);

      pthread_mutex_lock (&mutex);
      if (in_shutdown || !r) 
	  {
         pthread_mutex_unlock (&mutex);
         break;
      }

      pthread_mutex_unlock (&mutex);

	  Sleep(10);
   }
   return NULL;
}

int _tmain(int argc, _TCHAR* argv[])
{
	const char *uristr = "mongodb://127.0.0.1:27017/?appname=detect_result_svr";
	mongoc_uri_t *uri;
	mongoc_client_pool_t *pool;
	pthread_t threads[10];
	pthread_t websvr_thread;
	unsigned i;
	void *ret;

	pthread_mutex_init (&mutex, NULL);
	mongoc_init ();

	if (argc > 1) 
	{
		uristr = argv[1];
	}

	uri = mongoc_uri_new (uristr);
	if (!uri) 
	{
		fprintf (stderr, "Failed to parse URI: \"%s\".\n", uristr);
		return EXIT_FAILURE;
	}

	pthread_create(&websvr_thread,NULL,httpSvrProc,NULL);

	pool = mongoc_client_pool_new (uri);
	mongoc_client_pool_set_error_api (pool, 2);

	for (i = 0; i < 10; i++) 
	{
		pthread_create (&threads[i], NULL, worker, pool);
	}

	while (1)
	{
		Sleep (100);
	}
	pthread_mutex_lock (&mutex);
	in_shutdown = true;
	pthread_mutex_unlock (&mutex);

	for (i = 0; i < 10; i++) 
	{
		pthread_join (threads[i], &ret);
	}

	mongoc_client_pool_destroy (pool);
	mongoc_uri_destroy (uri);

	mongoc_cleanup ();
	return 0;
}

