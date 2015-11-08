#pragma once

#include <valarray>
#include "VDataSet.h"
#include "VParameter.h"
#include "VProgressInterface.h"
#include "parameters.h"
#include "VHtml.h"
#include "VExportImportTextFile.h"

/*static const uint32_t 
	ERROR_WIDTH				= 0x00000002, // ���p�� �������
	ERROR_WIDTH_FRONT		= 0x00000100, // ������ "�� �������"
	ERROR_WIDTH_BACK		= 0x00004000; // ������ "�� �����"*/

//������ ������
enum VERIFYING_LEVEL
{
	VERIFYING_ALL_RIGHT,
	VERIFYING_WARNING,
	VERIFYING_ERROR,
	VERIFYING_NOT_LOAD,
	VERIFYING_NOT_CALC
};

//��������� ����������� ��������� �����
struct VVerifyMark
{
	std::vector<std::string> message;	//���������
	VERIFYING_LEVEL verifyLevel;		//������� ������
};

//������� ����� ��� ����������� �����
class VVerifyFile
{
protected:
	VVerifyMark verifyMark;							//���������, ����������� ������� �����������
	std::vector<std::string> methodsDescription;	//�������� �������� ���������� �������

public:
	//����������� ������
	VVerifyFile()
	{
		needAnotherFile = false;
		allowExecute = false;
		allowVerify = true;
		isExecute = false;
	}
	//���� ������
	VDataSetPtr dataSet;
	//������������� ������� �����
	bool needAnotherFile;
	//����������� �����������
	bool allowExecute;
	//����������� ���������� �������������� ��������
	bool allowVerify;
	//���� ���������� ��������������� ���������� �������
	bool isExecute;
	//�������� �����
	std::string name_test;
	//����� �������� ������
	virtual bool Load() {return true;};
	//�����, ����������� ������� ����������� � ������� ����������� �� ���� �� �����
	virtual void Verify(VProgressBox & prog, bool calc_offset) = 0;
	//�����, ����������� ������� ����������� � ����������� �� ������ ������
	virtual void Verify(VDataSetPtr dataSetAnoth, VProgressBox & prog, bool calc_offset) {};
	//����� ��������������� ���������� �������
	virtual void Execute(VProgressBox & prog) {};
	//����� ��������� ���������� � ������� �����������
	VVerifyMark GetVerifyMark() 
	{
		return verifyMark;
	};
	//����� ������������ ������ � CSV
	virtual void GetCSVString(std::ofstream & output, const std::string divider) {};
	//����� ������������ ���-�� ��������, �������� ����� ��������� ������
	short int GetCountMethod()
	{
		return methodsDescription.size();
	};
	//����� ������������ �������� �������
	std::string GetMethodDescription(short int method_index) 
	{
		return methodsDescription[method_index];
	};
	//������� �������� ����������� ��������� ����������
	virtual bool IsChanged()
	{
		return false;
	};
	//�����, ������������ ������ � ��������� ����������� �����
	virtual std::string GetString()
	{ 
		return "";
	};
};
typedef boost::shared_ptr<VVerifyFile> VVerifyFilePtr; 

//��������� ��� ���� ������� ����������� ����������� �����
class VVerifyFileList : public VIndexList <VVerifyFilePtr>
{
public:
	//���������� � �����
	std::string orientation;	//���������� ������
	std::string date_trail;		//����� �������
	std::string direction;		//����������� ��������
	VDataSetPtr dataSet;		//���� ������

	// ����� �������� ���� ������
	void LoadList(VDataSetPtr _dataSet, VIBDatabasePtr db, VProgressBox & prog);
	// ����� ��� ������ �� ���� ������
	void Verify(VProgressBox & prog, bool calc_offset);
	// ����� ��� ��������������� ���������� ���� ������
	void Execute(VProgressBox & prog);
	// ����� ��� ��������� �������������� ������
	void Refresh(){};
	// ����� ��� ������ ���� ���������� � CSV
	void GetCSVResults(const std::string in_path, FILE_TYPE in_type);
	// ������� ���������� ���� ������������� ����������
	void Save();
	//����� ���������� ����������� ������
	void Update();
}; 

//����� ��������� ����������
class VVerifyParameter : public VVerifyFile
{
protected:
	PARAMETER_TYPE parameterEthalon;    //�������� ���������� ���������
	PARAMETER_TYPE parameterPhased;     //�������� ����������� ���������
	double coefVerifyOld;				//����������� ���������� ����� �� � �� ������� ������
	double coefVerifyNew;				//����������� ���������� �����
	int valueExecute;					//��������, �� ������� ���������� �������� ���������
	double abscoordExecute;				//��������, �� ������� ���������� �������� ��������� � ������
	//����� �������� ������� ����������
	void LoadCorrupt();
	//����� ���������
	int method_index;
public:
	//����������� ������
	VVerifyParameter(PARAMETER_TYPE _parameterPhased, PARAMETER_TYPE _parameterEthalon, VDataSetPtr _dataSet, short int _method_index);
	//����� �������� ������
	virtual bool Load();
	//�����, ����������� ������� ����������� � ������� ����������� �� ���� �� �����
	void Verify(VProgressBox & prog, bool calc_offset);
	//�����, ����������� ������� ����������� � ����������� �� ������ ������
	void Verify(VDataSetPtr dataSetAnoth, VProgressBox & prog, bool calc_offset) {};
	//����� ��������������� ���������� �������
	virtual void Execute(VProgressBox & prog);
	//����� ������������ ������ � CSV
	void GetCSVString(std::ofstream & output, const std::string divider);
	//�������, ������������ ��������� ��������
	PARAMETER_TYPE GetEthalonParameter();
	//�������, ������������ ���������� ��������
	PARAMETER_TYPE GetPhasedParameter();
	//������� �������� ����������� ��������� ����������
	bool IsChanged();
	//�����, ������������ ������ � ��������� ����������� �����
	std::string GetString();
};
typedef boost::shared_ptr<VVerifyParameter> VVerifyParameterPtr;

//����� ����� ��������� �����
class VVerifyKurs : public VVerifyParameter
{
public:
	//����������� ������
	VVerifyKurs(VDataSetPtr _dataSet, short int _method_index);
	//����� ��������������� ���������� ������
	void Execute(VProgressBox & prog);
	//������� �������� ����������� ��������� ����������
	bool IsChanged();
};
typedef boost::shared_ptr<VVerifyKurs> VVerifyKursPtr;

//����� ��������� ���
class VVerifyPPR : public VVerifyParameter
{
public:
	//����������� ������
	VVerifyPPR(PARAMETER_TYPE _parameterPhased, PARAMETER_TYPE _parameterEthalon, VDataSetPtr _dataSet, short int _method_index);
	//����� ��������������� ���������� ������
	void Execute(VProgressBox & prog);
};
typedef boost::shared_ptr<VVerifyPPR> VVerifyPPRPtr;


//����� ������ �� �� � ���������� �� ����������� �������
class VVerifyWidLev : public VVerifyFile
{
	//������� �������� �� � ������ �� ����������� �������� �� ��
	std::vector<VAprinfoControlAreaRecord> apr_control_area;
	//���������, ����������� �� �� � ������ �� ����������� ���������
	struct MsrControlArea
	{
		double mo_width;
		double mo_level;
	};
	//������� �������� �� � ������ �� ����������� �������� �� ����������� ���������
	std::vector<MsrControlArea> msr_control_area;
	//���������� ���������� ��� �������� �������� ��
	double width_range;
	//���������� ���������� ��� �������� �������� ������
	double level_range;
public:
	//����������� ������
	VVerifyWidLev(VDataSetPtr _dataSet, VIBDatabasePtr _db);
	//����� �������� ������
	bool Load(VIBDatabasePtr _db);
	//����� ����������� ������� ��������� � ���������� �� �� ����������� ��������
	void Verify(VProgressBox & prog, bool calc_offset);
	//����� ������������ ������ � CSV
	void GetCSVString(std::ofstream & output, const std::string divider);
	//�������, ������������ ���-�� ��������, �������� ����� ��������� ��������
	short int GetCountMethod()
	{
		return 0;
	};
	//�����, ������������ ������ � ��������� ����������� �����
	std::string GetString();
};
typedef boost::shared_ptr<VVerifyWidLev> VVerifyWidLevPtr;

//����� ����������� ���-�� ������� ������������ �������
class VVerifyPole : public VVerifyFile
{
	int procent;
public:
	//����������� ������
	VVerifyPole(VDataSetPtr _dataSet, VIBDatabasePtr _db);
	//����� �������� ������
	bool Load(VIBDatabasePtr _db);
	//����� ����������� ������� ������� �� �������
	void Verify(VProgressBox & prog, bool calc_offset);
	//����� ������������ ������ � CSV
	void GetCSVString(std::ofstream & output, const std::string divider);
	//�������, ������������ ���-�� ��������, �������� ����� ��������� ��������
	short int GetCountMethod()
	{
		return 0;
	};
	//�����, ������������ ������ � ��������� ����������� �����
	std::string GetString();
};
typedef boost::shared_ptr<VVerifyPole> VVerifyPolePtr;

//����� ����������� ��������� ���������� ������ � �����������
class VVerifyCurves : public VVerifyFile
{
	size_t count_pasport;
	size_t count_fact;
public:
	//����������� ������
	VVerifyCurves(VDataSetPtr _dataSet, VIBDatabasePtr _db);
	//����� �������� ������
	bool Load(VIBDatabasePtr _db);
	//����� ����������� ���������
	void Verify(VProgressBox & prog, bool calc_offset);
	//����� ������������ ������ � CSV
	void GetCSVString(std::ofstream & output, const std::string divider);
	//�������, ������������ ���-�� ��������, �������� ����� ��������� ��������
	short int GetCountMethod()
	{
		return 0;
	};
	//�����, ������������ ������ � ��������� ����������� �����
	std::string GetString();
};
typedef boost::shared_ptr<VVerifyCurves> VVerifyCurvesPtr;

//�����, ����������� ����� �������� ��������� �� ��������� ���������� �������
class VVerifyParameterRange : public VVerifyFile
{
	PARAMETER_TYPE param;
	float high_range;
	float low_range;
public:
	//����������� ������
	VVerifyParameterRange(VDataSetPtr _dataSet, PARAMETER_TYPE _param, float _high_range, float _low_range);
	//����� �������� ������
	bool Load();
	//����� ����������� ���������
	void Verify(VProgressBox & prog, bool calc_offset);
	//����� ������������ ������ � CSV
	void GetCSVString(std::ofstream & output, const std::string divider);
	//�������, ������������ ���-�� ��������, �������� ����� ��������� ��������
	short int GetCountMethod()
	{
		return 0;
	};
	//�����, ������������ ������ � ��������� ����������� �����
	std::string GetString();
};
typedef boost::shared_ptr<VVerifyParameterRange> VVerifyParameterRangePtr;

enum MARKER_TYPE
{
	ANYTHING_MARKER,
	START_MARKER,
	END_MARKER,
	SPALA_MARKER,
	POLE_MARKER,
	CURVE_MARKER,
	ZAZOR_MARKER,
};

struct WayMarker
{
	MARKER_TYPE type;
	std::string name;
	double msr_abscoord;
	double apr_abscoord;
	uint32_t index;
	bool isIgnore;

	bool operator < (WayMarker way_marker)
	{
		return msr_abscoord < way_marker.msr_abscoord;
	}
};

//��������� ���������� � ������� ����� RFID
class VVerifyAbscoord : public VVerifyFile
{
	//������ � �������� �������
	std::vector<WayMarker> wayMarkerData;
	//������ � �������������� ��������
	valarray<double> deltaAbscoord;
	VIBDatabasePtr db;
public:
	//����������� ������
	VVerifyAbscoord(VDataSetPtr _dataSet, VIBDatabasePtr _db);
	//����� �������� ������
	bool Load();

	void Verify(VProgressBox & prog, bool calc_offset);
	//����� ������ ������������������� ���������� ����������
	void Verify();
	//����� ���������� �������
	void Execute();

	/* ������ ��� ������� */
	//���������� ��� ����� ����
	short int GetMarkerCode(size_t in_index);
	//���������� ���-�� ����� �� �������
	size_t GetCount();
	//���������� ���-�� ��������������� ����� �� �������
	size_t GetNotIgnoreCount();
	//���������� ���� ���������� ����� �� ����
	bool IsIgnore(size_t in_index);
	//���������� ����� ��������
	void SetIgnore(size_t in_index);
	void SetIgnore(MARKER_TYPE in_type);
	//����� ����������
	void UnsetIgnore(size_t in_index);
	void UnsetIgnore(MARKER_TYPE in_type);
	//���������� ���������� ����� �� ��� X
	double GetX(size_t in_index);
	//���������� ���������� ��������������� ����� �� ��� X
	double GetNotIgnoreX(size_t in_index);
	//���������� ���������� ����� �� ��� Y
	double GetY(size_t in_index);
	//���������� ���������� ��������������� ����� �� ��� Y
	double GetNotIgnoreY(size_t in_index);
	//���������� ����������� �������� �� ��� Y
	double GetMinY();
	//���������� ������������ �������� �� ��� Y
	double GetMaxY();
	//���������� ������������ �������� �� ��� X
	double GetMaxX();
	//���������� ����������� �������� �� ��� X
	double GetMinX();
};
typedef boost::shared_ptr<VVerifyAbscoord> VVerifyAbscoordPtr;

//����� ����������� �������� �����, ���������� ���������� 
//� ����������� ������
class VVerifyReport
{
	ofstream expTextFile;
public:
	//����������� ������
	VVerifyReport(const wchar_t * reportName);
	//����� ���������� � ��������� ������� � ������������ ������
	void AddRowToReport (const wchar_t * fileName, VIBDatabasePtr CurrentDatabase, VProgressBox & prog, bool addLegend = false);
	//����� ��������� ����������� ���������
	void CloseReport();
};