#pragma once

class CCreateMgr;

class CScene
{
public:	// 생성자, 소멸자
	CScene();
	~CScene();

public: // 공개 함수
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

private: // 내부 함수

private: // 변수
	ID3D12RootSignature *m_pGraphicsRootSignature;
	ID3D12PipelineState *m_pPipelineState;

	ID3D12GraphicsCommandList *m_pCommandList;
};

