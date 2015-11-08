#pragma once

#include "VParameter.h"

class VDataSet;

/// �����
class VWear : public VParameter
{

    bool already_tried; /// �������� ���������� �������� ������

public:
	
	/// �����������
	VWear::VWear() : already_tried(false) {}
	
	/// ��������
	bool Load(VDataSet & ds, bool vertical_correction);
};