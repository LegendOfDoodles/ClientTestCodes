#include "stdafx.h"

#include "Framework.h"



/// <summary>
/// 목적: 테
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-04-14
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CFramework::CFramework()
{
}

CFramework::~CFramework()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
bool CFramework::Initialize(HINSTANCE hInstance, HWND hWnd)
{
	m_hWnd = hWnd;

	m_createMgr.Initialize(hInstance, hWnd);
	m_pRenderMgr = m_createMgr.GetRenderMgr();

	BuildObjects();

	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	m_mysocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(MY_SERVER_PORT);
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int Result = WSAConnect(m_mysocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);

	WSAAsyncSelect(m_mysocket, hWnd, WM_SOCKET, FD_CLOSE | FD_READ);

	m_send_wsabuf.buf = m_send_buffer;
	m_send_wsabuf.len = MAX_BUFF_SIZE;
	m_recv_wsabuf.buf = m_recv_buffer;
	m_recv_wsabuf.len = MAX_BUFF_SIZE;

	return(true);
}

void CFramework::Finalize()
{
	ReleaseObjects();
	m_createMgr.Release();
}

void CFramework::FrameAdvance(float timeElapsed)
{
	m_pScene->ProcessInput();
	m_pScene->AnimateObjects(timeElapsed);
	m_pRenderMgr->Render(m_pScene);
}

void CFramework::ProcessPacket(char *ptr)
{
	static bool first_time = true;
	switch (ptr[1])
	{
	case SC_PUT_PLAYER:
	{
		SC_MsgChCreate *my_packet = reinterpret_cast<SC_MsgChCreate *>(ptr);
		int id = my_packet->Character_id;
		if (first_time) {
			first_time = false;
			m_myid = id;
		}
		if (id == m_myid) {
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
	case SC_MOVE_PLAYER:
	{
		SC_MsgChMove *my_packet = reinterpret_cast<SC_MsgChMove *>(ptr);
		int other_id = my_packet->Character_id;
		if (other_id == m_myid) {
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

	case SC_DISCONNECT_PLAYER:
	{
		SC_MsgChDelete *my_packet = reinterpret_cast<SC_MsgChDelete *>(ptr);
		int other_id = my_packet->Character_id;
		if (other_id == m_myid) {
			
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
	}
}

void CFramework::ReadPacket(SOCKET sock)
{
	DWORD iobyte, ioflag = 0;

	int ret = WSARecv(sock, &m_recv_wsabuf, 1, &iobyte, &ioflag, NULL, NULL);
	if (ret) {
		int err_code = WSAGetLastError();
		printf("Recv Error [%d]\n", err_code);
	}

	BYTE *ptr = reinterpret_cast<BYTE *>(m_recv_buffer);

	while (0 != iobyte) {
		if (0 == m_in_packet_size) m_in_packet_size = ptr[0];
		if (iobyte + m_saved_packet_size >= m_in_packet_size) {
			memcpy(m_packet_buffer + m_saved_packet_size, ptr, m_in_packet_size - m_saved_packet_size);
			ProcessPacket(m_packet_buffer);
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


LRESULT CALLBACK CFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
	{
		m_createMgr.Resize(LOWORD(lParam), HIWORD(lParam));
		// m_createMgr.ChangeScreenMode();
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
		m_pScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	{
		int x = 0, y = 0;
		if (wParam == VK_RIGHT)	x += 1;
		if (wParam == VK_LEFT)	x -= 1;
		if (wParam == VK_UP)	y -= 1;
		if (wParam == VK_DOWN)	y += 1;
		CS_MsgChMove *my_packet = reinterpret_cast<CS_MsgChMove *>(m_send_buffer);
		my_packet->size = sizeof(my_packet);
		m_send_wsabuf.len = sizeof(my_packet);
		DWORD iobyte;
		if (0 != x) {
			if (1 == x) my_packet->type = CS_RIGHT;
			else my_packet->type = CS_LEFT;
			int ret = WSASend(m_mysocket, &m_send_wsabuf, 1, &iobyte, 0, NULL, NULL);
			if (ret) {
				int error_code = WSAGetLastError();
				printf("Error while sending packet [%d]", error_code);
			}
		}
		if (0 != y) {
			if (1 == y) my_packet->type = CS_DOWN;
			else my_packet->type = CS_UP;
			WSASend(m_mysocket, &m_send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		}



		break;
	}
	case WM_KEYUP:
		m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_SOCKET:
		{
			if (WSAGETSELECTERROR(lParam)) {
				closesocket((SOCKET)wParam);
				exit(-1);
				break;
			}
			switch (WSAGETSELECTEVENT(lParam)) {
			case FD_READ:
				//ReadPacket((SOCKET)wParam);
				break;
			case FD_CLOSE:
				closesocket((SOCKET)wParam);
				exit(-1);
				break;
			}
		}
	}
	return(0);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CFramework::BuildObjects()
{
	m_pRenderMgr->ResetCommandList();

	m_pScene = new CScene();
	m_pScene->Initialize(&m_createMgr);

	m_pRenderMgr->ExecuteCommandList();

	m_pScene->ReleaseUploadBuffers();
}

void CFramework::ReleaseObjects()
{
	if (!m_pScene) return;

	m_pScene->Finalize();
	Safe_Delete(m_pScene);
}