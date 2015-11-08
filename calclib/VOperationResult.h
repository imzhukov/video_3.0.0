#pragma once

#include <vector>
#include <string>

enum OPERATION_RESULT
{
	OPRES_OK,
	OPRES_OK_WITH_COMMENTS,
	OPRES_FAILURE,
	OPRES_IN_PROGRESS
};

class VOperationResult
{
	OPERATION_RESULT result;
	std::vector<std::wstring> messages;

public:

	/// �����������
	VOperationResult();

	/// ��� ���������� ��������
	OPERATION_RESULT GetOperationResult() const;

	/// ���������� ���������
	size_t GetMsgCount() const;

	/// ��������� � �������� ndx
	const std::wstring & GetMsg(size_t ndx) const;

	/// ���������� ��� ���������� ��������
	void SetOperationResult(OPERATION_RESULT __r);

	/// �������� ���������
	void AddMsg(const std::wstring & __m);

	/// �������� ������ ���������
	void ClearMsg();


};

	