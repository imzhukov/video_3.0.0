#pragma once

#include "VDataSet.h"
#include "VParameter.h"
#include "VAprinfo.h"
#include "VOpMark.h"

enum POINT_TYPE 
{
	POINT_BRIDGE = 1,
	POINT_POLE = 4,
	POINT_SPALA_MARKER = 10,
	POINT_POLE_ACCURATE = 20
};

/*
���� - 1
����� - 2
����. ����� - 3
�� ����� - 4
����� - 5
������ ����� - 6
������ ������ - 7
���������� ����� - 8
���������� ������ - 9
����� ������� - 10
���������� ������������ ����� - 20
*/

struct VCoordPoint 
{
	std::wstring filename;		 // ��� �����, � ������� ��������� ������ ������ ����
	std::wstring id;			 // ������������� �������
	double abscoord;			 // ���������� ����������
	double apr_abscoord;		 // ��������� ����������
	size_t index;				 // ������ �� ������� ������ � �����
	POINT_TYPE type;			 // ��� ������� ����
	bool isIgnore;				 // ������������ (true) ���������
	int row;					 // ����� ������ � �����
	int col;					 // ����� ������� � �����

	bool operator<(const VCoordPoint & coordpoint2)
	{
		return (abscoord < coordpoint2.abscoord);
	}

	// ������� ����������� � ������� �������, �� ����� ������
	bool Fits(size_t kadrCount) const { return true; }
};

class VCoordPointList : public VIndexList<VCoordPoint>
{
public:
	/// ��������
	bool Load(VDataSet & ds, POINT_TYPE type, bool load_apr_abscoord = false);
	// ��������
	void Unload(VDataSet & ds);
	// ��������� ������������� ��������
	double GetMax(POINT_TYPE in_type) const;
	// ��������� ������������� ��������
	double GetMin(POINT_TYPE in_type) const;
	// ���������� ������ �������� �������������� �������� ���������� ����
	// std::vector<size_t> GetPoint(POINT_TYPE in_type);
	// ����� �������� ���������� �� ���������� ����� � ������� ����� � ����� �����
	double CalcDelta(POINT_TYPE in_type, VCoordPointList coord_point_list);
	// ������ ������� �������� ���������� ��������� ������������ �������
	double GetAprAbscoord(double abscoord, VAprinfo aprinfo);
	// ������� ������
	void DeletePoint(size_t in_index);
};

//---------------����� ��� ������������� �� ��������� �������� ����-----------------------------------
class VCoordPointAutoCorr : public VCoordPointList
{
	//��������� ������ �� ����������� ��. �������
	line_t line_spala_marker;
	line_t line_pole;
	std::map<POINT_TYPE, size_t> start_index; //������ ����� ������ � ��������� ���� (��� ��������� ������� � ������)
public:
	// ���������� ��������������� ������
	void CalcLine(POINT_TYPE in_type);
	// ��������� ��������������� ������
	line_t GetLine(POINT_TYPE in_type);
	// ����������� ������
	void SetCorrupt(VDataSet & dataSet, POINT_TYPE type, size_t index);
	// ����� ����������
//	void UnsetCorrupt(VDataSet dataSet, size_t index);
	//������������� �����, ������������ �������� �� ������� � �� ���� �����
	VCoordPoint & GetPoint(POINT_TYPE in_type, size_t index);
	//������������� �����, ������������ ���-�� ���������, ��������������� ������� ���� �����
	size_t GetCountPoint(POINT_TYPE in_type);
	//����� ������������ ���-�� ��������� �����
	size_t GetCountNotCorrupt(POINT_TYPE in_type);
};

//---------------����� ��� ������ �������� �����------------------------------------------------------
class VRepPointList : public VCoordPointList
{
	// ������ ������� �������� �� �������
	std::vector<double> average;
	// ������ ��� �� �������
	std::vector<double> sko;
public:
	// ���������� ��� Grid 
	void SortForGrid(std::map<std::wstring,int> Filenames);
	// ���������� ������������ ���
	double GetMaxSKO();
	// ���������� ������� �� ������
	double GetAverage(int row);
	// ���������� ��� �� ������
	double GetSKO(int row);
	// ���������� ���������
	void ReCalc(double porog);
	// �������� �������� �������������� ������ ������ � �������
	VCoordPoint GetPoint(int row, int col);
	// ���������� �������� �������������� ������ ������ � �������
	void SetPoint(int row, int col);
	// ������������ �������� ��������� �������
	void Reload();
	// ��������� ��������� ���������� ������
	bool FirstCheckData();
};