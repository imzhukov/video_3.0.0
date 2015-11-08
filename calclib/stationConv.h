#pragma once

#include <string>
#include "VFileName.h"

class VIBDatabase;

/** �������� ����� */
struct VStationFileDescription
{
	VFileName fileName; ///< ��� ��������� �����

	std::wstring roadCode;	///< ����� ������
	std::wstring stationCode; ///< ��� �������
	std::wstring stationName; ///< �������� �������
	std::wstring parkName; ///< �������� �����
	std::wstring wayName; ///< �������� ����

	VFileName inmFile;	///< ��� �������������� �����

//	bool reverse; ///< ��������� ��� ������� disabled
//	bool import; ///< ��������� � ���� ���������������� �����

	void Clear();
	void ClearINM();
	bool IsEmpty() const;

	VStationFileDescription() 
	{
		this->Clear();
		this->ClearINM();
	}

	bool GenerateFileName(const std::wstring & folder);
};

/// ������ ��������� �� ����� INM � ���� 3 ������
std::wstring ImportStationINM(const VStationFileDescription & descr, const std::wstring & vps);


