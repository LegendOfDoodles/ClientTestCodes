#pragma once

/// <summary>
/// ����: Release �� Delete ���� �ش� ������ NULL�� �����Ͽ� ������ �����Ѵ�.
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-01-12
/// </summary>

#define NULL 0

template <typename T> 
inline void Safe_Delete(T& p)
{
	if (!p) return;

	delete p;
	p = NULL;
}

template <typename T> 
inline void Safe_Delete_Array(T& p)
{
	if (!p) return;

	delete[] p;
	p = NULL;
}

template <typename T> 
inline void Safe_Release(T& p)
{
	if (!p) return;

	p->Release();
	p = NULL;
}