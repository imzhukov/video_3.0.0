#pragma once

#include "VParameter.h"

class VDataSet;

/// Износ
class VWear : public VParameter
{

    bool already_tried; /// Избегать повторного логгинга ошибок

public:
	
	/// Конструктор
	VWear::VWear() : already_tried(false) {}
	
	/// Загрузка
	bool Load(VDataSet & ds, bool vertical_correction);
};