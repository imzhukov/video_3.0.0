#include "VAprinfoRecord.h"
#include "VParameter.h"

// ���������� �����, �� ��������� ����������
VWayCoord VAprinfoKmRecord::GetRealCoord(double absCoord) const
{
	VWayCoord coord(this->km, 0);
	if (absCoord < this->base)	
	{
		coord.m = this->len < 0 ? fabs(this->len) : 0.0f;
	}
	else if (absCoord > this->end)
	{
		coord.m = this->len < 0 ? 0.0f : this->len;
	}
	else
	{
		coord.m = this->len < 0 ? float(this->end - absCoord) : float(absCoord - this->base);
	}
	return coord;
}

// ���. ���������� ���������� �����
double VAprinfoKmRecord::GetAbsCoord(float realM) const
{
	if (realM < 0)
	{
		return this->len < 0 ? this->end : this->base;
	}
	else if (realM > fabs(this->end - this->base))
	{
		return this->len < 0 ? this->base : this->end;
	}
	else
	{
		return this->len < 0 ? this->end - realM : this->base + realM;
	}
}

// ������ ������� �� ���������
std::vector<VPicket> VAprinfoKmRecord::GetPkList() const
{
	std::vector<VPicket> pkList;
	float kmLen = this->len;
	long kmDir = this->len >= 0 ? 1 : -1;
	long pkSign = this->km >= 0 ? 1 : -1;
	double abs = kmDir > 0 ? this->base : this->end;

	while (fabs(kmLen) > 0)
	{
		VPicket pk;
		pk.size = sizeof(pk);
		pk.km = km;
		pk.pk = ((long) pkList.size() + 1) * pkSign;
		pk.section.dwSize = sizeof(pk.section);
		pk.absStart = abs;
		pk.section.start = this->GetRealCoord(pk.absStart);
		if (fabs(kmLen) > 150)
		{
			pk.len = 100.0f * kmDir;
			kmLen -= pk.len;
		}
		else
		{
			pk.len = kmLen;
			kmLen = 0;
		}
		abs += pk.len;
		pk.absEnd = abs;
		pk.section.end = this->GetRealCoord(abs);
		if (kmDir < 0)	
		{
			std::swap(pk.section.start, pk.section.end);
			std::swap(pk.absStart, pk.absEnd);
		}
		pkList.push_back(pk);
	}

	if (kmDir < 0)
	{
		size_t pkCnt = pkList.size();
		for (size_t i = 0; i < pkList.size() / 2; ++i)
		{	
			std::swap(pkList[i], pkList[pkCnt - i - 1]);
		}
	}

	return pkList;
}

bool operator < (const VAprinfoBasicRecord & r1, const VAprinfoBasicRecord & r2)
{
	return r1.base < r2.base;
}

void VAprinfoCurve::Clear()
{
	this->base = 0.0;
	this->end = 0.0;
	this->isThere = false;
	this->angle = 0.0;
	this->latStart = 0.0;
	this->latEnd = 0.0;
	this->longStart = 0.0;
	this->longEnd = 0.0;
	this->id = -1;
	this->levelLabel.clear();
	this->planLabel.clear();
	this->radius.clear();
}

std::wstring VAprinfoCurve::Validate() const
{
	std::wstring result;

	if (this->planLabel.size() != this->levelLabel.size())
	{
		result = L"��������� ����� ����� �� ����� � ������";
	}
	else if ((this->planLabel.size() % 2) != 0)
	{
		result = L"�������� ����� ����� �� ������";
	}
	else 
	{
		bool error_plan = false, error_level = false;
		for (size_t i = 1; i < this->planLabel.size(); ++i)
		{
			if (this->planLabel.at(i).coord <= this->planLabel.at(i-1).coord)
			{
				error_plan = true;
				break;
			}

			if (this->levelLabel.at(i).coord <= this->levelLabel.at(i-1).coord)
			{
				error_level = true;
				break;
			}
		}
		if (error_plan || error_level)
		{
			result = L"�������� ������� ����� �� ������";
		}
	}

	return result;
}

// �������� ������ ��� "��������" �������
double VAprinfoZLRecord::Shift(double coord, bool beginning) const
{
	if (beginning)
	{
		return (this->value_end - this->value_base) * 
			(coord - this->base) / (this->end - this->base);
	}
	else
	{
		return (this->value_end - this->value_base) *
			(this->end - coord) / (this->end - this->base);
	}
}

std::string VAprinfoRailRecord::GetDescr() const
{
	switch(this->value)
	{
	case RAIL_R75:	return "�75";
	case RAIL_R65:	return "�65";
	case RAIL_R65K:	return "�65�";
	case RAIL_R50:	return "�50";
	case RAIL_R43:	return "�43";
	default:		return "�� �������";
	}
}

std::string VAprinfoBraceRecord::GetDescr() const
{
	switch(this->value)
	{
	case BRACE_K2:			return "�2";
	case BRACE_BP:			return "��";
	case BRACE_ZHB:			return "��";
	case BRACE_ZHBR:		return "���";
	case BRACE_ZHBR65:		return "���-65";
	case BRACE_ZHBR65SH:	return "���-65�";
	case BRACE_ARS:			return "���";
	case BRACE_PANDROL_350:	return "�������-350";
	case BRACE_D2:			return "�2";
	case BRACE_D4:			return "�4";
	case BRACE_KD:			return "��";
	default:				return "�� �������";
	}
}

std::string GetMachineTypeDescription(MACHINE_TYPE mach)
{
	switch(mach)
	{
	case MACHINE_VPO:					return "���";
	case MACHINE_VRO_OR_VPR:			return "��� ��� ���";
	case MACHINE_VPO_PLUS_VPR:			return "��� + ���";
	case MACHINE_VPO_PLUS_VPR_PLUS_DSP:	return "��� + ��� + ���";
	default:							return "";
	}
}

std::string VAprinfoTestrunRecord::GetDescr() const
{
	return GetMachineTypeDescription(this->machine);
}

