#pragma once

#include "strlib.h"

/* README:
------------------------------------------------------------------------------------
Пользователь может получить указатель на глобальный объект:

	VProgressInterface * prog = GetProgressBox();
	prog->Create(1, L"Занесение кривых в базу");
	....
	prog->Destroy();

Если при выходе пользователь забудет сказать Destroy(), окно останется
висеть на экране.

Пользователь может создать локальный объект:

	VProgressBox prog(1, "Занесение кривых в базу";
	....

При этом окошко прогресса будет спрятано в деструкторе этого класса.
------------------------------------------------------------------------------------
*/

/// Интерфейс к разрисованному "где-то там" wxWidgets-классу
class VProgressInterface
{
public:

	/// Создание окна
	virtual void Create(int barCount, const wchar_t * title, bool canCancel, void * parent = NULL) = 0;

	/// Удаление окна
	virtual void Destroy() = 0;

	/// Установка заголовка окна
	virtual void SetTitle(const wchar_t * title) = 0;

	/// Максимум указанного бара
	virtual void SetRange(int barNdx, int value) = 0;

	/// Текущее значение указанного бара
	virtual void SetPosition(int barNdx, int value) = 0;

	/// Сдвиг позиции на единицу
	virtual void Step(int barNdx) = 0;

	/// Текст подписи к указанному бару
	virtual void SetMessage(int barNdx, const wchar_t * text) = 0;

	/// Проверка состояния кнопки Cancel (с подтверждением)
	virtual bool QueryCancel() = 0;

	/// Обновление окна
	virtual void Update() = 0;
	
	/// Показ окна
	virtual void Show(bool sh = true) = 0;
};

/// Указатель на статически созданный прогресс-бокс
extern VProgressInterface * GetProgressBox();

/// Пользовательский интерфейс к прогресс-боксу
class VProgressBox
{
	VProgressInterface * prog;

public:

	/// Создать окно [wchar-строки]
	VProgressBox(int barCount, const wchar_t * title, bool canCancel)
	{
		this->prog = GetProgressBox();
		if (this->prog != NULL)
			this->prog->Create(barCount, title, canCancel);
	}

	/// Создать окно [char-строки]
	VProgressBox(int barCount, const char * title, bool canCancel) 
	{
		this->prog = GetProgressBox();
		if (this->prog != NULL)
			this->prog->Create(barCount, string_to_wstring(title).c_str(), canCancel);
	}

	/// Установить заголовок окна [wchar-строки]
	void SetTitle(const wchar_t * title)
	{
		if (this->prog != NULL)
			this->prog->SetTitle(title);
	}

	/// Установить заголовок окна [char-строки]
	void SetTitle(const char * title)
	{
		if (this->prog != NULL)
			this->prog->SetTitle(string_to_wstring(title).c_str());
	}

	/// Удалить окно
	~VProgressBox()
	{
		if (this->prog != NULL)
			this->prog->Destroy();
	}

	/// Границы бара
	void SetRange(int barNdx, int value)
	{
		if (this->prog != NULL)
			this->prog->SetRange(barNdx, value);
	}

	/// Положение бара
	void SetPosition(int barNdx, int value)
	{
		if (this->prog != NULL)
			this->prog->SetPosition(barNdx, value);
	}

	/// Сдвиг позиции на единицу
	void Step(int barNdx)
	{
		if (this->prog != NULL)
			this->prog->Step(barNdx);
	}

	/// Подпись [wchar-строки]
	void SetMessage(int barNdx, const wchar_t * message)
	{
		if (this->prog != NULL)
			this->prog->SetMessage(barNdx, message);
	}

	/// Подпись [char-строки]
	void SetMessage(int barNdx, const char * message)
	{
		if (this->prog != NULL)
			prog->SetMessage(barNdx, string_to_wstring(message).c_str());
	}

	/// Проверка состояния кнопки Cancel (с подтверждением)
	bool QueryCancel()
	{
		return this->prog != NULL && this->prog->QueryCancel();
	}

	/// Обновление окна
	void Update()
	{
		if (this->prog != NULL)
			prog->Update();
	}
	
	void Show(bool sh = true)
	{
		if(this->prog != NULL)
			prog->Show(sh);
	}
};
