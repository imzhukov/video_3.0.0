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


/** ��������� ����������� ������ ����: "����1=��������1;����2=��������2 ..."*/
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

/** ��������� ������ ������������� ������������� ���������� �������*/
class VPreview
{
public:
	/** �������� ����������*/
	virtual bool ShowDialog(const wstring& in_title, VIBDataHandlerParameter& in_handler, const wstring& in_fields)=0;
	/** ���������� ������ ���������� �����*/
	virtual vector<bool> GetSelected()=0;
};
typedef boost::shared_ptr<VPreview> VPreviewPtr;

/** ���������� ������� �� ��������� ������� XML*/
class VXMLReportBuilder
{
private:
	
	//����������� ����������� � �������� ������������ �������
	VXMLReportBuilder(const VXMLReportBuilder&){};
	VXMLReportBuilder& operator=(const VXMLReportBuilder&){};
	
	//������

	/// ���������� DOM
	DOMImplementation * impl;	
	/// ������
	boost::shared_ptr<XercesDOMParser> parser;
	/// �������� ��������
	xercesc::DOMDocument * source_document;	
	/// ���������� ������
	boost::shared_ptr<ErrorHandler> error_handler;
	/// ��������� �� �����������
	DOMLSSerializer * serializer;
	/// ��������� �� �������� ������
	DOMLSOutput * output;	
	/// ���� � ����� ������� ���������
	wstring template_path;
	/// �������� ���������
	wstring name;
	/// �������� ������ ����������
	wstring group;	
	/// �������� �������
	wstring filter;
	/// ��� ��������� �����
	wstring out_file_name;
	/// ������
	wstring version;
	/// ������ ����� ������� ����������
	DOMNodeList * parameters;
	/// ������� ����, ��� ������ ���������
	bool builded;
	/// ������� ���� ��� ������� ��������
	bool parameter_changed;
	/// ������� ���� ��� BLOB� ���������� � ��������
	bool embed_pictures;
	/// ������� ����, ��� ���� preview ��������������
	bool preview_results;
	/// ������ BLOB
	size_t blob_index;
	/// ���� � BLOB
	std::wstring blob_path;
	/// ��������
	VProgressInterface * progress;		
	/// ��������� �� ���� ����������� �����������
	VPreviewPtr preview_dialog;
	/// ��������� �� ��
	VIBDatabasePtr database;
	/// ��������� �� ����������
	VIBTransactionPtr transaction;
	
	//������

	/// ���������� ���������
	void ProcessNode(DOMNode * in_node);
	/// ��������� ���� PARAMETER
	void ProcessParameter(DOMNode * in_node);
	/// ��������� ���� CHART
	void ProcessChart(DOMNode * in_node);
	/// ��������� ���� QUERY
	void ProcessQuery(DOMNode * in_node);
	///����������� ���������, ��������� �����������
	static void RemoveComments(DOMNode * in_node);	
	/// ����������� ��������� ���������� ����� ���������� �� ����������
	static void SubstData(DOMNode * in_node, const wstring& alias, const wstring& value);
	/// ��������� ��������
	void BindPicture(DOMNode * new_node, const std::wstring& in_alias, VBlob& blob);
	
public:
	/** �����������*/
	VXMLReportBuilder();
	/** ����������*/
	~VXMLReportBuilder();
	/** �������� �������*/
	void LoadTemplate(const wstring& in_template_path);
	/** �������� �������*/
	void LoadTemplate();
	/** ���������� �������*/
	void SaveTemplate(const wstring& in_template_path);
	/** ���������� �������*/
	void SaveTemplate();
	/** ���������� ����� ������� ����������*/
	size_t GetParameterCount();
	/** ���������� ��� ���������*/
	wstring GetParameterName(size_t in_index);
	/** ���������� �������� ��������� (�� ��������� ������������ �����)*/
	wstring GetParameterDescription(size_t in_index);
	/** ���������� �������� ���������*/
	wstring GetParameterValue(size_t in_index);
	/** ������������� �������� ���������*/
	void SetParameterValue(size_t in_index, const wstring& in_value);
	/** ����������� ������� ��������*/
	void SubstParameter(const wstring& in_name, const wstring& in_value);
	/** ���������� ������ ���������� �������� ��������� �� ����������� � ��*/
	vector<wstring> GetParameterAvail(size_t in_index, VIBDatabasePtr in_database);
	/** ���������� �������� �����*/
	void Build(const wstring& in_output_path, VIBDatabasePtr in_database, VIBTransactionPtr in_transaction, VProgressInterface * in_progress=NULL);
	/** �������*/
	void Reset();
	
	/** ���������� ��������� �� ������ ����������� �����������*/
	void SetDisplayDialog(VPreviewPtr in_preview_dialog);
	/** �������� ��������� �� ������ ����������� �����������*/
	void ResetDisplayDialog();

	/*
	ToDo!!! ���� ����� ��������� ���������� �� �������, �������� � ����� � ����������� � ����
	�������� � ���������� ����������� ����������� �������� � ���������� � ������. � ����� Build ����� ��������
	�� LoadTemplate, Process � SaveReport ��� ���-�� � ���� ����.
	*/	
	/** ���������� ��� ���������*/
	const wstring& GetName();
	/** ���������� ��� ������*/
	const wstring& GetGroup();
	/** ���������� ������*/
	const wstring GetFilter();
	/** ���������� ������ ���� � ����� ���������*/
	const wstring& GetOutFileName();
	/** ���������� ������*/
	const wstring& GetVersion();
	/** ���������� ���� � ����� ������� ���������*/
	const wstring& GetTemplatePath();
};
typedef boost::shared_ptr<VXMLReportBuilder> VXMLReportBuilderPtr;


/// ���������� ������ (������ ������)
class VXMLCommandScript
{
private:

	/// ��������� �� ��
	VIBDatabasePtr database;
	/// ��������� �� ����������
	VIBTransactionPtr transaction;
	/// ��������� �� �����������
	VProgressInterface * progress;
	/// ������
	boost::shared_ptr<XercesDOMParser> parser;
	/// ���������� ������
	boost::shared_ptr<ErrorHandler> error_handler;	

	/// ��������� ����
	void ProcessNode(DOMNode * in_node);
public:
	/// �����������
	VXMLCommandScript();
	/// ����������
	~VXMLCommandScript();
	/// �������� �������
	void LoadScript(const wstring& in_template_path);
	/// ����������
	void Execute(VIBDatabasePtr in_database, VIBTransactionPtr in_transaction, VProgressInterface * in_progress);
};
typedef boost::shared_ptr<VXMLCommandScript> VXMLCommandScriptPtr;

