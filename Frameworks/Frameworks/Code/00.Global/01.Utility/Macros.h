#pragma once

/// <summary>
/// 목적: 메크로 연산을 모아 보기쉽고 처리하기 쉽게 한다.
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-04-19
/// </summary>

#define _WINSOCK_DEPRECATED_NO_WARNINGS

// 프로그램에서 계속 사용될 메크로를 정의합니다.
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

// 랜덤 색상
#define RANDOM_COLOR XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX))

// 단위 변환 함수
#define CONVERT_PaperUnit_to_InG(val) val * 8.28
#define CONVERT_Unit_to_InG(val) val * 1000

// 캡션 FPS 출력 여부 -------------------
// 항상 캡션에 FPS를 출력		(0 : 비활성 | 1 : 활성)
#define USE_CAPTIONFPS_ALWAYS	 1
#define USE_TITLE_STRING		 1
#define USE_LIMIT_MAX_FPS		 1
#define USE_CONSOLE_WINDOW 0
#define USE_INSTANCING 0
#define USE_BATCH_MATERIAL 1

#if USE_CAPTIONFPS_ALWAYS
#define SHOW_CAPTIONFPS 
#elif _DEBUG	// Debug에서는 항상 실행
#define SHOW_CAPTIONFPS 
#endif

#if defined(SHOW_CAPTIONFPS)
#define MAX_UPDATE_FPS 1.0 / 3.0
#endif

// 최대 FPS
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