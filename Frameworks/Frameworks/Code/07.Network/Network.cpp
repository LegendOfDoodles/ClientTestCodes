#include "stdafx.h"
#include "Network.h"


Network::Network()
{
}


Network::~Network()
{
}

void Network::Initialize(HWND hWnd)
{
	m_hWnd = hWnd;
	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	m_mysocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(MY_SERVER_PORT);
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int Result = WSAConnect(m_mysocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);

	WSAAsyncSelect(m_mysocket, m_hWnd, WM_SOCKET, FD_CLOSE | FD_READ);

	m_send_wsabuf.buf = m_send_buffer;
	m_send_wsabuf.len = MAX_BUFF_SIZE;
	m_recv_wsabuf.buf = m_recv_buffer;
	m_recv_wsabuf.len = MAX_BUFF_SIZE;
}

void Network::ProcessPacket(int myid, char *ptr, CBaseObject* object)
{
	static bool first_time = true;
	//printf("%d\n", ptr[1]);
	if (ptr[1] != 0) {
		switch (ptr[1])
		{
		case SC_PUT_PLAYER:
		{
			SC_Msg_Put_Character *my_packet = reinterpret_cast<SC_Msg_Put_Character *>(ptr);
			int id = my_packet->Character_id;
			if (first_time) {
				first_time = false;
				myid = id;
			}
			if (id == myid) {
				//player.x = my_packet->x;//캐릭터
				//player.y = my_packet->y;//캐릭터
			}
			//else if (id < NPC_START) { //다른플레이어에게 알려줄때 쓰는거
			//skelaton[id].x = my_packet->x;
			//skelaton[id].y = my_packet->y;
			//skelaton[id].attr |= BOB_ATTR_VISIBLE;
			//}
			//else { //미니언, 몬스터 관리할때 쓰는거
			//npc[id - NPC_START].x = my_packet->x;
			//npc[id - NPC_START].y = my_packet->y;
			//npc[id - NPC_START].attr |= BOB_ATTR_VISIBLE;
			//}
			break;
		}
		case SC_PUT_MINION:
		{
			SC_MsgMoCreate *my_packet = reinterpret_cast<SC_MsgMoCreate *>(ptr);
			int id = my_packet->Monster_id;
			if (first_time) {
				first_time = false;
			}

			break;
		}

		case SC_MOVE_PLAYER:
		{
			SC_MsgChMove *my_packet = reinterpret_cast<SC_MsgChMove *>(ptr);
			int other_id = my_packet->Character_id;
			if (other_id == myid) {
				//printf("fuck\n");
				object->SetPosition(my_packet->x, my_packet->y);
				//player.x = my_packet->x;
				//player.y = my_packet->y;
				
			}
			/*else if (other_id < NPC_START) {
			skelaton[other_id].x = my_packet->x;
			skelaton[other_id].y = my_packet->y;
			}
			else {
			npc[other_id - NPC_START].x = my_packet->x;
			npc[other_id - NPC_START].y = my_packet->y;
			}*/
			break;
		}

		case SC_REMOVE_PLAYER:
		{
			SC_Msg_Remove_Character *my_packet = reinterpret_cast<SC_Msg_Remove_Character *>(ptr);
			int other_id = my_packet->Character_id;
			if (other_id == myid) {

			}
			else if (other_id < NPC_START) {
				//skelaton[other_id].attr &= //안보이게 처리;
			}
			else {
				//npc[other_id - NPC_START].attr &= //안보이게 처리;
			}
			break;
		}

		default:
			printf("Unknown PACKET type [%d]\n", ptr[1]);
			break;
		}
	}
}

void Network::Finalize()
{
	closesocket(m_mysocket);
}

void Network::ReadPacket(SOCKET sock,CBaseObject* object)
{
	DWORD ioflag = 0;
	DWORD iobyte = 0;
	int ret = WSARecv(sock, &m_recv_wsabuf, 1, &iobyte, &ioflag, NULL, NULL);
	if (ret) {
		int err_code = WSAGetLastError();
		printf("Recv Error [%d]\n", err_code);
		if (err_code == 10035) {
			Sleep(50);
			ProcessPacket(m_myid, m_packet_buffer, object);
		}
	}
	BYTE *ptr = reinterpret_cast<BYTE *>(m_recv_buffer);

	while (0 != iobyte) {
		if (0 == m_in_packet_size) m_in_packet_size = ptr[0];
		if (iobyte + m_saved_packet_size >= m_in_packet_size) {
			memcpy(m_packet_buffer + m_saved_packet_size, ptr, m_in_packet_size - m_saved_packet_size);
			ProcessPacket(m_myid, m_packet_buffer, object);
			ptr += m_in_packet_size - m_saved_packet_size;
			iobyte -= m_in_packet_size - m_saved_packet_size;
			m_in_packet_size = 0;
			m_saved_packet_size = 0;
		}
		else {
			memcpy(m_packet_buffer + m_saved_packet_size, ptr, iobyte);
			m_saved_packet_size += iobyte;
			iobyte = 0;
		}
	}
	
	
}

//void Network::SendMovePacket(WPARAM wParam, CBaseObject* object)
//{
//	int x = 0, y = 0;
//	if (wParam == VK_RIGHT)	x += 1;
//	if (wParam == VK_LEFT)	x -= 1;
//	if (wParam == VK_UP)	y -= 1;
//	if (wParam == VK_DOWN)	y += 1;
//	CS_MsgChMove *my_packet = reinterpret_cast<CS_MsgChMove *>(m_send_buffer);
//	my_packet->size = sizeof(my_packet);
//	m_send_wsabuf.len = sizeof(my_packet);
//	DWORD iobyte;
//	if (0 != x) {
//		if (1 == x) my_packet->type = CS_RIGHT;
//		else my_packet->type = CS_LEFT;
//		int ret = WSASend(m_mysocket, &m_send_wsabuf, 1, &iobyte, 0, NULL, NULL);
//		if (ret) {
//			int error_code = WSAGetLastError();
//			printf("Error while sending packet [%d]", error_code);
//		}
//	}
//	if (0 != y) {
//		if (1 == y) my_packet->type = CS_DOWN;
//		else my_packet->type = CS_UP;
//		WSASend(m_mysocket, &m_send_wsabuf, 1, &iobyte, 0, NULL, NULL);
//	}
//	ReadPacket(m_mysocket,object);
//}

