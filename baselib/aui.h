#pragma once

//Callback ������� � ����������� UI

/** ��������� ������� ���������� �����*/
class VAUISaveFileDialog
{
public:
	/** �������� ������*/
	virtual bool ShowModal()=0;
	/** ������������� ������ ����������*/
	virtual void SetWildcard(const std::wstring& in_wildcard)=0;
	/** ���������� ������ ����������*/
	virtual std::wstring GetWildcard()=0;
	/** ������������� ���� � �����*/
	virtual void SetPath(const std::wstring& in_path)=0;
	/** ���������� ���� � �����*/
	virtual std::wstring GetPath()=0;
};
typedef boost::shared_ptr<VAUISaveFileDialog> VAUISaveFileDialogPtr;

/** ��������� ������� ������ �������� �� ������*/
class VAUISelectItemDialog
{
public:
	/** �������� ������*/
	virtual bool ShowModal(const std::wstring& in_caption, const std::wstring& in_column)=0;
	/** ���������� ������ �����*/
	virtual void SetList(const std::vector<std::wstring>& in_list)=0;
	/** �������� ������*/
	virtual void SetSelectedItem(const std::wstring& in_value)=0;
	/** ���������� ��������� ������*/
	virtual std::wstring GetSelectedItem()=0;
	/** ���������� ��������� �������*/
	virtual size_t GetSelectedPos()=0;
};
typedef boost::shared_ptr<VAUISelectItemDialog> VAUISelectItemDialogPtr;

/** ��������� ������� ��������-����*/
class VAUIProgressDialog
{
public:
	/** �������� ������*/
	virtual void Show(size_t in_bar_count, const std::wstring& in_title)=0;
	/** ������ ������*/
	virtual void Hide()=0;
	/** ���������� ����� ���������*/
	virtual void Reset(size_t in_bar, size_t in_maximum)=0;
	/** �������� ��������*/
	virtual void UpdateProgress(size_t in_bar, size_t in_value)=0;
	/** �������� �����*/
	virtual void UpdateMessage(size_t in_bar, const std::wstring& in_message)=0;
	/** ��������� ���� cancel*/
	virtual bool Cancel()=0;
};
typedef boost::shared_ptr<VAUIProgressDialog> VAUIProgressDialogPtr;

/** ����� ��� ��������������� ������� ���������*/
class VAUIProgressAutoHider
{
private:
	/** ��������� �� ��������*/
	VAUIProgressDialogPtr progress;
public:
	/** �����������*/
	VAUIProgressAutoHider(VAUIProgressDialogPtr in_progress) : progress(in_progress) {};
	/** ����������*/
	~VAUIProgressAutoHider()
	{
		if (progress) 
			progress->Hide();
	};
};

/** ��������� ������� ��� �������������� ������ �������*/
class VAUIPropertyDialog
{
public:
	/** �������� ������*/
	virtual bool ShowModal(const std::wstring& in_caption, const std::wstring& in_column)=0;
	/** ���������� ����� �������*/
	virtual size_t GetCount()=0;
	/** ��������� ��������*/
	virtual void AddItemSimple(const std::wstring& in_description, const std::wstring& in_value)=0;
	/** ��������� �������� � ���������� �������*/
	virtual void AddItemList(const std::wstring& in_description, const std::wstring& in_value, 
		const std::vector<std::wstring>& in_avail_list)=0;
	/** ���������� �������� ��������*/
	virtual const std::wstring& GetValue(const std::wstring& in_description)=0;
	/** ������� ������ �������*/
	virtual void Clear()=0;
};
typedef boost::shared_ptr<VAUIPropertyDialog> VAUIPropertyDialogPtr;

/** ��������� ������� ��� ����� ������*/
class VAUIInputDialog
{
public:
	/** �������� ������*/
	virtual bool ShowModal(const std::wstring& in_caption, const std::wstring& in_value)=0;	
	/** ���������� ��������*/
	virtual const std::wstring& GetValue()=0;	
};
typedef boost::shared_ptr<VAUIInputDialog> VAUIInputDialogPtr;