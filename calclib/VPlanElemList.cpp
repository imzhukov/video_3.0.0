#include "VPlanElemList.h"
#include "VDataSet.h"

#include <algorithm>
#include "strlib.h"

#include "Shiny.h"

///���������� �������� ���� ��������
std::wstring GetPlanElementTypeName(PLAN_ELEM_TYPE in_type)
{
	switch (in_type)
	{
		default:
			return std::wstring(L"����������� ���");
		case PLAN_ELEM_LINE:			
			return std::wstring(L"������");
		case PLAN_ELEM_HALFLINE:
			return std::wstring(L"�������� ������");
		case PLAN_ELEM_CURVE:
			return std::wstring(L"������");
		case PLAN_ELEM_HALFCURVE:
			return std::wstring(L"�������� ������");
		case PLAN_ELEM_SIDEWAY:
			return std::wstring(L"������� ����");
		case PLAN_ELEM_RADIUS:
			return std::wstring(L"�������� ������");
		case PLAN_ELEM_INTERMEDIATE:
			return std::wstring(L"���������� ������");
	};
}

std::wostream & operator << (std::wostream & os, const VWayCoord & coord)
{
	os << coord.format_long();
	return os;
}

void VPlanElement::SetCoords(const VAbsCoordData & absCoord, const VAprinfo & aprInfo)
{
	this->absStart = absCoord.GetItem(this->idxStart);
	this->absEnd = absCoord.GetItem(this->idxEnd);
	this->section.dwSize = sizeof(VWaySection);
	this->section.start = aprInfo.GetRealCoord(this->absStart);
	this->section.end = aprInfo.GetRealCoord(this->absEnd);
}

/// ���������� true ���� ����� ��������� �� ��������
bool VPlanElement::HasPoint(double in_abscoord) const
{
	return in_abscoord>=absStart && in_abscoord<=absEnd;
}

/// ���������� ����� ��������
double VPlanElement::GetLength() const
{
	return fabs(absEnd-absStart);
}

bool operator < (const VPlanElement & pe1, const VPlanElement & pe2)
{
	return pe1.idxStart < pe2.idxStart;
}

bool is_zero_element(const VPlanElement& in_element)
{
	return in_element.idxStart==in_element.idxEnd;
}

// �������� �������� ������
void VPlanElemList::AddLineElemList(size_t idxFrom, size_t idxTo,
									bool detailed_line,
									const VLineMarkList & linemarks, 
									const VAprinfo & aprInfo, 
									const VAbsCoordData & absCoord)
{
	if (idxTo <= idxFrom) return;

	VPlanElement elem;
	elem.type = PLAN_ELEM_LINE;
	elem.curve_index = BAD_INDEX;

	if (detailed_line && linemarks.IsLoaded() && linemarks.GetCount() > 0)
	{
		std::vector<VLineMark> lm = linemarks.ExportMarks(idxFrom, idxTo);

		elem.idxStart = idxFrom;

		for (size_t i = 0; i < lm.size(); ++i)
		{
			elem.idxEnd = lm.at(i).index;
			elem.SetCoords(absCoord, aprInfo);
			this->records.push_back(elem);

			elem.idxStart = lm.at(i).index;
		}

		elem.idxEnd = idxTo;
		elem.SetCoords(absCoord, aprInfo);
		this->records.push_back(elem);
	}

	else
	{
		elem.idxStart = idxFrom;
		elem.idxEnd = idxTo;
		elem.SetCoords(absCoord, aprInfo);
		this->records.push_back(elem);
	}
}

// �������� �� ��������� ���������
bool VPlanElemList::Load(const VWaySection & sumsection,
						 const VCurveList & curves, 
						 const VLineMarkList & linemarks, 
						 const VSidewayList & sideways, 
						 const VAprinfo & aprinfo, 
						 const VAbsCoordData & absCoord,
						 bool detailed_curve, bool detailed_line)
{
	if (this->IsLoaded()) return true;
	if (!absCoord.IsLoaded() || !aprinfo.IsLoaded()) 
	{
		this->message = ERRMSG_MISSING_PARAMETER;
		this->SetLoaded(false);
		return false;
	}

	//������� 1

	//FixMe!!! ������ HALF_LINE

/*
	//������
	VPlanElement element;
	element.idxStart = 0;
	element.type = PLAN_ELEM_LINE;
	this->records.push_back(element);

	//������
	if (curves.IsLoaded())
	{
		for (size_t i = 0; i < curves.GetCount(); ++i)
		{
			const VCurve * crv = &curves.GetItem(i);

			//FixMe!!! ��������� detailed_curve

			if (crv->GetMarkCount()>1)
			{
				//������ ������				
				element.idxStart = crv->GetPlanMark(0).index;						
				element.type = PLAN_ELEM_CURVE;
				element.curve_index = i;

				if (!this->records.empty() && 
					element.idxStart < this->records.back().idxStart)
				{
					element.idxStart = this->records.back().idxStart;
				}

				this->records.push_back(element);			
				//������ ������
				element.idxStart = crv->GetPlanMark(crv->GetMarkCount()-1).index;			
				element.type = PLAN_ELEM_LINE;
				element.curve_index=BAD_INDEX;
				this->records.push_back(element);
			}
		}
	}
	//������
	if (detailed_line && linemarks.IsLoaded())
	{
		for (size_t i=0; i<linemarks.GetCount(); ++i)
		{
			//������ ������
			element.idxStart = linemarks.GetItem(i).index;
			element.type = PLAN_ELEM_LINE;
			element.curve_index=BAD_INDEX;
			this->records.push_back(element);
		}
	}

	//������� ����
	if (sideways.IsLoaded())
	{
		for (size_t i = 0; i < sideways.GetCount(); ++i)
		{			
			//������ �������� ����
			element.idxStart = sideways.GetItem(i).GetStartNdx();						
			switch(sideways.GetItem(i).GetReason())
			{
				case REASON_SIDEWAY: element.type = PLAN_ELEM_SIDEWAY; break;
				case REASON_HALFCURVE: element.type = PLAN_ELEM_HALFCURVE; break;
				default: continue;
			}			
			element.curve_index=BAD_INDEX;
			this->records.push_back(element);
			//������ ������
			element.idxStart = sideways.GetItem(i).GetEndNdx();			
			element.type = PLAN_ELEM_LINE;
			element.curve_index=BAD_INDEX;
			this->records.push_back(element);	
		}
	}
	//��������� �����
	std::stable_sort(this->records.begin(), this->records.end());
	//������������� ���������� ������
	for (size_t i=0; i<records.size(); ++i)
	{
		if (i+1!=records.size())
		{
			records.at(i).idxEnd=records.at(i+1).idxStart;
			records.at(i).SetCoords(absCoord, aprinfo);
		}
		else
		{
			records.at(i).idxEnd=absCoord.GetCount()-1;
			records.at(i).SetCoords(absCoord, aprinfo);
		}
	}
	//������� �������� ������� �����
	records.erase(std::remove_if(records.begin(), records.end(), is_zero_element), records.end());
	*/

	// ������ ������
	if (curves.IsLoaded())
	{
		for (size_t i = 0; i < curves.GetCount(); ++i)
		{
			const VCurve * crv = &curves.GetItem(i);

			if (detailed_curve)
			{
				size_t markCount = crv->GetMarkCount();
				for (size_t markNdx = 1; markNdx < markCount; ++markNdx)
				{
					VPlanElement element;
					element.idxStart = crv->GetPlanMark(markNdx-1).index;
					element.idxEnd = crv->GetPlanMark(markNdx).index;
					element.SetCoords(absCoord, aprinfo);
					if ((markNdx % 2) == 1) element.type = PLAN_ELEM_INTERMEDIATE;
					else element.type = PLAN_ELEM_RADIUS;
					element.curve_index = i;
					this->records.push_back(element);
				}
			}
			else
			{
				VPlanElement element;
				element.idxStart = crv->GetPlanMark(crv->GetRangeMark(COMPLETE_CURVE, false, true)).index;
				element.idxEnd = crv->GetPlanMark(crv->GetRangeMark(COMPLETE_CURVE, false, false)).index;
				element.SetCoords(absCoord, aprinfo);
				element.type = PLAN_ELEM_CURVE;
				element.curve_index = i;
				this->records.push_back(element);
			}
		}
	}

/*
	std::ofstream dpe("d:\\dump_plan_elem.txt";
	if (dpe.is_open())
	{
		for (size_t i = 0; i < this->records.size(); ++i)
		{
			dpe << i+1 << ". ";
			dpe << this->records[i].section.start.format_long() << " (";
			dpe << this->records[i].absStart << "; ";
			dpe << this->records[i].idxStart << " - ";

			dpe << this->records[i].section.end.format_long() << " (";
			dpe << this->records[i].absEnd << "; ";
			dpe << this->records[i].idxEnd << "" << std::endl;
		}
		dpe.close();
	}

	std::ofstream dpe_apr("d:\\dump_plan_elem_aprinfo.html";
	if (dpe_apr.is_open())
	{
		dpe_apr << aprinfo.GetReport(true, false);
		dpe_apr.close();
	}
*/

	// ������ ������� ����� � �������� ������
	if (sideways.IsLoaded())
	{
		for (size_t i = 0; i < sideways.GetCount(); ++i)
		{
			VPlanElement element;
			element.idxStart = sideways.GetItem(i).GetStartNdx();
			element.idxEnd = sideways.GetItem(i).GetEndNdx();
			element.SetCoords(absCoord, aprinfo);
			switch(sideways.GetItem(i).GetReason())
			{
			case REASON_SIDEWAY: element.type = PLAN_ELEM_SIDEWAY; break;
			case REASON_HALFCURVE: element.type = PLAN_ELEM_HALFCURVE; break;
			default: continue;
			}

			element.curve_index = BAD_INDEX;
			this->records.push_back(element);
		}
	}

	std::sort(this->records.begin(), this->records.end());

	// ��������� ����������
	size_t sumcount = this->records.size();

	if (sumcount > 0)
	{
		this->AddLineElemList(0, this->records[0].idxStart, detailed_line,
			linemarks, aprinfo, absCoord);

		for (size_t i = 1; i < sumcount; ++i)
		{
			VPlanElement element;
			element.type = PLAN_ELEM_LINE;
			element.curve_index = BAD_INDEX;
			element.idxStart = this->records[i-1].idxEnd;
			element.idxEnd = this->records[i].idxStart;
			element.SetCoords(absCoord, aprinfo);
			if (element.section.start == element.section.end) continue;


/*			if ((this->records[i-1].type == PLAN_ELEM_CURVE || this->records[i-1].type == PLAN_ELEM_HALFCURVE) &&
				(this->records[i].type == PLAN_ELEM_CURVE || this->records[i].type == PLAN_ELEM_HALFCURVE))
			{
				element.type = PLAN_ELEM_LINE;
			}
			else
			{
				element.type = PLAN_ELEM_HALFLINE;
			}*/

			if (!element.section.isValid())
			{
				std::wstringstream msg;
				msg << L"�������� ����� ������������: ";
				msg << GetPlanElementTypeName(this->records[i-1].type);
				msg << L" (����� " << element.section.start << L" � ";
				msg << GetPlanElementTypeName(this->records[i].type);
				msg << L" (������ " << element.section.end << L"";
				this->message = msg.str();
				this->SetLoaded(false);
				return false;
			}

			this->AddLineElemList(element.idxStart, element.idxEnd, detailed_line,
				linemarks, aprinfo, absCoord);
		}

		if (this->records[sumcount-1].idxEnd > absCoord.GetCount()-1)
		{
			this->message = L"������� ����� ������������ � ������ ����";
			this->SetLoaded(false);
			return false;
		}

		else
		{
			this->AddLineElemList(this->records[sumcount-1].idxEnd, absCoord.GetCount()-1,
				detailed_line, linemarks, aprinfo, absCoord);
		}

		std::sort(this->records.begin(), this->records.end());
	}
	else
	{
		this->AddLineElemList(0, absCoord.GetCount()-1,
			detailed_line, linemarks, aprinfo, absCoord);
	}

	this->message = ERRMSG_OK;
	this->SetLoaded(true);
	return true;
}

// ��������
bool VPlanElemList::Load(VDataSet & ds, bool model, bool detailed_curve, bool detailed_line)
{
	if (this->IsLoaded()) return true;

	this->Unload();
	ds.Load(PRM_ABS_COORD);
	ds.Load(PRM_SIDEWAYS);

	if (model)
	{
		ds.Load(PRM_MODEL_CURVES);
	}
	else
	{
		ds.Load(PRM_CURVE_MARKS);
	}
	ds.Load(PRM_LINE_MARKS);

	VAbsCoordData & absCoord = ds.GetAbsCoord();
	const VAprinfo & aprinfo = ds.GetAprinfo();
	VCurveList & curves = model ? ds.GetCurves(PRM_MODEL_CURVES) : ds.GetCurves(PRM_CURVE_MARKS);
	VLineMarkList &  linemarks = ds.GetLineMarks();
	VSidewayList & sideways = ds.GetSideways();
	VWaySection sumsection = ds.GetSection();

	return this->Load(sumsection, curves, linemarks, sideways, aprinfo, absCoord, detailed_curve, detailed_line);
}

/// ������� ������� �� ������� ��������� �������� �����
size_t VPlanElemList::Find(double in_abscoord) const
{
	class has_point
	{
	private:
		double abscoord;
	public:
		has_point(double in_abscoord){abscoord=in_abscoord;};
		bool operator() (const VPlanElement& in_slope){return in_slope.HasPoint(abscoord);};
	};	
	std::vector<VPlanElement>::const_iterator i=std::find_if(records.begin(), records.end(), has_point(in_abscoord));
	return i!=records.end() ? i-records.begin() : BAD_INDEX;
}

/// ���������� ������ ��������� �� ������� ��������� ����� � ��������� ��������
std::vector<size_t> VPlanElemList::GetAffectedElements(size_t in_index) const
{
	std::vector<size_t> affected_elements;
	for (size_t i=0; i<GetCount(); ++i)
	{
		if (GetItem(i).idxStart<=in_index && GetItem(i).idxEnd>=in_index)
		{
			affected_elements.push_back(i);
		}		
	}
	return affected_elements;
}