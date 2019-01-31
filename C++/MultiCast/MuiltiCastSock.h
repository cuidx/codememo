#ifndef __MUILTICASTSOCK_H__
#define __MUILTICASTSOCK_H__

#include <string>

class MuiltCastSock
{
public:
	MuiltCastSock();
	virtual ~MuiltCastSock();

public:
	static bool InitWinSock();
	static void UnInitWinSock();
	static void RecvDataProc(void* pObject);

	bool AddMemberShip(const std::string& muiltCastIp,const unsigned short& remotePort,const unsigned short& localPort);
	bool DropMemberShip();
	bool SendData(const char* buf,const int &len);
	bool OnRecvData(const char* buf,const int &len);

private:
	std::string			m_MuiltCastIp;
	unsigned short		m_remotePort;
	unsigned short		m_localPort;
	int					m_sock;
};

#endif
