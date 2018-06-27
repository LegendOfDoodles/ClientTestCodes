#include "stdafx.h"
#include "ShadowMap.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"

/// <summary>
/// 목적: 
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-06-27
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CShadowMap::CShadowMap(CCreateMgr *pCreateMgr)
{
	m_viewport = { 0.0f, 0.0f, static_cast<float>(pCreateMgr->GetWindowWidth()), static_cast<float>(pCreateMgr->GetWindowHeight()), 0.0f, 1.0f };
	m_scissorRect = { 0, 0, pCreateMgr->GetWindowWidth(), pCreateMgr->GetWindowHeight() };

	BuildResoucre(pCreateMgr);
}

CShadowMap::~CShadowMap()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CShadowMap::BuildResoucre(CCreateMgr *pCreateMgr)
{
	D3D12_CLEAR_VALUE optClear;
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	
	m_shadowMap = pCreateMgr->CreateTexture2DResource(
		pCreateMgr->GetWindowWidth(),
		pCreateMgr->GetWindowHeight(),
		DXGI_FORMAT_R24G8_TYPELESS, 
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, 
		D3D12_RESOURCE_STATE_GENERIC_READ, 
		&optClear);
}
