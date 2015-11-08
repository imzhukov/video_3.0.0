#include "VOperationResult.h"

// �����������
VOperationResult::VOperationResult() : result(OPRES_IN_PROGRESS)
{}

// ��� ���������� ��������
OPERATION_RESULT VOperationResult::GetOperationResult() const
{
	return this->result;
}

// ���������� ���������
size_t VOperationResult::GetMsgCount() const
{
	return this->messages.size();
}

// ��������� � �������� ndx
const std::wstring & VOperationResult::GetMsg(size_t ndx) const
{
	return this->messages.at(ndx);
}

// ���������� ��� ���������� ��������
void VOperationResult::SetOperationResult(OPERATION_RESULT __r)
{
	this->result = __r;
}

// �������� ���������
void VOperationResult::AddMsg(const std::wstring & __m)
{
	this->messages.push_back(__m);
}

// �������� ������ ���������
void VOperationResult::ClearMsg()
{
	this->messages.clear();
}
