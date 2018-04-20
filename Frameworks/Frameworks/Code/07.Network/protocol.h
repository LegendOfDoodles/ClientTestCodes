#include <windows.h>

#pragma once
#pragma pack(push,1)
struct Packet {
	
	BYTE size;
	short type;
};
//��Ģ
//SC -> �������� Ŭ�� CS-> Ŭ�󿡼� ����
//Ch -> ĳ���� Mo -> �߸�����, �̴Ͼ�
struct SC_MsgConnect : public Packet
{
	BYTE client_id;
	BYTE Character_id;
};
struct SC_MsgChCreate : public Packet
{
	BYTE Character_id;
	short x;
	short y;
};
struct SC_MsgMoCreate : public Packet
{
	BYTE Monster_id;
	short x;
	short y;
};
struct SC_MsgChDelete : public Packet
{
	BYTE Character_id;
};
struct SC_MsgMoDelete : public Packet
{
	BYTE Monster_id;
};
struct SC_MsgChMove : public Packet
{
	BYTE Character_id;
	short x;
	short y;
};
struct SC_MsgMoMove : public Packet
{
	BYTE Monster_id;
	short x;
	short y;
};
struct SC_MsgChAttack : public Packet
{
	BYTE Character_id;
	//�迭�� ó���ؾ� �Ҷ��� ������?
	//���߽�ų!
	BYTE Monster_id;
};
struct SC_MsgMoAttack : public Packet
{
	BYTE Monster_id;
	BYTE Character_id;
};
struct SC_MsgChHit : public Packet
{
	BYTE Character_id;
	short Ch_Health;
};
struct SC_MsgMoHit : public Packet
{
	BYTE Monster_id;
	short Mo_Health;
};

struct CS_MsgChMove : public Packet
{
	BYTE Character_id;
};
#pragma pack(pop)

#define MAX_BUFF_SIZE 4000
#define MAX_PACKET_SIZE 255

#define MAP_WIDTH 800
#define MAP_HEIGHT 800

#define MAX_USER 5
#define NPC_START 1000
#define NUM_OF_NPC 10000

#define MY_SERVER_PORT 4000

//enum class Type : BYTE { Connect, Create, Delete, Move, Attack, Hit };
//Server-> Client
#define SC_CONNECT_PLAYER		  101
#define SC_PUT_PLAYER			  102
#define SC_DISCONNECT_PLAYER	  103
#define SC_MOVE_PLAYER			  104
#define SC_ATTACK				  105
#define SC_HIT					  106
#define SC_PUT_MONSTER			  107
#define SC_PUT_MINION			  108
//Client->Server
#define CS_ACTION				  201
#define CS_MOVE_PLAYER			  202
//In Client Move Object
#define CS_UP					  203
#define CS_DOWN					  204
#define CS_LEFT					  205
#define CS_RIGHT				  206




//enum �߰��� ����

