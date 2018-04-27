#include "protocol.h"
#include "Winsock2.h"


#pragma once
class Network
{
public:
	HWND m_hWnd{ NULL };
	
	SOCKET  m_mysocket;
	WSABUF	m_send_wsabuf;
	char 	m_send_buffer[MAX_BUFF_SIZE];
	WSABUF	m_recv_wsabuf;
	char	m_recv_buffer[MAX_BUFF_SIZE];
	char	m_packet_buffer[MAX_BUFF_SIZE];
	DWORD	m_in_packet_size = 0;
	int		m_saved_packet_size = 0;
	int		m_myid;

	CBaseObject * m_pSelectedObject{ NULL };

public:
	Network();
	~Network();
	void Initialize(HWND hWnd);
	void ProcessPacket(int myid, char *ptr, CBaseObject* object);
	void Finalize();
	void ReadPacket(SOCKET sock, CBaseObject* object);
	
};

