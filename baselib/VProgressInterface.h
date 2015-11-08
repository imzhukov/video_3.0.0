#pragma once

#include "strlib.h"

/* README:
------------------------------------------------------------------------------------
������������ ����� �������� ��������� �� ���������� ������:

	VProgressInterface * prog = GetProgressBox();
	prog->Create(1, L"��������� ������ � ����");
	....
	prog->Destroy();

���� ��� ������ ������������ ������� ������� Destroy(), ���� ���������
������ �� ������.

������������ ����� ������� ��������� ������:

	VProgressBox prog(1, "��������� ������ � ����";
	....

��� ���� ������ ��������� ����� �������� � ����������� ����� ������.
------------------------------------------------------------------------------------
*/

/// ��������� � �������������� "���-�� ���" wxWidgets-������
class VProgressInterface
{
public:

	/// �������� ����
	virtual void Create(int barCount, const wchar_t * title, bool canCancel, void * parent = NULL) = 0;

	/// �������� ����
	virtual void Destroy() = 0;

	/// ��������� ��������� ����
	virtual void SetTitle(const wchar_t * title) = 0;

	/// �������� ���������� ����
	virtual void SetRange(int barNdx, int value) = 0;

	/// ������� �������� ���������� ����
	virtual void SetPosition(int barNdx, int value) = 0;

	/// ����� ������� �� �������
	virtual void Step(int barNdx) = 0;

	/// ����� ������� � ���������� ����
	virtual void SetMessage(int barNdx, const wchar_t * text) = 0;

	/// �������� ��������� ������ Cancel (� ��������������)
	virtual bool QueryCancel() = 0;

	/// ���������� ����
	virtual void Update() = 0;
	
	/// ����� ����
	virtual void Show(bool sh = true) = 0;
};

/// ��������� �� ���������� ��������� ��������-����
extern VProgressInterface * GetProgressBox();

/// ���������������� ��������� � ��������-�����
class VProgressBox
{
	VProgressInterface * prog;

public:

	/// ������� ���� [wchar-������]
	VProgressBox(int barCount, const wchar_t * title, bool canCancel)
	{
		this->prog = GetProgressBox();
		if (this->prog != NULL)
			this->prog->Create(barCount, title, canCancel);
	}

	/// ������� ���� [char-������]
	VProgressBox(int barCount, const char * title, bool canCancel) 
	{
		this->prog = GetProgressBox();
		if (this->prog != NULL)
			this->prog->Create(barCount, string_to_wstring(title).c_str(), canCancel);
	}

	/// ���������� ��������� ���� [wchar-������]
	void SetTitle(const wchar_t * title)
	{
		if (this->prog != NULL)
			this->prog->SetTitle(title);
	}

	/// ���������� ��������� ���� [char-������]
	void SetTitle(const char * title)
	{
		if (this->prog != NULL)
			this->prog->SetTitle(string_to_wstring(title).c_str());
	}

	/// ������� ����
	~VProgressBox()
	{
		if (this->prog != NULL)
			this->prog->Destroy();
	}

	/// ������� ����
	void SetRange(int barNdx, int value)
	{
		if (this->prog != NULL)
			this->prog->SetRange(barNdx, value);
	}

	/// ��������� ����
	void SetPosition(int barNdx, int value)
	{
		if (this->prog != NULL)
			this->prog->SetPosition(barNdx, value);
	}

	/// ����� ������� �� �������
	void Step(int barNdx)
	{
		if (this->prog != NULL)
			this->prog->Step(barNdx);
	}

	/// ������� [wchar-������]
	void SetMessage(int barNdx, const wchar_t * message)
	{
		if (this->prog != NULL)
			this->prog->SetMessage(barNdx, message);
	}

	/// ������� [char-������]
	void SetMessage(int barNdx, const char * message)
	{
		if (this->prog != NULL)
			prog->SetMessage(barNdx, string_to_wstring(message).c_str());
	}

	/// �������� ��������� ������ Cancel (� ��������������)
	bool QueryCancel()
	{
		return this->prog != NULL && this->prog->QueryCancel();
	}

	/// ���������� ����
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
