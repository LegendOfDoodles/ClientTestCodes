#pragma once

class CCreateMgr;

class CShadowMap
{
public:	// ������, �Ҹ���
	CShadowMap(CCreateMgr *pCreateMgr);
	virtual ~CShadowMap();

public:	// ���� �Լ�
	//void BuildDescriptors(
	//	D3D12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
	//	D3D12_GPU_DESCRIPTOR_HANDLE hGpuSrv, 
	//	D3D12_CPU_DESCRIPTOR_HANDLE hCpuDsv);

	//ID3D12Resource * GetResource();
	//D3D12_GPU_DESCRIPTOR_HANDLE GetSrv() const;
	//D3D12_GPU_DESCRIPTOR_HANDLE GetDsv() const;

	//D3D12_VIEWPORT GetViewport() const;
	//D3D12_RECT GetScissorRect() const;

protected: // ���� �Լ�
	//void BuildDescriptors();
	void BuildResoucre(CCreateMgr *pCreateMgr);

private:	// ����
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;

	DXGI_FORMAT m_format{ DXGI_FORMAT_R24G8_TYPELESS };

	D3D12_CPU_DESCRIPTOR_HANDLE m_cpuSrv;
	D3D12_GPU_DESCRIPTOR_HANDLE m_gpuSrv;
	D3D12_CPU_DESCRIPTOR_HANDLE m_cpuDsv;

	ComPtr<ID3D12Resource> m_shadowMap{ NULL };
};

