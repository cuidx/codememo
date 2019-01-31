#include "stdafx.h"
#include "MuiltiCastSock.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <process.h>

MuiltCastSock::MuiltCastSock()
{
	m_sock = 0;
	m_localPort = 0;
	m_remotePort = 0;
}

MuiltCastSock::~MuiltCastSock()
{
}

bool MuiltCastSock::InitWinSock()
{
	bool ret = true;
	WSADATA wsad;
	int nRet = WSAStartup(MAKEWORD(2,2),&wsad);
	if (nRet != 0)
	{
		printf("WSAStartup fail ,ret=%d\n",nRet);
	}
	return ret;
}

void MuiltCastSock::UnInitWinSock()
{
	WSACleanup();
}

void MuiltCastSock::RecvDataProc(void* pObject)
{
	MuiltCastSock* pSock = (MuiltCastSock*)pObject;
	if (pSock)
	{
		char recvbuf[1024] = { 0 };
		while (true)
		{
			sockaddr_in from;
			int fromlen  = sizeof(from);
			int rt = recvfrom(pSock->m_sock,recvbuf,sizeof(recvbuf) - 1,0,(sockaddr*)&from,&fromlen);
			if (rt > 0)
			{
				recvbuf[rt] = 0;
				printf("recv : -- %s -- , from : %s ,port : %d \n",recvbuf,inet_ntoa(from.sin_addr),from.sin_port);
				pSock->OnRecvData(recvbuf,rt);
			}
			else
			{
				printf("recvfrom 返回 %d, 接收线程退出",rt);
				break;
			}
		}
	}
}

bool MuiltCastSock::AddMemberShip(const std::string& muiltCastIp,const unsigned short& remotePort,const unsigned short& localPort)
{
	bool ret = true;
	m_localPort = localPort;
	m_remotePort = remotePort;
	m_MuiltCastIp = muiltCastIp;

	m_sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);

	sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_addr.S_un.S_addr = INADDR_ANY;
	local.sin_port = m_localPort;
	memset(local.sin_zero,0,8);
	bind(m_sock,(sockaddr*)&local,sizeof(local));

	ip_mreq mcast;
	mcast.imr_interface.S_un.S_addr = INADDR_ANY;
	mcast.imr_multiaddr.S_un.S_addr = inet_addr(m_MuiltCastIp.c_str());
	setsockopt(m_sock,IPPROTO_IP,IP_ADD_MEMBERSHIP,(char*)&mcast,sizeof(mcast));

	int optval = 8;
	setsockopt(m_sock,IPPROTO_IP,IP_MULTICAST_TTL,(char*)&optval,sizeof(int));

	int loop = 1;
	setsockopt(m_sock,IPPROTO_IP,IP_MULTICAST_LOOP,(char*)&loop,sizeof(int));

	_beginthread(&MuiltCastSock::RecvDataProc,0,(void*)this);
	return ret;
}

bool MuiltCastSock::DropMemberShip()
{
	bool ret = true;
	ip_mreq mcast;
	mcast.imr_interface.S_un.S_addr = INADDR_ANY;
	mcast.imr_multiaddr.S_un.S_addr = inet_addr(m_MuiltCastIp.c_str());
	setsockopt(m_sock,IPPROTO_IP,IP_ADD_MEMBERSHIP,(char*)&mcast,sizeof(mcast));
	setsockopt(m_sock,IPPROTO_IP,IP_DROP_MEMBERSHIP,(char*)&mcast,sizeof(mcast));

	closesocket(m_sock);

	m_sock = 0;

	return ret;
}

bool MuiltCastSock::SendData(const char* buf,const int &len)
{
	bool ret = false;
	if (m_sock > 0)
	{
		sockaddr_in remote;
		remote.sin_addr.S_un.S_addr = inet_addr(m_MuiltCastIp.c_str());
		remote.sin_family = AF_INET;
		remote.sin_port = m_remotePort;
		memset(remote.sin_zero,0,8);
		printf(buf);
		int rt = sendto(m_sock,buf,len,0,(sockaddr*)&remote,sizeof(remote));
	}
	return ret;
}

bool MuiltCastSock::OnRecvData(const char* buf,const int &len)
{
	bool ret = false;

	return ret;
}
