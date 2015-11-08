#pragma once

#include "VCameraProperties.h"

///����������� �� ���������
VCameraBase::VCameraBase()
{
	cameraName = L"Name of camera is not defined";
	//directory = L"Directory for records is not defined";
	type = RTSP_CAMERA;
}
///����������� � �����������, ��������� ��� ������ � ����� ��� ������
VCameraBase::VCameraBase(int _type, std::wstring Name, int _type_rec)
{
	cameraName = Name;
	//directory = Directory;
	type_rec = _type_rec;
	type = _type;
}
///����������� �����������
VCameraBase::VCameraBase(VCameraBase & src)
{
	type = src.GetType();
	cameraName = src.GetCameraName();
	//directory = src.GetDirectory();
	type_rec = src.GetTypeRec();
}
///����� ��� ��������� ����� ������
std::wstring VCameraBase::GetCameraName()
{
	return cameraName;
}
///����� ��� ��������� ���� � ����� ��� ������
/*std::wstring VCameraBase::GetDirectory()
{
	return directory;
}*/
///����� ��� ��������� ���� ������ �����
int VCameraBase::GetTypeRec()
{
	return type_rec;
}

///����� ��� ��������� ��������� �� ��� ������
std::wstring * VCameraBase::GetCameraNamePtr()
{
	return &cameraName;
}

///����� ��� ��������� ��������� �� ���� � ����� ��� ������
/*std::wstring * VCameraBase::GetDirectoryPtr()
{
	return &directory;
}*/

///����� ��� ��������� ��������� �� ��� ������
int * VCameraBase::GetTypeRecPtr()
{
	return &type_rec;
}

///����� ��� ��������� ���� ������
int VCameraBase::GetType()
{
	return type;
}
///����� ��� ������� ������ ����� ������
void VCameraBase::SetCameraName(std::wstring newCameraName)
{
	cameraName = newCameraName;
}
///����� ��� ������� ����� ����� ��� ������
/*void VCameraBase::SetDirectory(std::wstring newDirectory)
{
	directory = newDirectory;
}*/

bool VCameraBase::SetType(int newType)
{
	type = newType;
	return true;
}
//--------------------------------------- ������ ������ VRTSPCameraProperties ----------------------------------------
/// �����������
VRTSPCameraProperties::VRTSPCameraProperties(const std::wstring& in_name, const std::wstring& in_address, const std::wstring & in_directory, const int & in_type_rec) :
	VCameraBase(RTSP_CAMERA, in_name /*, in_directory*/, in_type_rec)
{
	address = in_address;
}

///����������� �� ������� �� ����� ������������
VRTSPCameraProperties::VRTSPCameraProperties(std::wstring conf_str, int version)
{
	std::vector<std::wstring> split_vec;
	boost::algorithm::split(split_vec, conf_str, boost::is_any_of(L";"));
	switch(version)
	{
		case CAMERA_CONFIG:
		case (CAMERA_CONFIG - 1) :
		{
			try
			{
				if(split_vec.size() == 4)
				{
					type = atoi(wstring_to_string(split_vec[0]).c_str());
					cameraName = split_vec[1];
					address = split_vec[2];
					type_rec = atoi(wstring_to_string(split_vec[3]).c_str());
				}
			}
			catch(std::exception e)
			{
				LOG_ERROR(string_to_wstring(e.what()).c_str());
			}
			break;
		}
		case (CAMERA_CONFIG - 2) :
		{
			try
			{
				if(split_vec.size() == 5)
				{
					type = atoi(wstring_to_string(split_vec[0]).c_str());
					cameraName = split_vec[1];
					address = split_vec[3];
					type_rec = atoi(wstring_to_string(split_vec[4]).c_str());
				}
			}
			catch(std::exception e)
			{
				LOG_ERROR(string_to_wstring(e.what()).c_str());
			}
			break;
		}
		default:
		{
			if(split_vec.size()==4)
			{
				type = atoi(wstring_to_string(split_vec[0]).c_str());
				cameraName = split_vec[1];
				address = split_vec[3];
				type_rec = AVI_RECORDING;		
			}
			else
			{
				type = RTSP_CAMERA;	
				cameraName = L"Name of camera is not defined";
				//directory = L"Directory for records is not defined";
				address = L"rtsp://172.16.1.80:554/channel1";
				type_rec = V4F_RECORDING;
			}
			break;
		}
	}
}

/// ���������� ����� ������
const std::wstring& VRTSPCameraProperties::GetAddress() const
{
	return address;
}

std::wstring * VRTSPCameraProperties::GetAddressPtr()
{
	return &address;
}

///��������� ������ ��� ����� ������������
std::wstring VRTSPCameraProperties::GetConfigStr()
{
	wchar_t str [1000]=L"";
	_snwprintf(str, 999, L"%i;%s;%s;%i", type, cameraName.c_str(), address.c_str(), type_rec);
	return std::wstring(str);
}

VJAICameraProperties::VJAICameraProperties(const std::wstring& in_name) :
	VCameraBase(JAI_CAMERA, in_name, V4F_RECORDING)
{}

VJAICameraProperties::VJAICameraProperties(std::wstring conf_str, int version)
{
	std::vector<std::wstring> split_vec;
	boost::algorithm::split(split_vec, conf_str, boost::is_any_of(L";"));
	switch(version)
	{
		case CAMERA_CONFIG:
		{
			type = atoi(wstring_to_string(split_vec[0]).c_str());
			cameraName = split_vec[1];
			//directory = split_vec[2];
			numCamera = _wtoi(split_vec[3].c_str());
			break;
		}
		case 1:
		{
			type = atoi(wstring_to_string(split_vec[0]).c_str());
			cameraName = split_vec[1];
			//directory = split_vec[2];
			break;
		}
	}
}

///��������� ������ ��� ����� ������������
std::wstring VJAICameraProperties::GetConfigStr()
{
	wchar_t str [1000]=L"";
	_snwprintf(str, 999, L"%i;%s;%i", type, cameraName.c_str(), numCamera);
	return std::wstring(str);
}

int * VJAICameraProperties::GetNumCameraPtr()
{
	return &numCamera;
}