#pragma once

#include "VParameterDrawing.h"


/** График параметров главного пути с загрузкой из XML*/
class VParameterMainDrawing : public VParameterDrawing
{
private:	
		
	//Данные
	
	

	//Методы

		

public:
	/** Конструктор*/
	VParameterMainDrawing();
	/** Деструктор*/
	virtual ~VParameterMainDrawing();
			
		
	
};
typedef boost::shared_ptr<VParameterMainDrawing> VParameterMainDrawingPtr;