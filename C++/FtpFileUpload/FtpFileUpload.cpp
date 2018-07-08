// FtpFileUpload.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ftp.h"
#include "Winsock2.h"

#pragma comment(lib,"Ws2_32.lib")

int _tmain(int argc, _TCHAR* argv[])
{
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;
	int err = WSAStartup(wVersionRequested,&wsaData);
	if (err != 0)
	{
		printf("WSAStartup fail ,return %d",err);
		return -1;
	}
	struct fileinfo dir;
	_resume_table resume_table;
	_fsession ftpsess;
	host_t ftp_host;
	char target_dname[128] = "/media";
	ftp_host.hostname = "10.160.71.11:8001";
	ftp_host.ip = inet_addr("10.160.71.11");
	ftp_host.port =8001;

	resume_table.large_large = RESUME_TABLE_UPLOAD;
	resume_table.large_small = RESUME_TABLE_UPLOAD;
	resume_table.small_large = RESUME_TABLE_SKIP;

	memset(&dir,0,sizeof(dir));
	dir.name = "C:/Users/cuidx/Desktop/bfq";
	dir.type = FT_DIRECTORY;

	memset(&ftpsess,0,sizeof(ftpsess));
	ftpsess.host = &ftp_host;
	ftpsess.directory = &dir;
	ftpsess.local_fname = "C:/Users/cuidx/Desktop/bfq/decode.pcm";
	ftpsess.target_dname = target_dname;
	ftpsess.target_fname = "decode.pcm";
	ftpsess.binary = 1;
	ftpsess.retry = 3;
	ftpsess.user = "anonymous";//"cuidx";
	ftpsess.pass = "";//"123456";
	//ftpsess.user = "cuidx";
	//ftpsess.pass = "123456";
	ftpsess.resume_table = &resume_table;
	int nRet = fsession_transmit_file(&ftpsess,NULL);

	WSACleanup();
	return 0;
}

