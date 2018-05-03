#pragma once

/// <summary>
/// ����: ��ũ�� ������ ��� ���⽱�� ó���ϱ� ���� �Ѵ�.
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-04-19
/// </summary>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

// ���α׷����� ��� ���� ��ũ�θ� �����մϴ�.
#define	FRAME_BUFFER_WIDTH		1280
#define	FRAME_BUFFER_HEIGHT		720

#define	SWAP_CHAIN_BUFFER_CNT	2

#define MAX_LIGHTS				8 
#define MAX_MATERIALS			1

#define POINT_LIGHT				1
#define SPOT_LIGHT				2
#define DIRECTIONAL_LIGHT		3
#define SPECULAR_LIGHT		4

#define TERRAIN_SIZE_WIDTH 10000
#define TERRAIN_SIZE_HEIGHT 5000

#define TERRAIN_IMAGE_WIDTH 500
#define TERRAIN_IMAGE_HEIGHT 250
#define TERRAIN_IMAGE_SCALE XMFLOAT3(TERRAIN_SIZE_WIDTH / TERRAIN_IMAGE_WIDTH, 0.7, TERRAIN_SIZE_HEIGHT / TERRAIN_IMAGE_HEIGHT)

#define REVISE_HEIGHT 153
#define REVISE_HEIGHT_FOR_OBJECT REVISE_HEIGHT + 7.0f

#define NODE_SIZE 41.4

// ���� ����
#define RANDOM_COLOR XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX))

// ���� ��ȯ �Լ�
#define CONVERT_PaperUnit_to_InG(val) val * 8.28
#define CONVERT_Unit_to_InG(val) val * 1000

// ĸ�� FPS ��� ���� -------------------
// �׻� ĸ�ǿ� FPS�� ���		(0 : ��Ȱ�� | 1 : Ȱ��)
#define USE_CAPTIONFPS_ALWAYS	 1
#define USE_TITLE_STRING		 1
#define USE_LIMIT_MAX_FPS		 1
#define USE_CONSOLE_WINDOW 0
#define USE_INSTANCING 0
#define USE_BATCH_MATERIAL 1

#if USE_CAPTIONFPS_ALWAYS
#define SHOW_CAPTIONFPS 
#elif _DEBUG	// Debug������ �׻� ����
#define SHOW_CAPTIONFPS 
#endif

#if defined(SHOW_CAPTIONFPS)
#define MAX_UPDATE_FPS 1.0 / 3.0
#endif

// �ִ� FPS
#if USE_LIMIT_MAX_FPS
#define MAX_FPS 1.0 / 120.0
#else
#define MAX_FPS 0.0
#endif

#define TITLE_MAX_LENGTH 64

#if USE_TITLE_STRING
#define TITLESTRING	TEXT("LegendOfDoodles")
#endif

#if USE_CONSOLE_WINDOW
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console") 
#elif _DEBUG
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console") 
#endif