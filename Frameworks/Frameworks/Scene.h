#pragma once

class CCreateMgr;

class CScene
{
public:	// ������, �Ҹ���
	CScene();
	~CScene();

public: // ���� �Լ�
	void BuildObjects(CCreateMgr *pCreateMgr);
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();
	void Render();

	// Message Process
	bool OnProcessingMouseMessage(HWND hwnd, UINT messageID,
		WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hwnd, UINT messageID,
		WPARAM wParam, LPARAM lParam);

private: // ���� �Լ�

private: // ����
	ID3D12RootSignature *m_pGraphicsRootSignature;
	ID3D12PipelineState *m_pPipelineState;

	ID3D12GraphicsCommandList *m_pCommandList;
};

