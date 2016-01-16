#pragma once

#include <boost/asio.hpp>

#include <string> 
#include "strlib.h"
#include "VLog.h"
#include "options.h"

//Boost
#include <boost/format.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/split.hpp>

#define AVI_RECORDING 80301
#define PNG_RECORDING 80302
#define V4F_RECORDING 80303

#define RTSP_CAMERA 5600
#define JAI_CAMERA 5601
#define FIREWIRE_CAMERA 5602

#define CAMERA_CONFIG 3L

///����� ������� ������
class VCameraBase {
protected:
	///��� ������
	int type;
	///��� ������
	std::wstring cameraName;
	///����� ��� ������
	//std::wstring directory;
	///����� ������ ������
	int type_rec;
public:
	///����������� �� ���������
	VCameraBase();
	///����������� �����������
	VCameraBase(VCameraBase & src);
	///����������� � �����������
	VCameraBase(int _type, std::wstring Name, int _type_rec);
	///����������� ����������
	virtual ~ VCameraBase(){}
	///����� ��� ��������� ����� ������
	std::wstring GetCameraName();
	///����� ��� ��������� ���� � ����� ��� ������
	//std::wstring GetDirectory();
	///����� ��� ��������� ���� ������ �����
	int GetTypeRec();
	///����� ��� ��������� ���� ������
	int GetType();

	///����� ��� ��������� ��������� �� ��� ������
	std::wstring * GetCameraNamePtr();
	///����� ��� ��������� ��������� �� ���� � ����� ��� ������
	//std::wstring * GetDirectoryPtr();
	///����� ��� ��������� ��������� �� ��� ������
	int * GetTypeRecPtr();

	///����� ��� ������� ������ ����� ������
	void SetCameraName(std::wstring newCameraName );
	///����� ��� ������� ����� ����� ��� ������
	void SetDirectory(std::wstring newDirectory);
	///����� ��� ������� ���� ������
	bool SetType(int newType);
	///������� ������ ��� ����� ������������
	virtual std::wstring GetConfigStr() = 0;
};

/// ����� ������� IP ������
class VRTSPCameraProperties : public VCameraBase
{
	/// ����� ������
	std::wstring address;
public:
	/// �����������
	VRTSPCameraProperties(const std::wstring& in_name, const std::wstring& in_address, const std::wstring & in_directory, const int & in_type_rec);
	/// ����������� �� ������� �� ����� ������������
	VRTSPCameraProperties(std::wstring conf_str, int version);
	/// ����������
	virtual ~VRTSPCameraProperties(){};
	/// ���������� ����� ������
	const std::wstring& GetAddress() const;
	///����� ��� ��������� ��������� �� ����� ������
	std::wstring * GetAddressPtr();
	///������� ������ ��� ����� ������������
	std::wstring GetConfigStr();	
};

//����� ������� ����������� �����
class VJAICameraProperties : public VCameraBase
{
	/// ����� ������
	int numCamera;
public:
	/// �����������
	VJAICameraProperties(const std::wstring & in_name);
	/// ����������� �� ������� �� ����� ������������
	VJAICameraProperties(std::wstring conf_str, int version);
	/// ����������
	virtual ~VJAICameraProperties(){};
	///����� ��� ��������� ��������� �� ����� ������
	int * GetNumCameraPtr();
	///������� ������ ��� ����� ������������
	std::wstring GetConfigStr();
};