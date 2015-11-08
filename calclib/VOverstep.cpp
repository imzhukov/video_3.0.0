#include "VOverstep.h"

#include <algorithm>

#include "VMainFile.h"
#include "VRemoteFile.h"

std::string GetOverstepName(int type)
{
	switch(type)
	{
	case ovrWidthPl:	return "Уширение";
	case ovrWidthMn:	return "Сужение";
	case ovrLevel:		return "Плавное отступление по уровню";
	case ovrDistort:	return "Перекос по уровню";
	case ovrPitR:		return "Просадка правая";
	case ovrPitL:		return "Просадка левая";
	case ovrRichR:		return "Рихтовка правая";
	case ovrRichL:		return "Рихтовка левая";
	case ovrOtvod:		return "Отвод ширины колеи";
	case ovrDistShort:	return "Перекос на базе тележки";
	case ovrDistLong:	return "Перекос на базе вагона";

	case ovrAnp:		return "Непогашенное ускорение";
	case ovrKsi:		return "Скорость нарастания непогашенного ускорения";
	case ovrOtvodH:		return "Отвод возвышения";
	case ovrZazor:		return "Зазор";
	case ovrIznos:		return "Износ";
	case ovrRich:		return "Рихтовка по рихтовочной нити";
	case ovrRichN:		return "Рихтовка по нерихтовочной нити";

	case ovrZazorR:		return "Зазор левый";
	case ovrZazorL:		return "Зазор правый";
	default:			return "";
	}
}

// доп.код неисправности: импорт из структур v0 и v1
void VOvrAddCode::Import(long type, long comb, char objectApr)
{
	this->Init();

	if (IsFlag(comb, decNewObject))
	{
		this->C        = IsFlag(comb, decC);
		this->PP       = IsFlag(comb, decPP);
		this->ThreePr  = IsFlag(comb, decThreePr);
		this->PrjRich  = IsFlag(comb, decPrjRich);
		this->PrjP     = IsFlag(comb, decPrjP);
		this->PrjPitR  = IsFlag(comb, decPrjPitR);
		this->PrjPitL  = IsFlag(comb, decPrjPitL);
		this->PrjRP    = IsFlag(comb, decPrjRP);
		this->PrjRPitR = IsFlag(comb, decPrjRPitR);
		this->PrjRPitL = IsFlag(comb, decPrjRPitL);
		this->Bridge   = IsTypeBridgeLimit(type) && IsFlag(comb, decBridge);
		this->Tonnel   = IsTypeBridgeLimit(type) && IsFlag(comb, decTonnel); 
		this->Switch   = IsTypeOptics(type) && IsFlag(comb, decSwitch);
	}
	else
	{
		this->C        = IsFlag(comb, decCombine);
		this->PP       = IsFlag(comb, decSequence);
		this->Bridge   = objectApr == 1;
		this->Tonnel   = objectApr == 2;
	}
}

int VOvrAddCode::Export()
{
	int value = decNewObject;
	if (this->C)		value |= decC;
	if (this->PP)		value |= decPP;
	if (this->ThreePr)	value |= decThreePr;
	if (this->PrjRich)	value |= decPrjRich;
	if (this->PrjP)		value |= decPrjP;
	if (this->PrjPitR)	value |= decPrjPitR;
	if (this->PrjPitL)	value |= decPrjPitL;
	if (this->PrjRP)	value |= decPrjRP;
	if (this->PrjRPitR)	value |= decPrjRPitR;
	if (this->PrjRPitL)	value |= decPrjRPitL;
	if (this->Bridge)	value |= decBridge;
	if (this->Tonnel)	value |= decTonnel;
	if (this->Switch)	value |= decSwitch;
	return value;
}

// отступление соответствует маске
bool VOverstep2::MeetsMask(unsigned long __mask) const
{
	// отступления с ограничением скорости
	if (IsFlag(__mask, ovmLimit) && this->IsLimit()) return true;

	// отступления данной степени
	else if (IsFlag(__mask, ovmDeg4) && this->degree == 4) return true;
	else if (IsFlag(__mask, ovmDeg3) && this->degree == 3) return true;
	else if (IsFlag(__mask, ovmDeg2) && this->degree == 2) return true;

	// отступления с превышением длины
	else if (IsFlag(__mask, ovmOverLen) && 
		this->degree >= 5 && this->degree <= 7) return true;

	// сбои измерений
	else if (IsFlag(__mask, ovmHoles) && 
		this->type >= ovrHoleSh && this->type <= ovrHoleRhL) return true;

	// стрелки
	else if (IsFlag(__mask, ovmSwitch) && 
		this->type >= ovrStrelka && this->type <= ovrStrXOptRt)
			return true;

	// всякая муть
	else if (IsFlag(__mask, ovmTech) && (this->type == ovrSpeed1 || 
		this->type == ovrSpeed2 || this->type == ovrKmChange)) return true;

	// нэ трэба!
	else return false;
}

// отступление: импорт из структуры версии 0
void VOverstep2::Import(const VOverstepOnDiskV0 & src)
{
	this->Init();
	this->version = 0;

	this->absCoord = src.abs;
	this->wayCoord.km = src.km;
	this->wayCoord.m = src.m;
	this->absCoordOrig = src.abs;
	this->wayCoordOrig.km = src.km;
	this->wayCoordOrig.m = src.m;
	this->isOrigCoord = false;

	this->type = src.type;
	this->typeOrig = ovrUnknown;
	this->isOrigType = false;

	this->value = float(src.value / 10.0);
	this->norma = float(src.norma / 10.0);
	this->len = src.len;
	this->number = src.number;

	if (this->type == ovrWidthPl || this->type == ovrWidthMn)	this->numElem = int(ceil(this->len /4.0));
	else if (this->type == ovrLevel)							this->numElem = int(ceil(this->len / 10.0));
	else														this->numElem = 1;

	this->degree = src.degree;

	this->code.Import(src.type, src.comb, src.objectApr);

	this->setSpd = VSpeeds(src.speedSet1, src.speedSet2, src.speedSet2);
	this->limSpd = VSpeeds(src.speedLim1, src.speedLim2, src.speedLim2);
	this->isThirdSpeed = false;

	// ГРЯЗНЫЙ-ГРЯЗНЫЙ ХАК 
	// НУ, ЗАБЫЛ Я ПОЛОЖИТЬ ПРИЗНАК СТРЕЛКИ !!!!!!!

		// если у нас на отступлении 4 степени нет ограничения, 
	// наверное, мы его сняли из-за стрелки
	if (IsTypeOptics(this->type) && this->degree == 4 && !this->IsLimit())
	{
		// наверное, подразумевалась стрелка :)
		this->code.Switch = true;
	}

}

// импорт из структуры версии 1
void VOverstep2::Import(const VOverstepOnDiskV1 & src)
{
	this->Init();
	this->version = 1;

	this->absCoord = src.rabs;
	this->wayCoord.km = src.rkm;
	this->wayCoord.m = src.rm;
	this->absCoordOrig = src.abs;
	this->wayCoordOrig.km = src.km;
	this->wayCoordOrig.m = src.m;
	this->isOrigCoord = true;

	this->value = src.value;
	this->norma = src.norma;
	this->len = src.len;
	this->degree = src.degree;
	this->number = src.number;

	this->type = src.type;
	this->typeOrig = src.typeOrig;
	this->isOrigType = true;

	if (this->type == ovrWidthPl || this->type == ovrWidthMn)	this->numElem = int(ceil(this->len /4.0));
	else if (this->type == ovrLevel)							this->numElem = int(ceil(this->len / 10.0));
	else														this->numElem = 1;

	this->code.Import(src.type, (src.comb | decNewObject), 0);

	this->setSpd = VSpeeds(src.speedSet1, src.speedSet2, src.speedSet3);
	this->limSpd = VSpeeds(src.speedLim1, src.speedLim2, src.speedLim3);
	this->isThirdSpeed = true;
}

// Баллы начисляемые за отступление
int VOverstep2::GetPenalty() const
{
	int penalty = 0;

	switch (this->type)
	{
		// Уширение, сужение
	case ovrWidthPl: 
	case ovrWidthMn:

		if (this->type == ovrWidthPl) // Уширение
		{
			if (this->norma == 1520)
			{
				if (this->setSpd.GetPassSpeed() > 120)
				{
					if (this->value >= 1529.0 && this->value < 1530.0)		penalty = 1;
					else if (this->value >= 1530.0 && this->value < 1531.0)	penalty = 2;
					else if (this->value >= 1531.0 && this->value < 1532.0)	penalty = 18;
					else if (this->value >= 1532.0 && this->value < 1533.0)	penalty = 24;
					else if (this->value >= 1533.0)							penalty = 100;
				}
				else if (this->setSpd.GetPassSpeed() > 60)
				{
					if (this->value >= 1529.0 && this->value < 1531.0)		penalty = 1;
					else if (this->value >= 1531.0 && this->value < 1533.0)	penalty = 2;
					else if (this->value >= 1533.0 && this->value < 1534.0) penalty = 3;
					else if (this->value >= 1534.0 && this->value < 1535.0)	penalty = 5;
					else if (this->value >= 1535.0 && this->value < 1537.0)	penalty = 18;
					else if (this->value >= 1537.0 && this->value < 1538.0) penalty = 24;
					else if (this->value >= 1538.0 && this->value < 1539.0) penalty = 32;
					else if (this->value >= 1539.0)							penalty = 100;
				}
				else if (this->setSpd.GetPassSpeed() > 25)
				{
					if (this->value >= 1529.0 && this->value < 1535.0)		penalty = 1; 
					else if (this->value >= 1535.0 && this->value < 1538.0)	penalty = 2; 
					else if (this->value >= 1538.0 && this->value < 1540.0) penalty = 3; 
					else if (this->value >= 1540.0 && this->value < 1541.0)	penalty = 5; 
					else if (this->value >= 1541.0 && this->value < 1543.0)	penalty = 18; 
					else if (this->value >= 1543.0 && this->value < 1545.0) penalty = 24; 
					else if (this->value >= 1545.0 && this->value < 1546.0) penalty = 32; 
					else if (this->value >= 1546.0)							penalty = 100;
				}
				else // if (this->setSpd.GetPassSpeed() <= 25)
				{
					if (this->value >= 1531.0 && this->value < 1540.0)		penalty = 1; 
					else if (this->value >= 1540.0 && this->value < 1543.0)	penalty = 2; 
					else if (this->value >= 1543.0 && this->value < 1546.0) penalty = 3; 
					else if (this->value >= 1546.0 && this->value < 1547.0)	penalty = 5; 
					else if (this->value >= 1547.0 && this->value < 1548.0)	penalty = 18;
					else if (this->value >= 1548.0 && this->value < 1549.0) penalty = 24;
					else if (this->value >= 1549.0)							penalty = 100;
				}
			}
			else if (this->norma == 1524.0)
			{
				if (this->setSpd.GetPassSpeed() > 120)
				{
					if (this->value >= 1533.0 && this->value < 1534.0)		penalty = 1; 
					else if (this->value >= 1534.0 && this->value < 1535.0)	penalty = 2; 
					else if (this->value >= 1535.0 && this->value < 1536.0)	penalty = 18;
					else if (this->value >= 1536.0 && this->value < 1537.0) penalty = 24;
					else if (this->value >= 1537.0)							penalty = 100;
				}
				else if (this->setSpd.GetPassSpeed() > 60)
				{
					if (this->value >= 1533.0 && this->value < 1534.0)		penalty = 1; 
					else if (this->value >= 1534.0 && this->value < 1535.0)	penalty = 2; 
					else if (this->value >= 1535.0 && this->value < 1536.0) penalty = 3; 
					else if (this->value >= 1536.0 && this->value < 1537.0)	penalty = 5; 
					else if (this->value >= 1537.0 && this->value < 1539.0)	penalty = 18;
					else if (this->value >= 1539.0 && this->value < 1540.0) penalty = 24;
					else if (this->value >= 1540.0 && this->value < 1541.0) penalty = 32;
					else if (this->value >= 1541.0)							penalty = 100;
				}
				else if (this->setSpd.GetPassSpeed() > 25)
				{
					if (this->value >= 1533.0 && this->value < 1536.0)		penalty = 1; 
					else if (this->value >= 1536.0 && this->value < 1538.0)	penalty = 2; 
					else if (this->value >= 1538.0 && this->value < 1540.0) penalty = 3; 
					else if (this->value >= 1540.0 && this->value < 1541.0)	penalty = 5; 
					else if (this->value >= 1537.0 && this->value < 1539.0)	penalty = 18;
					else if (this->value >= 1539.0 && this->value < 1540.0) penalty = 24;
					else if (this->value >= 1540.0 && this->value < 1541.0) penalty = 32;
					else if (this->value >= 1541.0)							penalty = 100;
				}
				else 
				{
					if (this->value >= 1535.0 && this->value < 1540.0)		penalty = 1; 
					else if (this->value >= 1540.0 && this->value < 1543.0)	penalty = 2; 
					else if (this->value >= 1543.0 && this->value < 1546.0) penalty = 3; 
					else if (this->value >= 1546.0 && this->value < 1547.0) penalty = 5; 
					else if (this->value >= 1547.0 && this->value < 1548.0) penalty = 18; 
					else if (this->value >= 1548.0 && this->value < 1549.0) penalty = 24; 
					else if (this->value >= 1549.0)							penalty = 100; 
				}
			}
			else if (this->norma == 1530.0)
			{
				if (this->setSpd.GetPassSpeed() > 120)
				{
					if (this->value >= 1537.0 && this->value < 1538.0)		penalty = 1; 
					else if (this->value >= 1538.0 && this->value < 1539.0)	penalty = 2; 
					else if (this->value >= 1539.0 && this->value < 1540.0)	penalty = 18; 
					else if (this->value >= 1540.0 && this->value < 1541.0)	penalty = 24; 
					else if (this->value >= 1541.0)							penalty = 100; 
				}
				else if (this->setSpd.GetPassSpeed() > 25)
				{
					if (this->value >= 1539.0 && this->value < 1542.0)		penalty = 1; 
					else if (this->value >= 1542.0 && this->value < 1543.0)	penalty = 2; 
					else if (this->value >= 1543.0 && this->value < 1544.0)	penalty = 3; 
					else if (this->value >= 1544.0 && this->value < 1545.0) penalty = 5; 
					else if (this->value >= 1545.0 && this->value < 1546.0) penalty = 18; 
					else if (this->value >= 1546.0 && this->value < 1547.0) penalty = 24; 
					else if (this->value >= 1547.0)							penalty = 100; 
				}
				else // if (this->setSpd.spd1 <= 25)
				{
					if (this->value >= 1541.0 && this->value < 1544.0)		penalty = 1; 
					else if (this->value >= 1544.0 && this->value < 1545.0)	penalty = 2; 
					else if (this->value >= 1545.0 && this->value < 1546.0)	penalty = 3; 
					else if (this->value >= 1546.0 && this->value < 1547.0)	penalty = 5; 
					else if (this->value >= 1547.0 && this->value < 1548.0)	penalty = 18; 
					else if (this->value >= 1548.0 && this->value < 1549.0)	penalty = 24; 
					else if (this->value >= 1549.0)							penalty = 100; 
				}
			}
			else if (this->norma == 1535.0)
			{
				if (this->setSpd.GetPassSpeed() > 60)
				{
					if (this->value >= 1542.0 && this->value < 1543.0)		penalty = 1; 
					else if (this->value >= 1543.0 && this->value < 1544.0)	penalty = 2; 
					else if (this->value >= 1544.0 && this->value < 1545.0)	penalty = 3; 
					else if (this->value >= 1545.0 && this->value < 1546.0)	penalty = 18; 
					else if (this->value >= 1546.0 && this->value < 1547.0)	penalty = 24; 
					else if (this->value >= 1547.0)							penalty = 100; 
				}
				else if (this->setSpd.GetPassSpeed() > 25)
				{
					if (this->value >= 1544.0 && this->value < 1545.0)		penalty = 1; 
					else if (this->value >= 1545.0 && this->value < 1546.0)	penalty = 2; 
					else if (this->value >= 1546.0 && this->value < 1547.0)	penalty = 3; 
					else if (this->value >= 1547.0 && this->value < 1548.0)	penalty = 18; 
					else if (this->value >= 1548.0 && this->value < 1549.0)	penalty = 24; 
					else if (this->value >= 1549.0)							penalty = 100; 
				}
				else // if (this->setSpd.spd1 <= 25)
				{
					if (this->value >= 1545.0 && this->value < 1546.0)		penalty = 1; 
					else if (this->value >= 1546.0 && this->value < 1547.0)	penalty = 2; 
					else if (this->value >= 1547.0 && this->value < 1548.0)	penalty = 18; 
					else if (this->value >= 1548.0 && this->value < 1549.0)	penalty = 24; 
					else if (this->value >= 1549.0)							penalty = 100; 
				}
			}
			else if (this->norma == 1540.0)
			{
				if (this->value >= 1547.0 && this->value < 1548.0)			penalty = 1; 
				else if (this->value >= 1548.0 && this->value < 1549.0)		penalty = 5; 
				else if (this->value >= 1549.0)								penalty = 100; 
			}

		}

		else // Сужение
		{
			if (this->norma == 1520.0)
			{
				if (this->setSpd.GetPassSpeed() > 60)
				{
					if (this->value >= 1515.0 && this->value < 1516.0)		penalty = 1; 
					else if (this->value >= 1514.0 && this->value < 1515.0)	penalty = 2; 
					else if (this->value >= 1513.0 && this->value < 1514.0) penalty = 18; 
					else if (this->value >= 1512.0 && this->value < 1513.0) penalty = 24; 
					else if (this->value < 1512.0)							penalty = 100; 
				}
				else // if (this->setSpd.GetPassSpeed() < 60)
				{
					if (this->value >= 1513.0 && this->value < 1514.0)		penalty = 1; 
					else if (this->value >= 1512.0 && this->value < 1513.0)	penalty = 2; 
					else if (this->value >= 1511.0 && this->value < 1512.0) penalty = 18; 
					else if (this->value >= 1510.0 && this->value < 1511.0) penalty = 24; 
					else if (this->value < 1510.0)							penalty = 100;
				}
			}
			else if (this->norma == 1524.0)
			{
				if (this->setSpd.GetPassSpeed() > 15)
				{
					if (this->value >= 1519.0 && this->value < 1520.0)		penalty = 1; 
					else if (this->value >= 1518.0 && this->value < 1519.0)	penalty = 2; 
					else if (this->value >= 1517.0 && this->value < 1518.0) penalty = 18; 
					else if (this->value >= 1516.0 && this->value < 1517.0) penalty = 24; 
					else if (this->value < 1516.0)							penalty = 100; 
				}
				else
				{
					if (this->value >= 1517.0 && this->value < 1518.0)		penalty = 1; 
					else if (this->value >= 1516.0 && this->value < 1517.0)	penalty = 2; 
					else if (this->value >= 1515.0 && this->value < 1516.0) penalty = 18; 
					else if (this->value >= 1514.0 && this->value < 1515.0) penalty = 24; 
					else if (this->value < 1514.0)							penalty = 100; 
				}
			}
			else if (this->norma == 1530.0)
			{
				if (this->setSpd.GetPassSpeed() > 120)
				{
					if (this->value >= 1524.0 && this->value < 1526.0)		penalty = 1; 
					else if (this->value >= 1521.0 && this->value < 1524.0)	penalty = 2; 
					else if (this->value >= 1520.0 && this->value < 1521.0) penalty = 3; 
					else if (this->value >= 1519.0 && this->value < 1520.0) penalty = 18; 
					else if (this->value >= 1518.0 && this->value < 1519.0) penalty = 24; 
					else if (this->value < 1518.0)							penalty = 100; 
				}
				else if (this->setSpd.GetPassSpeed() > 15)
				{
					if (this->value >= 1524.0 && this->value < 1526.0)		penalty = 1; 
					else if (this->value >= 1521.0 && this->value < 1524.0)	penalty = 2; 
					else if (this->value >= 1520.0 && this->value < 1521.0) penalty = 3; 
					else if (this->value >= 1518.0 && this->value < 1520.0) penalty = 18; 
					else if (this->value >= 1517.0 && this->value < 1518.0) penalty = 24; 
					else if (this->value >= 1516.0 && this->value < 1517.0) penalty = 32; 
					else if (this->value < 1516.0)							penalty = 100; 
				}
				else
				{
					if (this->value >= 1524.0 && this->value < 1526.0)		penalty = 1; 
					else if (this->value >= 1521.0 && this->value < 1524.0)	penalty = 2; 
					else if (this->value >= 1520.0 && this->value < 1521.0) penalty = 3; 
					else if (this->value >= 1518.0 && this->value < 1520.0) penalty = 18; 
					else if (this->value >= 1516.0 && this->value < 1518.0) penalty = 24; 
					else if (this->value >= 1514.0 && this->value < 1516.0) penalty = 32; 
					else if (this->value < 1514.0)							penalty = 100; 
				}
			}
			else if (this->norma == 1535.0)
			{
				if (this->setSpd.GetPassSpeed() > 15)
				{
					if (this->value >= 1527.0 && this->value < 1532.0)		penalty = 1; 
					else if (this->value >= 1525.0 && this->value < 1527.0)	penalty = 2; 
					else if (this->value >= 1524.0 && this->value < 1525.0)	penalty = 3; 
					else if (this->value >= 1522.0 && this->value < 1524.0)	penalty = 5; 
					else if (this->value >= 1520.0 && this->value < 1522.0)	penalty = 18; 
					else if (this->value >= 1519.0 && this->value < 1520.0)	penalty = 24; 
					else if (this->value >= 1518.0 && this->value < 1519.0)	penalty = 32; 
					else if (this->value < 1518.0)							penalty = 100; 
				}
				else
				{
					if (this->value >= 1527.0 && this->value < 1532.0)		penalty = 1; 
					else if (this->value >= 1525.0 && this->value < 1527.0)	penalty = 2; 
					else if (this->value >= 1524.0 && this->value < 1525.0)	penalty = 3; 
					else if (this->value >= 1522.0 && this->value < 1524.0)	penalty = 5; 
					else if (this->value >= 1520.0 && this->value < 1522.0)	penalty = 18; 
					else if (this->value >= 1518.0 && this->value < 1520.0)	penalty = 24; 
					else if (this->value >= 1516.0 && this->value < 1518.0)	penalty = 32; 
					else if (this->value < 1516.0)							penalty = 100; 
				}
			}
			else if (this->norma == 1540.0)
			{
				if (this->setSpd.GetPassSpeed() > 15)
				{
					if (this->value >= 1529.0 && this->value < 1532.0)		penalty = 1; 
					else if (this->value >= 1528.0 && this->value < 1529.0)	penalty = 2; 
					else if (this->value >= 1527.0 && this->value < 1528.0)	penalty = 3; 
					else if (this->value >= 1526.0 && this->value < 1527.0)	penalty = 5; 
					else if (this->value >= 1524.0 && this->value < 1526.0)	penalty = 18; 
					else if (this->value >= 1521.0 && this->value < 1524.0)	penalty = 24; 
					else if (this->value >= 1520.0 && this->value < 1521.0)	penalty = 32; 
					else if (this->value < 1520.0)							penalty = 100; 
				}
				else
				{
					if (this->value >= 1529.0 && this->value < 1532.0)		penalty = 1; 
					else if (this->value >= 1528.0 && this->value < 1529.0)	penalty = 2; 
					else if (this->value >= 1527.0 && this->value < 1528.0)	penalty = 3; 
					else if (this->value >= 1526.0 && this->value < 1527.0)	penalty = 5; 
					else if (this->value >= 1521.0 && this->value < 1526.0)	penalty = 18; 
					else if (this->value >= 1519.0 && this->value < 1521.0)	penalty = 24; 
					else if (this->value >= 1518.0 && this->value < 1519.0)	penalty = 32; 
					else if (this->value < 1518.0)							penalty = 100; 
				}
			}
		}

		// Модификаторы для уширения и сужения
		if (penalty < 10)	
		{
			if (this->numElem > 10)	penalty += 11;
			else					penalty += (this->numElem - 1);
		}
		else if (penalty < 50)
		{
			if (this->numElem > 10)	penalty += 20;
			else					penalty += 2 * (this->numElem - 1);
		}
		else
		{
			if (this->numElem > 10)	penalty += 50;
			else					penalty += 5 * (this->numElem - 1);
		}

		break;

	// уровень
	case ovrLevel:

		if (this->setSpd.GetPassSpeed() > 120)
		{
			if (this->value >= 11.0 && this->value < 13.0)		penalty = 1;
			else if (this->value >= 13.0 && this->value < 14.0)	penalty = 2;
			else if (this->value >= 14.0 && this->value < 15.0)	penalty = 3;
			else if (this->value >= 15.0 && this->value < 16.0)	penalty = 4;
			else if (this->value >= 16.0 && this->value < 17.0)	penalty = 6;
			else if (this->value >= 17.0 && this->value < 18.0)	penalty = 18;
			else if (this->value >= 18.0 && this->value < 19.0)	penalty = 20;
			else if (this->value >= 19.0 && this->value < 20.0)	penalty = 24;
			else if (this->value >= 20.0 && this->value < 21.0)	penalty = 50;
			else if (this->value >= 21.0)						penalty = 100;
		}
		else if (this->setSpd.GetPassSpeed() > 60)
		{
			if (this->value >= 12.0 && this->value < 14.0)		penalty = 1;
			else if (this->value >= 14.0 && this->value < 16.0)	penalty = 2;
			else if (this->value >= 16.0 && this->value < 18.0)	penalty = 3;
			else if (this->value >= 18.0 && this->value < 20.0)	penalty = 4;
			else if (this->value >= 20.0 && this->value < 21.0)	penalty = 6;
			else if (this->value >= 21.0 && this->value < 22.0)	penalty = 18;
			else if (this->value >= 22.0 && this->value < 23.0)	penalty = 20;
			else if (this->value >= 23.0 && this->value < 24.0)	penalty = 24;
			else if (this->value >= 24.0 && this->value < 25.0)	penalty = 32;
			else if (this->value >= 25.0 && this->value < 26.0)	penalty = 50;
			else if (this->value >= 26.0)						penalty = 100;
		}
		else if (this->setSpd.GetPassSpeed() > 40)
		{
			if (this->value >= 16.0 && this->value < 19.0)		penalty = 1;
			else if (this->value >= 19.0 && this->value < 21.0)	penalty = 2;
			else if (this->value >= 21.0 && this->value < 23.0)	penalty = 3;
			else if (this->value >= 23.0 && this->value < 25.0)	penalty = 4;
			else if (this->value >= 25.0 && this->value < 26.0)	penalty = 6;
			else if (this->value >= 26.0 && this->value < 27.0)	penalty = 18;
			else if (this->value >= 27.0 && this->value < 28.0)	penalty = 20;
			else if (this->value >= 28.0 && this->value < 29.0)	penalty = 24;
			else if (this->value >= 29.0 && this->value < 30.0)	penalty = 32;
			else if (this->value >= 30.0 && this->value < 31.0)	penalty = 50;
			else if (this->value >= 31.0)						penalty = 100;
		}
		else if (this->setSpd.GetPassSpeed() > 15)
		{
			if (this->value >= 16.0 && this->value < 19.0)		penalty = 1;
			else if (this->value >= 19.0 && this->value < 21.0)	penalty = 2;
			else if (this->value >= 21.0 && this->value < 23.0)	penalty = 3;
			else if (this->value >= 23.0 && this->value < 25.0)	penalty = 4;
			else if (this->value >= 25.0 && this->value < 26.0)	penalty = 6;
			else if (this->value >= 26.0 && this->value < 28.0)	penalty = 18;
			else if (this->value >= 28.0 && this->value < 30.0)	penalty = 20;
			else if (this->value >= 30.0 && this->value < 32.0)	penalty = 24;
			else if (this->value >= 32.0 && this->value < 34.0)	penalty = 32;
			else if (this->value >= 34.0 && this->value < 36.0)	penalty = 50;
			else if (this->value >= 36.0)						penalty = 100;
		}
		else
		{
			if (this->value >= 16.0 && this->value < 19.0)		penalty = 1;
			else if (this->value >= 19.0 && this->value < 21.0)	penalty = 2;
			else if (this->value >= 21.0 && this->value < 23.0)	penalty = 3;
			else if (this->value >= 23.0 && this->value < 25.0)	penalty = 4;
			else if (this->value >= 25.0 && this->value < 26.0)	penalty = 6;
			else if (this->value >= 26.0 && this->value < 31.0)	penalty = 18;
			else if (this->value >= 31.0 && this->value < 36.0)	penalty = 20;
			else if (this->value >= 36.0 && this->value < 41.0)	penalty = 24;
			else if (this->value >= 41.0 && this->value < 46.0)	penalty = 32;
			else if (this->value >= 46.0 && this->value < 51.0)	penalty = 50;
			else if (this->value >= 51.0)						penalty = 100;
		}

		// поправка на длину
		if (penalty < 10)
		{
			if (len > 50)		penalty += 5;
			else if (len > 40)	penalty += 4;
			else if (len > 30)	penalty += 3;
			else if (len > 20)	penalty += 2;
			else if (len > 10)	penalty += 1;
		}
		else if (penalty < 40)
		{
			if (len > 50)		penalty += 10;
			else if (len > 40)	penalty += 8;
			else if (len > 30)	penalty += 6;
			else if (len > 20)	penalty += 4;
			else if (len > 10)	penalty += 2;
		}
		else if (penalty < 80)
		{
			if (len > 50)		penalty += 20;
			else if (len > 40)	penalty += 16;
			else if (len > 30)	penalty += 12;
			else if (len > 20)	penalty += 8;
			else if (len > 10)	penalty += 4;
		}
		else
		{
			if (len > 50)		penalty += 50;
			else if (len > 40)	penalty += 40;
			else if (len > 30)	penalty += 30;
			else if (len > 20)	penalty += 20;
			else if (len > 10)	penalty += 10;
		}

		break;

	// перекос
	case ovrDistort:

		if (this->setSpd.GetPassSpeed() > 120)
		{
			if (this->value >= 10.0 && this->value < 11.0)		penalty = 1;
			else if (this->value >= 11.0 && this->value < 12.0)	penalty = 2;
			else if (this->value >= 12.0 && this->value < 13.0)	penalty = 3;
			else if (this->value >= 13.0 && this->value < 14.0)	penalty = 7;
			else if (this->value >= 14.0 && this->value < 15.0)	penalty = 20;
			else if (this->value >= 15.0 && this->value < 16.0)	penalty = 32;
			else if (this->value >= 16.0 && this->value < 17.0)	penalty = 70;
			else if (this->value >= 17.0)						penalty = 150;
		}
		else if (this->setSpd.GetPassSpeed() > 60)
		{
			if (this->value >= 12.0 && this->value < 13.0)		penalty = 1;
			else if (this->value >= 13.0 && this->value < 14.0)	penalty = 2;
			else if (this->value >= 14.0 && this->value < 15.0)	penalty = 3;
			else if (this->value >= 15.0 && this->value < 16.0)	penalty = 5;
			else if (this->value >= 16.0 && this->value < 17.0)	penalty = 7;
			else if (this->value >= 17.0 && this->value < 18.0)	penalty = 20;
			else if (this->value >= 18.0 && this->value < 19.0)	penalty = 24;
			else if (this->value >= 19.0 && this->value < 20.0)	penalty = 48;
			else if (this->value >= 20.0 && this->value < 21.0)	penalty = 70;
			else if (this->value >= 21.0)						penalty = 150;
		}
		else if (this->setSpd.GetPassSpeed() > 40)
		{
			if (this->value >= 16.0 && this->value < 17.0)		penalty = 1;
			else if (this->value >= 17.0 && this->value < 18.0)	penalty = 2;
			else if (this->value >= 18.0 && this->value < 19.0)	penalty = 3;
			else if (this->value >= 19.0 && this->value < 20.0)	penalty = 5;
			else if (this->value >= 20.0 && this->value < 21.0)	penalty = 7;
			else if (this->value >= 21.0 && this->value < 22.0)	penalty = 20;
			else if (this->value >= 22.0 && this->value < 23.0)	penalty = 24;
			else if (this->value >= 23.0 && this->value < 24.0)	penalty = 32;
			else if (this->value >= 24.0 && this->value < 25.0)	penalty = 48;
			else if (this->value >= 25.0 && this->value < 26.0)	penalty = 70;
			else if (this->value >= 26.0)						penalty = 150;
		}
		else if (this->setSpd.GetPassSpeed() > 15)
		{
			if (this->value >= 16.0 && this->value < 17.0)		penalty = 1;
			else if (this->value >= 17.0 && this->value < 18.0)	penalty = 2;
			else if (this->value >= 18.0 && this->value < 19.0)	penalty = 3;
			else if (this->value >= 19.0 && this->value < 20.0)	penalty = 5;
			else if (this->value >= 20.0 && this->value < 21.0)	penalty = 7;
			else if (this->value >= 21.0 && this->value < 23.0)	penalty = 20;
			else if (this->value >= 23.0 && this->value < 25.0)	penalty = 24;
			else if (this->value >= 25.0 && this->value < 27.0)	penalty = 32;
			else if (this->value >= 27.0 && this->value < 29.0)	penalty = 48;
			else if (this->value >= 29.0 && this->value < 31.0)	penalty = 70;
			else if (this->value >= 31)							penalty = 150;
		}
		else
		{
			if (this->value >= 16.0 && this->value < 17.0)		penalty = 1;
			else if (this->value >= 17.0 && this->value < 18.0)	penalty = 2;
			else if (this->value >= 18.0 && this->value < 19.0)	penalty = 3;
			else if (this->value >= 19.0 && this->value < 20.0)	penalty = 5;
			else if (this->value >= 20.0 && this->value < 21.0)	penalty = 7;
			else if (this->value >= 21.0 && this->value < 25.0)	penalty = 20;
			else if (this->value >= 25.0 && this->value < 32.0)	penalty = 24;
			else if (this->value >= 32.0 && this->value < 39.0)	penalty = 32;
			else if (this->value >= 39.0 && this->value < 46.0)	penalty = 48;
			else if (this->value >= 46.0 && this->value < 51.0)	penalty = 70;
			else if (this->value >= 51)							penalty = 150;
		}

		// поправка на длину
		if (penalty == 5 && this->len > 10.0)		penalty = 4;
		else if (penalty == 7 && this->len > 10.0)	penalty = 6;
		else if (penalty == 20 && this->len > 10.0)	penalty = 19;
		else if (penalty == 24 && this->len > 10.0)	penalty = 22;
		else if (penalty == 32)
		{
			if (this->len > 18.0)		penalty = 24;
			else if (this->len > 14.0)	penalty = 26;
			else if (this->len > 10.0)	penalty = 28;
			else if (this->len > 5.0)	penalty = 30;
		}
		else if (penalty  == 48)
		{
			if (this->len > 18.0)		penalty = 32;
			else if (this->len > 14.0)	penalty = 36;
			else if (this->len > 10.0)	penalty = 40;
			else if (this->len > 5.0)	penalty = 44;
		}
		else if (penalty  == 70)
		{
			if (this->len > 18.0)		penalty = 50;
			else if (this->len > 14.0)	penalty = 55;
			else if (this->len > 10.0)	penalty = 60;
			else if (this->len > 5.0)	penalty = 65;
		}
		else if (penalty  == 150)
		{
			if (this->len > 18.0)		penalty = 100;
			else if (this->len > 16.0)	penalty = 110;
			else if (this->len > 14.0)	penalty = 120;
			else if (this->len > 10.0)	penalty = 130;
			else if (this->len > 5.0)	penalty = 140;
		}

		// ПржП
		if (this->code.PrjP && 
			((this->setSpd.GetPassSpeed() > 120 && this->setSpd.GetTruckSpeed() > 80 && 
			this->value >= 13.0 && this->len <= 10.0) ||
			(this->setSpd.GetPassSpeed() > 60 && this->setSpd.GetTruckSpeed() > 60 && 
			this->value >= 16.0 && this->len <= 10.0)))
		{
			penalty += 100;
		}

		break;

	// просадки
	case ovrPitR:
	case ovrPitL:

		if (this->setSpd.GetPassSpeed() > 120)
		{
			if (this->value >= 12.0 && this->value < 13.0)		penalty = 1;
			else if (this->value >= 13.0 && this->value < 14.0)	penalty = 2;
			else if (this->value >= 14.0 && this->value < 15.0)	penalty = 3;
			else if (this->value >= 15.0 && this->value < 16.0)	penalty = 7;
			else if (this->value >= 16.0 && this->value < 17.0)	penalty = 20;
			else if (this->value >= 17.0 && this->value < 18.0)	penalty = 32;
			else if (this->value >= 18.0 && this->value < 19.0)	penalty = 70;
			else if (this->value >= 19.0)						penalty = 150;
		}
		else if (this->setSpd.GetPassSpeed() > 60)
		{
			if (this->value >= 15.0 && this->value < 17.0)		penalty = 1;
			else if (this->value >= 17.0 && this->value < 18.0)	penalty = 2;
			else if (this->value >= 18.0 && this->value < 19.0)	penalty = 3;
			else if (this->value >= 19.0 && this->value < 20.0)	penalty = 5;
			else if (this->value >= 20.0 && this->value < 21.0)	penalty = 7;
			else if (this->value >= 21.0 && this->value < 22.0)	penalty = 20;
			else if (this->value >= 22.0 && this->value < 23.0)	penalty = 24;
			else if (this->value >= 23.0 && this->value < 24.0)	penalty = 32;
			else if (this->value >= 24.0 && this->value < 25.0)	penalty = 48;
			else if (this->value >= 25.0 && this->value < 26.0)	penalty = 70;
			else if (this->value >= 26.0)						penalty = 150;
		}
		else if (this->setSpd.GetPassSpeed() > 40)
		{
			if (this->value >= 21.0 && this->value < 22.0)		penalty = 1;
			else if (this->value >= 22.0 && this->value < 23.0)	penalty = 2;
			else if (this->value >= 23.0 && this->value < 24.0)	penalty = 3;
			else if (this->value >= 24.0 && this->value < 25.0)	penalty = 5;
			else if (this->value >= 25.0 && this->value < 26.0)	penalty = 7;
			else if (this->value >= 26.0 && this->value < 27.0)	penalty = 20;
			else if (this->value >= 27.0 && this->value < 28.0)	penalty = 24;
			else if (this->value >= 28.0 && this->value < 29.0)	penalty = 32;
			else if (this->value >= 29.0 && this->value < 30.0)	penalty = 48;
			else if (this->value >= 30.0 && this->value < 31.0)	penalty = 70;
			else if (this->value >= 31.0)						penalty = 150;
		}
		else if (this->setSpd.GetPassSpeed() > 15)
		{
			if (this->value >= 21.0 && this->value < 22.0)		penalty = 1;
			else if (this->value >= 22.0 && this->value < 23.0)	penalty = 2;
			else if (this->value >= 23.0 && this->value < 24.0)	penalty = 3;
			else if (this->value >= 24.0 && this->value < 25.0)	penalty = 5;
			else if (this->value >= 25.0 && this->value < 26.0)	penalty = 7;
			else if (this->value >= 26.0 && this->value < 28.0)	penalty = 20;
			else if (this->value >= 28.0 && this->value < 30.0)	penalty = 24;
			else if (this->value >= 30.0 && this->value < 32.0)	penalty = 32;
			else if (this->value >= 32.0 && this->value < 34.0)	penalty = 48;
			else if (this->value >= 34.0 && this->value < 36.0)	penalty = 70;
			else if (this->value >= 36.0)						penalty = 150;
		}
		else
		{
			if (this->value >= 21.0 && this->value < 22.0)		penalty = 1;
			else if (this->value >= 22.0 && this->value < 23.0)	penalty = 2;
			else if (this->value >= 23.0 && this->value < 24.0)	penalty = 3;
			else if (this->value >= 24.0 && this->value < 25.0)	penalty = 5;
			else if (this->value >= 25.0 && this->value < 26.0)	penalty = 7;
			else if (this->value >= 26.0 && this->value < 29.0)	penalty = 20;
			else if (this->value >= 29.0 && this->value < 34.0)	penalty = 24;
			else if (this->value >= 34.0 && this->value < 39.0)	penalty = 32;
			else if (this->value >= 39.0 && this->value < 43.0)	penalty = 48;
			else if (this->value >= 43.0 && this->value < 46.0)	penalty = 70;
			else if (this->value >= 46.0)						penalty = 150;
		}

		// поправка на длину
		if (penalty == 5 && this->len > 6.0)		penalty = 4;
		else if (penalty == 7 && this->len > 6.0)	penalty = 6;
		else if (penalty == 20)
		{
			if (this->len > 8.0)		penalty = 18;
			else if (this->len > 6.0)	penalty = 19;
		}
		else if (penalty == 24)
		{
			if (this->len > 8.0)		penalty = 20;
			else if (this->len > 6.0)	penalty = 22;
			else if (this->len > 4.0)	penalty = 23;
		}
		else if (penalty == 32)
		{
			if (this->len > 8.0)		penalty = 24;
			else if (this->len > 6.0)	penalty = 26;
			else if (this->len > 4.0)	penalty = 24;
			else if (this->len > 2.0)	penalty = 30;
		}
		else if (penalty == 48)
		{
			if (this->len > 8.0)		penalty = 32;
			else if (this->len > 6.0)	penalty = 36;
			else if (this->len > 4.0)	penalty = 40;
			else if (this->len > 2.0)	penalty = 44;
		}
		else if (penalty == 70)
		{
			if (this->len > 8.0)		penalty = 50;
			else if (this->len > 6.0)	penalty = 55;
			else if (this->len > 4.0)	penalty = 60;
			else if (this->len > 2.0)	penalty = 65;
		}

		// повторяющиеса просадки
		if (this->code.ThreePr)
		{
			if (this->value >= 16.0 && this->value < 18.0)
			{
				penalty += 3;
			}
			else if (this->value >= 18.0)
			{
				penalty += 33;
			}
		}

		break;

	// рихтовки
	case ovrRichR:
	case ovrRichL:

		if (this->setSpd.GetPassSpeed() > 120)
		{
			if (this->value >= 15.0 && this->value < 17.0)		penalty = 1;
			else if (this->value >= 17.0 && this->value < 18.0)	penalty = 2;
			else if (this->value >= 18.0 && this->value < 19.0)	penalty = 3;
			else if (this->value >= 19.0 && this->value < 20.0)	penalty = 5;
			else if (this->value >= 20.0 && this->value < 21.0)	penalty = 7;
			else if (this->value >= 21.0 && this->value < 22.0)	penalty = 20;
			else if (this->value >= 22.0 && this->value < 23.0)	penalty = 24;
			else if (this->value >= 23.0 && this->value < 24.0)	penalty = 32;
			else if (this->value >= 24.0 && this->value < 25.0)	penalty = 48;
			else if (this->value >= 25.0 && this->value < 26.0)	penalty = 70;
			else if (this->value >= 26.0)						penalty = 150;
		}
		else if (this->setSpd.GetPassSpeed() > 60)
		{
			if (this->value >= 21.0 && this->value < 23.0)		penalty = 1;
			else if (this->value >= 23.0 && this->value < 25.0)	penalty = 2;
			else if (this->value >= 25.0 && this->value < 27.0)	penalty = 3;
			else if (this->value >= 27.0 && this->value < 29.0)	penalty = 5;
			else if (this->value >= 29.0 && this->value < 30.0)	penalty = 7;
			else if (this->value >= 30.0 && this->value < 32.0)	penalty = 20;
			else if (this->value >= 32.0 && this->value < 33.0)	penalty = 24;
			else if (this->value >= 33.0 && this->value < 34.0)	penalty = 32;
			else if (this->value >= 34.0 && this->value < 35.0)	penalty = 48;
			else if (this->value >= 35.0 && this->value < 36.0)	penalty = 70;
			else if (this->value >= 36.0)						penalty = 150;
		}
		else if (this->setSpd.GetPassSpeed() > 40)
		{
			if (this->value >= 31.0 && this->value < 33.0)		penalty = 1;
			else if (this->value >= 33.0 && this->value < 35.0)	penalty = 2;
			else if (this->value >= 35.0 && this->value < 37.0)	penalty = 3;
			else if (this->value >= 37.0 && this->value < 39.0)	penalty = 5;
			else if (this->value >= 39.0 && this->value < 40.0)	penalty = 7;
			else if (this->value >= 40.0 && this->value < 42.0)	penalty = 20;
			else if (this->value >= 42.0 && this->value < 43.0)	penalty = 24;
			else if (this->value >= 43.0 && this->value < 44.0)	penalty = 32;
			else if (this->value >= 44.0 && this->value < 45.0)	penalty = 48;
			else if (this->value >= 45.0 && this->value < 46.0)	penalty = 70;
			else if (this->value >= 46.0)						penalty = 150;
		}
		else if (this->setSpd.GetPassSpeed() > 15)
		{
			if (this->value >= 31.0 && this->value < 33.0)		penalty = 1;
			else if (this->value >= 33.0 && this->value < 35.0)	penalty = 2;
			else if (this->value >= 35.0 && this->value < 37.0)	penalty = 3;
			else if (this->value >= 37.0 && this->value < 39.0)	penalty = 5;
			else if (this->value >= 39.0 && this->value < 40.0)	penalty = 7;
			else if (this->value >= 40.0 && this->value < 46.0)	penalty = 20;
			else if (this->value >= 46.0 && this->value < 49.0)	penalty = 24;
			else if (this->value >= 49.0 && this->value < 52.0)	penalty = 32;
			else if (this->value >= 52.0 && this->value < 54.0)	penalty = 48;
			else if (this->value >= 54.0 && this->value < 56.0)	penalty = 70;
			else if (this->value >= 56.0)						penalty = 150;
		}
		else
		{
			if (this->value >= 31.0 && this->value < 33.0)		penalty = 1;
			else if (this->value >= 33.0 && this->value < 35.0)	penalty = 2;
			else if (this->value >= 35.0 && this->value < 37.0)	penalty = 3;
			else if (this->value >= 37.0 && this->value < 39.0)	penalty = 5;
			else if (this->value >= 39.0 && this->value < 40.0)	penalty = 7;
			else if (this->value >= 40.0 && this->value < 60.0)	penalty = 20;
			else if (this->value >= 60.0 && this->value < 70.0)	penalty = 24;
			else if (this->value >= 70.0 && this->value < 80.0)	penalty = 32;
			else if (this->value >= 80.0 && this->value < 90.0)	penalty = 48;
			else if (this->value >= 90.0 && this->value < 100.0) penalty = 70;
			else if (this->value >= 100.0)						penalty = 150;
		}

		// поправка на длину
		if (penalty == 7 && this->len < 10.0)		penalty = 6;
		else if (penalty == 20)
		{
			if (this->len > 18.0)		penalty = 18;
			else if (this->len > 10.0)	penalty = 19;
		}
		else if (penalty == 24)
		{
			if (this->len > 18.0)		penalty = 20;
			else if (this->len > 10.0)	penalty = 22;
		}
		else if (penalty == 32)
		{
			if (this->len > 18.0)		penalty = 24;
			else if (this->len > 14.0)	penalty = 26;
			else if (this->len > 10.0)	penalty = 28;
			else if (this->len > 5.0)	penalty = 30;
		}
		else if (penalty == 48)
		{
			if (this->len > 18.0)		penalty = 32;
			else if (this->len > 14.0)	penalty = 36;
			else if (this->len > 10.0)	penalty = 40;
			else if (this->len > 5.0)	penalty = 44;
		}
		else if (penalty == 70)
		{
			if (this->len > 18.0)		penalty = 50;
			else if (this->len > 14.0)	penalty = 55;
			else if (this->len > 10.0)	penalty = 60;
			else if (this->len > 5.0)	penalty = 65;
		}
		else if (penalty == 150)
		{
			if (this->len > 18.0)		penalty = 100;
			else if (this->len > 16.0)	penalty = 110;
			else if (this->len > 14.0)	penalty = 120;
			else if (this->len > 10.0)	penalty = 130;
			else if (this->len > 5.0)	penalty = 140;
		}

		// три рихтовки подряд
		if (this->code.PP)
		{
			if (this->value >= 25.0 && this->value < 27.0)
			{
				penalty += 3;
			}
			else if (this->value >= 27.0)
			{
				penalty += 33;
			}
		}

		break;

	}

	return penalty;
}

// Потроха - загрузка из блока данных
bool VOvr2List::Load(VBasicParameter & data)
{
	if (this->IsLoaded()) return true;

	// версия 1
	if (data.GetVersion() <= 1 && data.GetRecSize() == sizeof(VOverstepOnDiskV0))
	{
		this->full_v0.reserve(data.GetRecCount());
		for (register size_t i = 0; i < data.GetRecCount(); ++i)
		{
			const VOverstepOnDiskV0 & o = data.GetItem<VOverstepOnDiskV0>(i);
			if (VOvr2List::CheckType(o.type))
			{
				this->full_v0.push_back(o);
			}
		}
	}

	// версия 2
	else if (data.GetVersion() == 2 && data.GetRecSize() == sizeof(VOverstepOnDiskV1))
	{
		this->full_v1.reserve(data.GetRecCount());
		for (register size_t i = 0; i < data.GetRecCount(); ++i)
		{
			const VOverstepOnDiskV1 & o = data.GetItem<VOverstepOnDiskV1>(i);
			if (VOvr2List::CheckType(o.type))
			{
				this->full_v1.push_back(o);
			}
		}
	}

	// х.з.
	else
	{
		this->Unload();
		data.Unload();
		this->message = ERRMSG_VERSION_INCORRECT;
		return false;
	}

	return this->BuildOvrList(ovmAll, NULL);
}

// сохранение в файл
bool VOvr2List::Save(VMainFile & file, uint32_t type)
{
	if (!this->IsLoaded())
	{
		this->message = ERRMSG_DATA_NOT_LOADED;
		return false;
	}

	// при записи пустого блока нужно просто удалить старый
	if (this->GetCount() == 0) 
	{
		this->DeleteFromFile(file, type);
		this->message = ERRMSG_OK;
		return true;
	}

	if (!this->full_v0.empty())
	{
		VBasicParameter data;
		data.Create(type, VCT_USER, this->full_v0.size(), sizeof(VOverstepOnDiskV0), 1);
		size_t position = 0;
		for (size_t i = 0; i < this->full_v0.size(); ++i)
		{
			data.Write<VOverstepOnDiskV0>(full_v0[i], position);
		}
		if (data.Save(file, data.GetRecCount(), data.GetRecSize()))
		{
			this->message = ERRMSG_OK;
			return true;
		}
		else
		{
			this->message = ERRMSG_BLOCK_WRITE_FAILED;
			return false;
		}
	}
	else if (!this->full_v1.empty())
	{
		VBasicParameter data;
		data.Create(type, VCT_USER, this->full_v1.size(), sizeof(VOverstepOnDiskV1), 2);
		size_t position = 0;
		for (size_t i = 0; i < this->full_v1.size(); ++i)
		{
			data.Write<VOverstepOnDiskV1>(full_v1[i], position);
		}
		if (data.Save(file, data.GetRecCount(), data.GetRecSize()))
		{
			this->message = ERRMSG_OK;
			return true;
		}
		else
		{
			this->message = ERRMSG_BLOCK_WRITE_FAILED;
			return false;
		}
	}
	else 
	{
		this->message = ERRMSG_VERSION_INCORRECT;
		return false;
	}
}

// установка маски и корректировка списка отступлений
bool VOvr2List::BuildOvrList(unsigned long __mask, VWaySection * section)
{
	VIndexList<VOverstep2>::Unload();

	if (!this->full_v0.empty())
	{
		for (size_t i = 0; i < this->full_v0.size(); ++i)
		{
			VOverstep2 ovr(full_v0[i]);
			if (ovr.MeetsMask(__mask) && ovr.MeetsSection(section))
			{
				this->AddItem(ovr);
			}
		}
	}
	else if (!this->full_v1.empty())
	{
		for (size_t i = 0; i < this->full_v1.size(); ++i)
		{
			VOverstep2 ovr(full_v1[i]);
			if (ovr.MeetsMask(__mask) && ovr.MeetsSection(section))
			{
				this->AddItem(ovr);
			}
		}
	}

	this->Sort();
	this->message = ERRMSG_OK;
	this->SetLoaded(true);
	return true;
}

// Копирование списка 
bool VOvr2List::CopyFrom(const VOvr2List & src, const VWaySection * section)
{
	this->Unload();
	VOverstep2 ovr;

	if (!src.full_v0.empty())
	{
		for (size_t i = 0; i < src.full_v0.size(); ++i)
		{
			VOverstep2 ovr(src.full_v0[i]);
			if (ovr.MeetsSection(section))
			{
				this->full_v0.push_back(src.full_v0[i]);
			}
		}
	}

	else if (!src.full_v1.empty())
	{
		for (size_t i = 0; i < src.full_v1.size(); ++i)
		{
			VOverstep2 ovr(src.full_v1[i]);
			if (ovr.MeetsSection(section))
			{
				this->full_v1.push_back(src.full_v1[i]);
			}
		}
	}

	this->BuildOvrList(ovmAll, NULL);
	this->SetLoaded(true);
	this->message = ERRMSG_OK;
	return true;
}

// Импорт из вектора
bool VOvr2List::Import(const std::vector<VOverstepOnDiskV1> & ovrs)
{
	this->full_v0.clear();
	this->full_v1 = ovrs;
	return this->BuildOvrList(ovmAll, NULL);
}

// Экспорт списка отступлений для указанного км
std::vector<VOverstep2> VOvr2List::ExportList(int32_t km) const
{
	std::vector<VOverstep2> ovrlist;
	for (size_t i = 0; i < this->records.size(); ++i)
	{
		if (this->records[i].wayCoord.km == km)
			ovrlist.push_back(records[i]);
	}
	return ovrlist;
}

static int cntOvrs(const std::vector<VOverstep2> & ovrs, int degree, int ovrtype)
{
	int count = 0;
	for (size_t i = 0; i < ovrs.size(); ++i)
	{
		if (ovrs[i].degree == degree && ovrs[i].type == ovrtype) 
		{
			++count;
		}
	}
	return count;
}

static int cntLims(const std::vector<VOverstep2> & ovrs, int ovrtype)
{
	int count = 0;
	for (size_t i = 0; i < ovrs.size(); ++i)
	{
		if (ovrs[i].type == ovrtype && ovrs[i].IsLimit())
		{
			++count;
		}
	}
	return count;
}

static std::string printCnt(int count)
{
	if (count == 0)	return "&#8199;";
	else 
	{
		std::stringstream str;
		str << count;
		return str.str();
	}
}

// ПУ-32 для указанного километра
std::string VOvr2List::GetPU32String(int32_t km) const
{
	std::stringstream report;
	std::vector<VOverstep2> ovrlist = this->ExportList(km);
	if (ovrlist.empty()) return "";

	// минимальная скорость на километре
	short minPrkSpd = ovrlist[0].setSpd.GetPassSpeed();
	for (size_t i = 1; i < ovrlist.size(); ++i)
		minPrkSpd = std::min<short>(minPrkSpd, ovrlist[i].setSpd.GetPassSpeed());

	int count2 = 0, count3 = 0, sumCount2 = 0, sumCount3 = 0;

	report << "<tr><th>" << km << "</th>" << std::endl;

	// Отступления

	// Сужения
	count2 = cntOvrs(ovrlist, 2, ovrWidthMn); sumCount2 += count2; 
	count3 = cntOvrs(ovrlist, 3, ovrWidthMn); sumCount3 += count3;
	report << "<td>" << printCnt(count2) << "/" << printCnt(count3) << "</td>" << std::endl;

	// Уширения
	count2 = cntOvrs(ovrlist, 2, ovrWidthPl); sumCount2 += count2; 
	count3 = cntOvrs(ovrlist, 3, ovrWidthPl); sumCount3 += count3;
	report << "<td>" << printCnt(count2) << "/" << printCnt(count3) << "</td>" << std::endl;

	// Уровень
	count2 = cntOvrs(ovrlist, 2, ovrLevel); sumCount2 += count2; 
	count3 = cntOvrs(ovrlist, 3, ovrLevel); sumCount3 += count3;
	report << "<td>" << printCnt(count2) << "/" << printCnt(count3) << "</td>" << std::endl;

	// Перекосы
	count2 = cntOvrs(ovrlist, 2, ovrDistort); sumCount2 += count2; 
	count3 = cntOvrs(ovrlist, 3, ovrDistort); sumCount3 += count3;
	report << "<td>" << printCnt(count2) << "/" << printCnt(count3) << "</td>" << std::endl;

	// Просадки
	count2 = cntOvrs(ovrlist, 2, ovrPitL) + cntOvrs(ovrlist, 2, ovrPitR); sumCount2 += count2; 
	count3 = cntOvrs(ovrlist, 3, ovrPitL) + cntOvrs(ovrlist, 2, ovrPitR); sumCount3 += count3;
	report << "<td>" << printCnt(count2) << "/" << printCnt(count3) << "</td>" << std::endl;

	// Рихтовки
	count2 = cntOvrs(ovrlist, 2, ovrRichL) + cntOvrs(ovrlist, 2, ovrRichR) + 
		cntOvrs(ovrlist, 2, ovrRich) + cntOvrs(ovrlist, 2, ovrRichN); sumCount2 += count2; 
	count3 = cntOvrs(ovrlist, 3, ovrRichL) + cntOvrs(ovrlist, 2, ovrRichR) + 
		cntOvrs(ovrlist, 3, ovrRich) + cntOvrs(ovrlist, 2, ovrRichN); sumCount3 += count3;
	report << "<td>" << printCnt(count2) << "/" << printCnt(count3) << "</td>" << std::endl;

	// Итого
	report << "<td>" << printCnt(sumCount2) << "/" << printCnt(sumCount3) << "</td>" << std::endl;

	// Неисправности (Ограничения скорости)
	report << "<td>";

	// Уширения
	int countLim = cntLims(ovrlist, ovrWidthMn);
	int sumCountLim = countLim;
	if (countLim > 0)	report << countLim << "&nbsp;Уш ";

	// Сужения
	countLim = cntLims(ovrlist, ovrWidthPl);
	sumCountLim += countLim;
	if (countLim > 0)	report << countLim << "&nbsp;Суж ";

	// Уровень
	countLim = cntLims(ovrlist, ovrLevel);
	sumCountLim += countLim;
	if (countLim > 0)	report << countLim << "&nbsp;У ";

	// Перекос
	countLim = cntLims(ovrlist, ovrDistort);
	sumCountLim += countLim;
	if (countLim > 0)	report << countLim << "&nbsp;П ";

	// Просадки
	countLim = cntLims(ovrlist, ovrPitL) + cntLims(ovrlist, ovrPitR);
	sumCountLim += countLim;
	if (countLim > 0)	report << countLim << "&nbsp;Пр ";

	// Рихтовки (Либо правая и левая, либо рихтовочная и нерихтовочная)
	countLim = cntLims(ovrlist, ovrRichL) + cntLims(ovrlist, ovrRichR) 
		+ cntLims(ovrlist, ovrRich) + cntLims(ovrlist, ovrRichN);
	sumCountLim += countLim;
	if (countLim > 0)	report << countLim << "&nbsp;Р ";

	// Отвод ШК
	countLim = cntLims(ovrlist, ovrOtvod);
	sumCountLim += countLim;
	if (countLim > 0)	report << countLim << "&nbsp;ОШК ";

	// Отвод возвышения
	countLim = cntLims(ovrlist, ovrOtvodH);
	sumCountLim += countLim;
	if (countLim > 0)	report << countLim << "&nbsp;ОВ ";

	// Непогашенное
	countLim = cntLims(ovrlist, ovrAnp);
	sumCountLim += countLim;
	if (countLim > 0)	report << countLim << "&nbsp;Анп ";

	// Зазоры (либо правый и левый, либо без маркировки стороны)
	countLim = cntLims(ovrlist, ovrZazor) + cntLims(ovrlist, ovrZazorR) + cntLims(ovrlist, ovrZazorL);
	sumCountLim += countLim;
	if (countLim > 0)	report << countLim << "&nbsp;Ззр ";

	// Износы
	countLim = cntLims(ovrlist, ovrIznos);
	sumCountLim += countLim;
	if (countLim > 0)	report << countLim << "&nbsp;Изн ";
	report << "</td>" << std::endl;

	// Бальная оценка
	report << "<td>";
	sumCount3 = cntLims(ovrlist, ovrLevel) + cntLims(ovrlist, ovrDistort) +
		cntLims(ovrlist, ovrPitR) + cntLims(ovrlist, ovrPitL) + 
		cntLims(ovrlist, ovrRichR) + cntLims(ovrlist, ovrRichL);

	int old_penalty;
	if (minPrkSpd > 60)
	{
		if (sumCountLim > 0)											old_penalty = 500;
		else if (sumCountLim == 0 && sumCount3 > 6)						old_penalty = 500;
		else if (sumCountLim == 0 && sumCount3 > 0)						old_penalty = 150;
		else if (sumCountLim == 0 && sumCount3 == 0 && sumCount2 > 25)	old_penalty = 150;
		else if (sumCountLim == 0 && sumCount3 == 0 && sumCount2 > 5)	old_penalty = 40;
		else															old_penalty = 10;
	}
	else
	{
		if (sumCountLim > 0)											old_penalty = 500;
		else if (sumCountLim == 0 && sumCount3 > 3)						old_penalty = 500;
		else if (sumCountLim == 0 && sumCount3 > 0)						old_penalty = 150;
		else if (sumCountLim == 0 && sumCount3 == 0 && sumCount2 > 12)	old_penalty = 150;
		else if (sumCountLim == 0 && sumCount3 == 0 && sumCount2 > 3)	old_penalty = 40;
		else															old_penalty = 10;
	}
	report << (old_penalty == 500 ? "<b>" : "") << printCnt(old_penalty) << 
		(old_penalty == 500 ? "</b>" : "");

	int new_penalty = 0;
	for (size_t i = 0; i < ovrlist.size(); ++i)
	{
		new_penalty += ovrlist[i].GetPenalty();
	}
	report << "/" << (new_penalty >= 100 ? "<b>" : "") << printCnt(new_penalty) << 
		(new_penalty >= 100 ? "</b>" : "") << "</td>";
	
	report << "<td>";
	if (old_penalty == 500)	report << "<b>Н</b>";
	else if (old_penalty == 150) report << "У";
	else if (old_penalty == 40) report << "Х";
	else if (old_penalty == 10) report << "О";
	else report << "";
	
	report << "/";
	if (new_penalty >= 100)	report << "<b>Н</b>";
	else if ((new_penalty >= 15 && minPrkSpd > 60) || (new_penalty >= 9 && minPrkSpd <= 60)) report << "У";
	else if (new_penalty >= 1) report << "X";
	else report << "О";
	report << "</td>" << std::endl;

	// Комментарий - список пикетов с ограничениями
	report << "<td>";
	std::set<int> pks;
	for (size_t i = 0; i < ovrlist.size(); ++i)
	{
		if (ovrlist[i].IsLimit())
		{
			pks.insert(1 + static_cast<int>(ovrlist[i].wayCoord.m) / 100);
		}
	}
	if (!pks.empty())
	{
		report << "ПК ";
		for (std::set<int>::const_iterator it = pks.begin(); it != pks.end(); ++it)
		{
			report << *it << " ";
		}
	}
	report << "</td></tr>" << std::endl;

	return report.str();
}

/// Список оцененных километров, в порядке оценки
std::set<int32_t> VOvr2List::GetKmList() const
{
	std::set<int32_t> result;
	const std::vector<VOverstep2> & overs = this->ExportList();
	for (size_t i = 0; i < overs.size(); ++i)
	{
		result.insert(overs[i].wayCoord.km);
	}
	return result;
}

template <class FILETYPE>
bool VOvr2List::Load(FILETYPE & file, uint32_t type)
{
	VBasicParameter data;
	if (!data.Load(file, type, false))
	{
		this->message = data.GetErrorMessage();
		return false;
	}

	bool result = this->Load(data);
	data.UnloadFile(file, type);
	data.Unload();
	return result;
}

template bool VOvr2List::Load<VMainFile>(VMainFile & file, uint32_t type);
template bool VOvr2List::Load<VRemoteFile>(VRemoteFile & file, uint32_t type);