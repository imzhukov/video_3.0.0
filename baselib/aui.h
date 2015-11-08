#pragma once

//Callback функции и абстрактный UI

/** Интрефейс диалога сохранения файла*/
class VAUISaveFileDialog
{
public:
	/** Показать диалог*/
	virtual bool ShowModal()=0;
	/** Устанавливает формат расширения*/
	virtual void SetWildcard(const std::wstring& in_wildcard)=0;
	/** Возвращает формат расширения*/
	virtual std::wstring GetWildcard()=0;
	/** Устанавливает путь к файлу*/
	virtual void SetPath(const std::wstring& in_path)=0;
	/** Возвращает путь к файлу*/
	virtual std::wstring GetPath()=0;
};
typedef boost::shared_ptr<VAUISaveFileDialog> VAUISaveFileDialogPtr;

/** Интерфейс диалога выбора элемента из списка*/
class VAUISelectItemDialog
{
public:
	/** Показать диалог*/
	virtual bool ShowModal(const std::wstring& in_caption, const std::wstring& in_column)=0;
	/** Установить список строк*/
	virtual void SetList(const std::vector<std::wstring>& in_list)=0;
	/** Выделяет строку*/
	virtual void SetSelectedItem(const std::wstring& in_value)=0;
	/** Возвращает выбранную строку*/
	virtual std::wstring GetSelectedItem()=0;
	/** Возвращает выбранную позицию*/
	virtual size_t GetSelectedPos()=0;
};
typedef boost::shared_ptr<VAUISelectItemDialog> VAUISelectItemDialogPtr;

/** Интерфейс диалога прогресс-бара*/
class VAUIProgressDialog
{
public:
	/** Показать диалог*/
	virtual void Show(size_t in_bar_count, const std::wstring& in_title)=0;
	/** Скрыть диалог*/
	virtual void Hide()=0;
	/** Установить новые параметры*/
	virtual void Reset(size_t in_bar, size_t in_maximum)=0;
	/** Обновить прогресс*/
	virtual void UpdateProgress(size_t in_bar, size_t in_value)=0;
	/** Обновить текст*/
	virtual void UpdateMessage(size_t in_bar, const std::wstring& in_message)=0;
	/** Запросить флаг cancel*/
	virtual bool Cancel()=0;
};
typedef boost::shared_ptr<VAUIProgressDialog> VAUIProgressDialogPtr;

/** Класс для автоматического скрытия прогресса*/
class VAUIProgressAutoHider
{
private:
	/** Указатель на прогресс*/
	VAUIProgressDialogPtr progress;
public:
	/** Конструктор*/
	VAUIProgressAutoHider(VAUIProgressDialogPtr in_progress) : progress(in_progress) {};
	/** Деструктор*/
	~VAUIProgressAutoHider()
	{
		if (progress) 
			progress->Hide();
	};
};

/** Интерфейс диалога для редактирования списка свойств*/
class VAUIPropertyDialog
{
public:
	/** Показать диалог*/
	virtual bool ShowModal(const std::wstring& in_caption, const std::wstring& in_column)=0;
	/** Возвращает число свойств*/
	virtual size_t GetCount()=0;
	/** Добавляет свойство*/
	virtual void AddItemSimple(const std::wstring& in_description, const std::wstring& in_value)=0;
	/** Добавляет свойство с выпадающим списком*/
	virtual void AddItemList(const std::wstring& in_description, const std::wstring& in_value, 
		const std::vector<std::wstring>& in_avail_list)=0;
	/** Возвращает значение свойства*/
	virtual const std::wstring& GetValue(const std::wstring& in_description)=0;
	/** Очищает список свойств*/
	virtual void Clear()=0;
};
typedef boost::shared_ptr<VAUIPropertyDialog> VAUIPropertyDialogPtr;

/** Интерфейс диалога для ввода текста*/
class VAUIInputDialog
{
public:
	/** Показать диалог*/
	virtual bool ShowModal(const std::wstring& in_caption, const std::wstring& in_value)=0;	
	/** Возвращает значение*/
	virtual const std::wstring& GetValue()=0;	
};
typedef boost::shared_ptr<VAUIInputDialog> VAUIInputDialogPtr;