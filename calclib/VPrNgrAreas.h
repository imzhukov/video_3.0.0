#pragma once

#include "types.h"
#include "VFileName.h"
#include "VParameter.h"
#include "VAprinfo.h"

/// �������� �������
struct VPrNgrAreaDescr
{	
	double begin, end; // ������� � ���.�-���
	VWaySection section; // ������� �������
	std::wstring railsType, railsBrace; // ��� ������� � ����������

	// ����.����.��������
	short maxSpeed1;

	// ���-�������-���� 
	float prLeftMin, prLeftAvg, prLeftMax;
	float prRightMin, prRightAvg, prRightMax;
	float ngrLeftMin, ngrLeftAvg, ngrLeftMax;
	float ngrRightMin, ngrRightAvg, ngrRightMax;
	
	// �-�� ������ ���� 1/60 (1,66667) � ���� 1/12 (8,33333)
	double lenPrLeftUnderEdge, lenPrLeftOverEdge, lenPrRightUnderEdge, lenPrRightOverEdge,
		lenNgrLeftUnderEdge, lenNgrLeftOverEdge, lenNgrRightUnderEdge, lenNgrRightOverEdge; 

	VPrNgrAreaDescr();
};

class VPrNgrAreas : public VIndexList<VPrNgrAreaDescr>
{
public:

	bool Load(const VAprinfo & aprInfo, const VAbsCoordData & absCoord,
		const VParameter & prLeft, const VParameter & prRight,
		const VParameter & ngrLeft, const VParameter & ngrRight);
};

