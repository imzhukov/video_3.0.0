#pragma once
//std
#include <string>
#include <map>
//Boost
#include <boost/scoped_ptr.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/tokenizer.hpp>


//Xerces
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>

//V
#include "VException.h"
#include "dblib.h"
#include "VFileName.h"

#include "VProgressInterface.h"

using namespace xercesc;
using namespace std;


/** Процедура расшифровки строки вида: "ключ1=значение1;ключ2=значение2 ..."*/
class icompare
{
public:
		bool operator()(const wstring& lhs, const wstring& rhs) const
		{
			return boost::ilexicographical_compare(lhs, rhs);
		}
};
typedef map<wstring, wstring, icompare> KeyList;
typedef map<wstring, wstring, icompare>::iterator KeyListItr;
typedef pair<wstring, wstring> Key;
KeyList ParseKeys(const wstring& in_key_string);

/** Интерфейс класса отображающего промежуточные результаты запроса*/
class VPreview
{
public:
	/** Показать результаты*/
	virtual bool ShowDialog(const wstring& in_title, VIBDataHandlerParameter& in_handler, const wstring& in_fields)=0;
	/** Возвращает вектор выделенных строк*/
	virtual vector<bool> GetSelected()=0;
};
typedef boost::shared_ptr<VPreview> VPreviewPtr;

/** Построение отчетов по заданному шаблону XML*/
class VXMLReportBuilder
{
private:
	
	//Конструктор копирования и оператор присваивания закрыты
	VXMLReportBuilder(const VXMLReportBuilder&){};
	VXMLReportBuilder& operator=(const VXMLReportBuilder&){};
	
	//Данные

	/// Реализация DOM
	DOMImplementation * impl;	
	/// Парсер
	boost::shared_ptr<XercesDOMParser> parser;
	/// Исходный документ
	xercesc::DOMDocument * source_document;	
	/// Обработчик ошибок
	boost::shared_ptr<ErrorHandler> error_handler;
	/// Указатель на сериалайзер
	DOMLSSerializer * serializer;
	/// Указатель на выходной объект
	DOMLSOutput * output;	
	/// Путь к файлу шаблона документа
	wstring template_path;
	/// Описание ведомости
	wstring name;
	/// Описание группы ведомостей
	wstring group;	
	/// Описание фильтра
	wstring filter;
	/// Имя выходного файла
	wstring out_file_name;
	/// Версия
	wstring version;
	/// Список тегов входных параметров
	DOMNodeList * parameters;
	/// Признак того, что шаблон развернут
	bool builded;
	/// Признак того что изменен параметр
	bool parameter_changed;
	/// Признак того что BLOBы внедряются в документ
	bool embed_pictures;
	/// Признак того, что теги preview обрабатываются
	bool preview_results;
	/// Индекс BLOB
	size_t blob_index;
	/// Путь к BLOB
	std::wstring blob_path;
	/// Прогресс
	VProgressInterface * progress;		
	/// Указатель на окно отображения результатов
	VPreviewPtr preview_dialog;
	/// Указатель на БД
	VIBDatabasePtr database;
	/// Указатель на транзакцию
	VIBTransactionPtr transaction;
	
	//Методы

	/// Построение ведомости
	void ProcessNode(DOMNode * in_node);
	/// Обработка узла PARAMETER
	void ProcessParameter(DOMNode * in_node);
	/// Обработка узла CHART
	void ProcessChart(DOMNode * in_node);
	/// Обработка узла QUERY
	void ProcessQuery(DOMNode * in_node);
	///Рекурсивная процедура, удаляющая комментарии
	static void RemoveComments(DOMNode * in_node);	
	/// Рекурсивная процедура замещающая имена переменных их значениями
	static void SubstData(DOMNode * in_node, const wstring& alias, const wstring& value);
	/// Вставляет картинку
	void BindPicture(DOMNode * new_node, const std::wstring& in_alias, VBlob& blob);
	
public:
	/** Конструктор*/
	VXMLReportBuilder();
	/** Деструктор*/
	~VXMLReportBuilder();
	/** Загрузка шаблона*/
	void LoadTemplate(const wstring& in_template_path);
	/** Загрузка шаблона*/
	void LoadTemplate();
	/** Сохранение шаблона*/
	void SaveTemplate(const wstring& in_template_path);
	/** Сохранение шаблона*/
	void SaveTemplate();
	/** Возвращает число входных параметров*/
	size_t GetParameterCount();
	/** Возвращает имя параметра*/
	wstring GetParameterName(size_t in_index);
	/** Возвращает описание параметра (по умолчанию соответсвует имени)*/
	wstring GetParameterDescription(size_t in_index);
	/** Возвращает значение параметра*/
	wstring GetParameterValue(size_t in_index);
	/** Устанавливает значение параметра*/
	void SetParameterValue(size_t in_index, const wstring& in_value);
	/** Подставляет внешний параметр*/
	void SubstParameter(const wstring& in_name, const wstring& in_value);
	/** Возвращает список допустимых значений параметра из справочника в БД*/
	vector<wstring> GetParameterAvail(size_t in_index, VIBDatabasePtr in_database);
	/** Построение отчетной формы*/
	void Build(const wstring& in_output_path, VIBDatabasePtr in_database, VIBTransactionPtr in_transaction, VProgressInterface * in_progress=NULL);
	/** Очистка*/
	void Reset();
	
	/** Установить указатель на диалог отображения результатов*/
	void SetDisplayDialog(VPreviewPtr in_preview_dialog);
	/** Сбросить указатель на диалог отображения результатов*/
	void ResetDisplayDialog();

	/*
	ToDo!!! Пока класс допускает построение из шаблона, лежащего в файле с сохранением в файл
	Вероятно в дальнейшем потребуется возможность загрузки и сохранения в потоки. И метод Build будет разделен
	на LoadTemplate, Process и SaveReport или что-то в этом роде.
	*/	
	/** Возвращает имя ведомости*/
	const wstring& GetName();
	/** Возвращает имя группы*/
	const wstring& GetGroup();
	/** Возвращает фильтр*/
	const wstring GetFilter();
	/** Возвращает полный путь к файлу ведомости*/
	const wstring& GetOutFileName();
	/** Возвращает версию*/
	const wstring& GetVersion();
	/** Возвращает путь к файлу шаблона документа*/
	const wstring& GetTemplatePath();
};
typedef boost::shared_ptr<VXMLReportBuilder> VXMLReportBuilderPtr;


/// Выполнение команд (импорт данных)
class VXMLCommandScript
{
private:

	/// Указатель на БД
	VIBDatabasePtr database;
	/// Указатель на транзакцию
	VIBTransactionPtr transaction;
	/// Указатель на прогрессбар
	VProgressInterface * progress;
	/// Парсер
	boost::shared_ptr<XercesDOMParser> parser;
	/// Обработчик ошибок
	boost::shared_ptr<ErrorHandler> error_handler;	

	/// Обработка узла
	void ProcessNode(DOMNode * in_node);
public:
	/// Конструктор
	VXMLCommandScript();
	/// Деструктор
	~VXMLCommandScript();
	/// Загрузка шаблона
	void LoadScript(const wstring& in_template_path);
	/// Выполнение
	void Execute(VIBDatabasePtr in_database, VIBTransactionPtr in_transaction, VProgressInterface * in_progress);
};
typedef boost::shared_ptr<VXMLCommandScript> VXMLCommandScriptPtr;

