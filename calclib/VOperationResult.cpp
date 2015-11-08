#include "VOperationResult.h"

// Конструктор
VOperationResult::VOperationResult() : result(OPRES_IN_PROGRESS)
{}

// Код завершения операции
OPERATION_RESULT VOperationResult::GetOperationResult() const
{
	return this->result;
}

// Количество сообщений
size_t VOperationResult::GetMsgCount() const
{
	return this->messages.size();
}

// Сообщение с индексом ndx
const std::wstring & VOperationResult::GetMsg(size_t ndx) const
{
	return this->messages.at(ndx);
}

// Установить код завершения операции
void VOperationResult::SetOperationResult(OPERATION_RESULT __r)
{
	this->result = __r;
}

// Добавить сообщение
void VOperationResult::AddMsg(const std::wstring & __m)
{
	this->messages.push_back(__m);
}

// Очистить список сообщений
void VOperationResult::ClearMsg()
{
	this->messages.clear();
}
