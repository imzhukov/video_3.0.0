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

	/// Конструктор
	VOperationResult();

	/// Код завершения операции
	OPERATION_RESULT GetOperationResult() const;

	/// Количество сообщений
	size_t GetMsgCount() const;

	/// Сообщение с индексом ndx
	const std::wstring & GetMsg(size_t ndx) const;

	/// Установить код завершения операции
	void SetOperationResult(OPERATION_RESULT __r);

	/// Добавить сообщение
	void AddMsg(const std::wstring & __m);

	/// Очистить список сообщений
	void ClearMsg();


};

	