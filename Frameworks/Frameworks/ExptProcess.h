#pragma once
#include <iostream>
#include <Windows.h>

namespace ExptProcess
{
	class HResultExpt		// �Ƹ� ��� �� �� ��
	{
		HRESULT m_hResult;
	public:	// ������, �Ҹ���
		HResultExpt(HRESULT hResult) : m_hResult(hResult) {}

	public:	//���� �Լ�
		void What()
		{
			if (m_hResult == E_ABORT) std::cerr << "Operation aborted" << std::endl;
			else if (m_hResult == E_ACCESSDENIED) std::cerr << "General access denied error" << std::endl;
			else if (m_hResult == E_FAIL) std::cerr << "Unspecified failure" << std::endl;
			else if (m_hResult == E_HANDLE) std::cerr << "Handle that is not valid" << std::endl;
			else if (m_hResult == E_INVALIDARG) std::cerr << "One or more arguments are not valid	" << std::endl;
			else if (m_hResult == E_NOINTERFACE) std::cerr << "No such interface supported" << std::endl;
			else if (m_hResult == E_NOTIMPL) std::cerr << "Not implemented" << std::endl;
			else if (m_hResult == E_OUTOFMEMORY) std::cerr << "Failed to allocate necessary memory" << std::endl;
			else if (m_hResult == E_POINTER) std::cerr << "Pointer that is not valid" << std::endl;
			else if (m_hResult == E_UNEXPECTED) std::cerr << "Unexpected failure" << std::endl;
			else std::cerr << "What is this?" << std::endl;
		}
	};

	void PrintErrorBlob(ComPtr<ID3DBlob>& pErrorBlob);
}