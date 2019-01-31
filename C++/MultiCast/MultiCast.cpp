// MultiCast.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "MuiltiCastSock.h"

#pragma comment(lib,"Ws2_32.lib")

#if 0
int main(int argc, char *argv[])
{
	WSADATA wsad;
	WSAStartup(MAKEWORD(1,1),&wsad);

	SOCKET socksend = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);

	sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_addr.S_un.S_addr = INADDR_ANY;
	local.sin_port = 4004;
	memset(local.sin_zero,0,8);
	bind(socksend,(sockaddr*)&local,sizeof(local));

	ip_mreq mcast;
	mcast.imr_interface.S_un.S_addr = INADDR_ANY;
	mcast.imr_multiaddr.S_un.S_addr = inet_addr("234.0.23.23");
	setsockopt(socksend,IPPROTO_IP,IP_ADD_MEMBERSHIP,(char*)&mcast,sizeof(mcast));

	int optval = 8;
	setsockopt(socksend,IPPROTO_IP,IP_MULTICAST_TTL,(char*)&optval,sizeof(int));

	int loop = 1;
	setsockopt(socksend,IPPROTO_IP,IP_MULTICAST_LOOP,(char*)&loop,sizeof(int));

	sockaddr_in remote;
	remote.sin_addr.S_un.S_addr = inet_addr("234.0.23.23");
	remote.sin_family = AF_INET;
	remote.sin_port = 4005;
	memset(remote.sin_zero,0,8);

	char sendbuf[100];
	memset(sendbuf,0,sizeof(char)*100);
	for (int i = 0; i < 3600; i ++ )
	{
		sprintf(sendbuf,"send: this is test %d\n",i);
		printf(sendbuf);
		int rt = sendto(socksend,sendbuf,strlen(sendbuf),0,(sockaddr*)&remote,sizeof(remote));

		Sleep(1000);
	}

	setsockopt(socksend,IPPROTO_IP,IP_DROP_MEMBERSHIP,(char*)&mcast,sizeof(mcast));

	closesocket(socksend);

	return 0;
}
#else

int main(int argc, char *argv[])
{
	MuiltCastSock sock;
	sock.InitWinSock();
	if (sock.AddMemberShip(std::string("234.0.23.23"),4005,4004))
	{
		char sendbuf[100];
		for (int icount = 0; icount < 100; icount++)
		{
			sprintf(sendbuf,"send: this is test %d\n",icount);
			sock.SendData(sendbuf,strlen(sendbuf)+ 1);
			Sleep(1000);
		}
		sock.DropMemberShip();
	}
	sock.UnInitWinSock();

	getchar();

	return 0;
}

#endif
