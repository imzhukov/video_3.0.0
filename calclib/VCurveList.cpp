#include "VCurveList.h"
#include "VAprinfo.h"
#include "error_messages.h"

#include <sstream>

// ---------------------------------------------------------------------------
// ��������� � �����
// ---------------------------------------------------------------------------
#pragma pack (push, 1)

/// ��������� ������ � �����
struct VCurveRecHeader			
{
	uint32_t size;			///< ������ ���������
	uint32_t version;		///< ������ ���������
	uint32_t paramCount;	///< ���������� �������� � �������
	uint32_t markSize;		///< ������ ��������� ����������� �����
	uint32_t elCount;		///< ���������� �������� � ������������� �������
	uint32_t elSize;		///< ������ ��������� ����������� ������������ ������
	unsigned short spd1, spd2, spd3, spd4;	///< �������� (������������ � ������)
};

/// ��������� ������� ����� � �����
struct VCurveMarkArrHdr	
{
	uint32_t size;		///< ������ ���������
	uint32_t version;	///< ������ ���������
	uint32_t paramType;	///< ��� ������� �� ������� ������������� �����
	uint32_t markCount;	///< ���������� �����
	char reserve[16];	///< ������
};

/// ����� � �����
struct VFileCurveMark
{
	uint32_t size;		///< ������ ���������
	uint32_t type;		///< ��� �����
	VWayCoord coord;	///< ������� ����������	
	float value;		///< ��������
	double absCoord;	///< ���. ����������
};
#pragma pack (pop)

bool VCurveList::Load(const VBasicParameter & data, const VAbsCoordData & absCoord)
{

	size_t position = 0;
	if (data.GetDataSize() > 0)
	{
		this->message.clear();
		for (size_t crvNdx = 0; crvNdx < data.GetRecCount(); ++crvNdx)
		{
			VCurveRecHeader * recHeader = (VCurveRecHeader *) data.GetPtr(position);
			position += sizeof(VCurveRecHeader);

			VCurve crv;

			// ���������� � ������
			if (data.GetDataType() == DHT_MODEL_CURVES)
			{
				// ��������� ������
				crv.SetModel(true);

				size_t version = data.GetVersion();

				// ������ ������
				if (version >= 3)
				{
					data.Read<long>(crv.model.status, position);
				}
				else
					crv.model.status = 0;

				// ��������
				if (version == 2 || version == 3 || version == 4)
				{
					unsigned short spd;
					data.Read<unsigned short>(spd, position);
					crv.model.Vpass = spd;
					data.Read<unsigned short>(spd, position);
					crv.model.Vcrg = spd;
					data.Read<unsigned short>(spd, position);
					crv.model.Vavg = spd;
					data.Read<unsigned short>(spd, position);
					crv.model.Vmin = spd;
				}
				else if (version >= 5)
				{
					long spd;
					data.Read<long>(spd, position);
					crv.model.Vhigh_bt = spd;
					data.Read<long>(spd, position);
					crv.model.Vhigh = spd;
					data.Read<long>(spd, position);
					crv.model.Vpass = spd;
					data.Read<long>(spd, position);
					crv.model.Vcrg = spd;
					data.Read<long>(spd, position);
					crv.model.Vavg = spd;
					data.Read<long>(spd, position);
					crv.model.Vmin = spd;
				}

				// ����� ���������
				else
				{
					crv.model.Vpass = 100;
					crv.model.Vcrg = 80;
					crv.model.Vavg = 60;
					crv.model.Vmin = 40;
				}
			}

			// ������� � ������� ������������� ����
			for (uint32_t paramNdx = 0; paramNdx < recHeader->paramCount; ++paramNdx)
			{
				VCurveMarkArrHdr * markHeader = (VCurveMarkArrHdr *) data.GetPtr(position);
				position += sizeof(VCurveMarkArrHdr);

				for (uint32_t markNdx = 0; markNdx < markHeader->markCount; ++markNdx)
				{
					VCurveMark crvMark;
					VFileCurveMark * fileMark = (VFileCurveMark *) data.GetPtr(position);
					position += sizeof(VFileCurveMark);

					if (fileMark->type == 0) // ��������
					{
						crv.AddPlanMark(
							absCoord.FindIndex(fileMark->absCoord, true), 
							fileMark->value);
					}
					else if (fileMark->type == 1) // �������
					{
						crv.AddLevelMark(
							absCoord.FindIndex(fileMark->absCoord, true), 
							fileMark->value);
					}
				}
			}

			if (data.GetDataType() == DHT_MODEL_CURVES && data.GetVersion() >= 4)
				data.ReadBlock(crv.model.comment, COMMENT_SIZE, position);

			if (crv.CheckMarks())
				this->AddItem(crv);
			else
			{
				std::wstringstream msg;
				msg << L"������ " << crvNdx+1 << L" ���������: ������������ ��������" << std::endl;				
				this->message += msg.str();
			}
		}
	}
	else
	{
		this->message = ERRMSG_DATA_INCOMPLETE;
	}

	this->SetLoaded(true);
	if (this->message.empty())
	{
		this->message = ERRMSG_OK;
	}
	return true;
}

// ������ � ����
bool VCurveList::Save(VMainFile & file, uint32_t type, const VAbsCoordData & absCoord)
{
	if (type != DHT_CURVE_MARKS && type != DHT_MODEL_CURVES) return false;
	if (!this->IsLoaded())
	{
		this->message = ERRMSG_DATA_NOT_LOADED;
		return false;
	}

	//
	if (GetActiveCurve()!=BAD_INDEX)
	{
		if (GetItem(GetActiveCurve()).CheckMarks())
			DeactivateCurve();
		else
			DeleteItem(GetActiveCurve());
	}

	// ��� ������ ������� ����� ����� ������ ������� ������
	if (this->GetCount() == 0) 
	{
		this->DeleteFromFile(file, type);
		this->message = ERRMSG_OK;
		return true;
	}

	// ���������� ������ ����� ������
	size_t blkSize = 0;
	for (size_t crvNdx = 0; crvNdx < this->GetCount(); ++crvNdx)
	{
		// ��������� ������
		blkSize += sizeof(VCurveRecHeader);

		// ��������� ������ - ���������� � ������
		if (type == DHT_MODEL_CURVES)
		{
			blkSize += 7 * sizeof(long);
		}

		// �����
		blkSize += 2 * (sizeof(VCurveMarkArrHdr) + sizeof(VFileCurveMark) * this->GetItem(crvNdx).GetMarkCount());

		// ��������� ������ - �����������
		if (type == DHT_MODEL_CURVES)
		{
			blkSize += COMMENT_SIZE;
		}
	}

	VBasicParameter data;
	uint32_t version = type == DHT_CURVE_MARKS ? 1 : 5;
	data.Create(type, VCT_USER, 1, blkSize, version);
	size_t position = 0;

	for (size_t crvNdx = 0; crvNdx < this->GetCount(); ++crvNdx)
	{
		// ��������� ������
		VCurveRecHeader recHdr = {0};
		recHdr.size = sizeof(recHdr);
		recHdr.version = version;
		recHdr.paramCount = 2;
		recHdr.markSize = sizeof(VFileCurveMark);
		data.Write<VCurveRecHeader>(recHdr, position);

		// ������
		if (type == DHT_MODEL_CURVES)
		{
			data.Write<long>(this->GetItem(crvNdx).model.status, position);
			data.Write<long>(this->GetItem(crvNdx).model.Vhigh_bt, position);
			data.Write<long>(this->GetItem(crvNdx).model.Vhigh, position);
			data.Write<long>(this->GetItem(crvNdx).model.Vpass, position);
			data.Write<long>(this->GetItem(crvNdx).model.Vcrg, position);
			data.Write<long>(this->GetItem(crvNdx).model.Vavg, position);
			data.Write<long>(this->GetItem(crvNdx).model.Vmin, position);
		}

		// ������ ����� � �����
		VCurveMarkArrHdr planHdr = {0};
		planHdr.size = sizeof(planHdr);
		planHdr.paramType = DHT_MID_CURV;
		planHdr.version = version;
		planHdr.markCount = this->GetItem(crvNdx).GetMarkCount();
		data.Write<VCurveMarkArrHdr>(planHdr, position);
		
		// ����� � �����
		for (size_t markNdx = 0; markNdx < planHdr.markCount; ++markNdx)
		{
			VFileCurveMark mark = {0};
			mark.size = sizeof(mark);
			mark.type = 0;
			mark.value = this->GetItem(crvNdx).GetPlanMark(markNdx).value;
			mark.absCoord = absCoord.GetItem(this->GetItem(crvNdx).GetPlanMark(markNdx).index);
			data.Write<VFileCurveMark>(mark, position);
		}

		// ������ ����� � ������
		VCurveMarkArrHdr levelHdr = {0};
		levelHdr.size = sizeof(levelHdr);
		levelHdr.paramType = DHT_MID_LEVEL;
		levelHdr.version = version;
		levelHdr.markCount = this->GetItem(crvNdx).GetLevelMarkCount();
		data.Write<VCurveMarkArrHdr>(levelHdr, position);
		
		// ����� � ������
		for (size_t markNdx = 0; markNdx < levelHdr.markCount; ++markNdx)
		{
			VFileCurveMark mark = {0};
			mark.size = sizeof(mark);
			mark.type = 1;
			mark.value = this->GetItem(crvNdx).GetLevelMark(markNdx).value;
			mark.absCoord = absCoord.GetItem(this->GetItem(crvNdx).GetLevelMark(markNdx).index);
			data.Write<VFileCurveMark>(mark, position);
		}

		// ������
		if (type == DHT_MODEL_CURVES)
		{
			data.WriteBlock(this->GetItem(crvNdx).model.comment, COMMENT_SIZE, position);
		}
	}

	if (data.Save(file, this->GetCount(), 0))
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

// ���������� ������ �� ������� ������, � ������������� �������� � ����� ���.�-�
bool VCurveList::CopyFrom(const VCurveList & src, const VAbsCoordData & srcAbs, const VAbsCoordData & destAbs)
{
	this->Unload();
	for (size_t crvNdx = 0; crvNdx < src.GetCount(); ++crvNdx)
	{
		const VCurve * crv = &src.GetItem(crvNdx);
		VCurve newCurve;

		// updating indices
		bool update_result = true;
		for (size_t markNdx = 0; markNdx < crv->GetMarkCount(); ++markNdx)
		{
			uint32_t index = crv->GetPlanMark(markNdx).index;
			if (!destAbs.UpdateIndex(index, srcAbs))
			{
				update_result = false;
				break;
			}
			newCurve.AddPlanMark(index, crv->GetPlanMark(markNdx).value);

			index = crv->GetLevelMark(markNdx).index;
			if (!destAbs.UpdateIndex(index, srcAbs))
			{
				update_result = false;
				break;
			}
			newCurve.AddLevelMark(index, crv->GetLevelMark(markNdx).value);
		}

		if (update_result)
		{
			// ������?
			newCurve.SetModel(crv->IsModel());
			newCurve.model = crv->model;			

			// �������� � ������
			this->AddItem(newCurve);			
		}
	}
	this->SetLoaded(true);
	this->message = ERRMSG_OK;
	return true;
}

// ���������� �������������� ���������
bool VCurveList::SetGeoCoord(const VDoubleParameter & latitude, const VDoubleParameter & longitude)
{
	if (!latitude.IsLoaded() || !longitude.IsLoaded())
	{
		return false;
	}

	for (size_t crvNdx = 0; crvNdx < this->GetCount(); ++crvNdx)
	{
		for (size_t markNdx = 0; markNdx < this->GetItem(crvNdx).GetMarkCount(); ++markNdx)
		{			
			this->GetItem(crvNdx).GetPlanMark(markNdx).SetGeoCoord(latitude, longitude);						
		}
		for (size_t markNdx = 0; markNdx < this->GetItem(crvNdx).GetLevelMarkCount(); ++markNdx)
		{
			this->GetItem(crvNdx).GetLevelMark(markNdx).SetGeoCoord(latitude, longitude);
		}
	}

	return true;
}

/// ������������� �������� ������
void VCurveList::ActivateCurve(size_t in_active_curve_index)
{	
	if (active_curve_index==BAD_INDEX || active_curve_index < GetCount() && GetItem(active_curve_index).PlanMarksValid() && GetItem(active_curve_index).LevelMarksValid())
		active_curve_index=in_active_curve_index;
}

/// ������� �������� ������
void VCurveList::DeleteActiveCurve()
{
	if (active_curve_index < GetCount())
	{
		DeleteItem(active_curve_index);
		active_curve_index=BAD_INDEX;
	}
}

/// ���������� �������� ������
void VCurveList::DeactivateCurve()
{
	if (GetCount()==0 || active_curve_index < GetCount() && GetItem(active_curve_index).PlanMarksValid() && GetItem(active_curve_index).LevelMarksValid())
		active_curve_index=BAD_INDEX;
}

/// ��������� ����� �������� ������
void VCurveList::BackupActiveCurve()
{
	if (active_curve_index!=BAD_INDEX)
	{
		active_curve_bak=GetItem(active_curve_index);
	}
}

/// �������������� �������� ������ �� ��������� �����
void VCurveList::RestoreActiveCurve()
{
	if (active_curve_index!=BAD_INDEX)
	{
		GetItem(active_curve_index)=active_curve_bak;
	}
}

/// ���������� ������ �������� ������
size_t VCurveList::GetActiveCurve() const
{
	return active_curve_index;
}

/// ������� ������ �� ������� ��������� ����� � �������� ��������
size_t VCurveList::FindCrv(size_t in_index) const
{
	size_t start_point=0;	
	if (in_index>=find_crv_cache_record_index)
		start_point=find_crv_cache_curve_index;

	for (size_t i=0; i<GetCount(); ++i)
	{
		if (GetItem(i).HasPointCrv(in_index)==0)
		{
			find_crv_cache_record_index=in_index;
			find_crv_cache_curve_index=i;
			return i;
		}
		else if (i>0 && GetItem(i-1).HasPointCrv(in_index)*GetItem(i).HasPointCrv(in_index)<0 ||
			     i<GetCount()-1 && GetItem(i).HasPointCrv(in_index)*GetItem(i+1).HasPointCrv(in_index)<0)
			return BAD_INDEX;
	}
	return BAD_INDEX;
}

/// ������� ������ �� ������� ��������� ����� � �������� ��������
size_t VCurveList::FindLvl(size_t in_index) const
{
	size_t start_point=0;	
	if (in_index>=find_lvl_cache_record_index)
		start_point=find_lvl_cache_curve_index;
		
	for (size_t i=start_point; i<GetCount(); ++i)
	{
		if (GetItem(i).HasPointLvl(in_index)==0)
		{
			find_lvl_cache_record_index=in_index;
			find_lvl_cache_curve_index=i;
			return i;
		}
		else if (i>0 && GetItem(i-1).HasPointLvl(in_index)*GetItem(i).HasPointLvl(in_index)<0 ||
			     i<GetCount()-1 && GetItem(i).HasPointLvl(in_index)*GetItem(i+1).HasPointLvl(in_index)<0)
			return BAD_INDEX;
	}
	return BAD_INDEX;
}

/// ���������� ��������� ��������
float VCurveList::AproxPlan(size_t in_index, const VAbsCoordData & absCoord) const
{
	size_t curve_index=FindCrv(in_index);
	if (curve_index!=BAD_INDEX)
		return GetItem(curve_index).AproxPlan(in_index, absCoord);
	else
		return 0;
}

/// ���������� ��������� ����������
float VCurveList::AproxLevel(size_t in_index, const VAbsCoordData & absCoord) const
{
	size_t curve_index=FindLvl(in_index);
	if (curve_index!=BAD_INDEX)
		return GetItem(curve_index).AproxLevel(in_index, absCoord);
	else
		return 0;
}

// ������� ����� - �������� ��������
bool VZLine::Load(const VAbsCoordData & absCoord, const VCurveList & curves,
				  bool level, const VAprinfo * aprInfo)
{
	if (this->IsLoaded()) return true;
	if (!absCoord.IsLoaded())
	{
		this->message = ERRMSG_MISSING_PARAMETER;
		return false;
	}

	// � ������ - ������� ����� �� ������
	bool use_nl = (level && aprInfo && aprInfo->IsLoaded());
	std::vector<VAprinfoIndexRecord> nl;

	// "����������" ������
	std::vector<const VCurve*> crv;
	for (size_t crvNdx = 0; crvNdx < curves.GetCount(); ++crvNdx)
	{
		if (curves.GetItem(crvNdx).GetMarkCount() > 1)
		{
			crv.push_back(&curves.GetItem(crvNdx));
		}
	}

	// ��������
	std::vector<Element> els;

	// ������ ������ ������
	if (crv.empty())
	{
		// ������ ���� ����������
		if (use_nl)
		{
			nl = aprInfo->GetStraightNormaLevelList(absCoord.GetItem(0), absCoord.GetLastItem());
			for (size_t nlNdx = 0; nlNdx < nl.size(); ++nlNdx)
			{
				Element el;
				el.valueBegin = (double) nl[nlNdx].value;
				el.valueEnd = el.valueBegin;
				el.idxBegin = absCoord.FindIndex(nl[nlNdx].base, true);
				el.idxEnd = absCoord.FindIndex(nl[nlNdx].end, true);
				els.push_back(el);
			}
		}
		// ����
		else
		{
			Element el;
			el.valueBegin = 0;
			el.valueEnd = 0;
			el.idxBegin = 0;
			el.idxEnd = absCoord.GetCount() - 1;
			els.push_back(el);
		}
	}

	// ���� ������ ������
	else
	{
		// �� ������ ����� �� ������ ������
		// ������ ���� ����������
		if (use_nl)
		{
			nl = aprInfo->GetStraightNormaLevelList(absCoord.GetItem(0), 
				absCoord.GetItem(crv.front()->GetLevelMark(0).index));

			for (size_t nlNdx = 0; nlNdx < nl.size(); ++nlNdx)
			{
				Element el;
				el.valueBegin = (double) nl[nlNdx].value;
				el.valueEnd = el.valueBegin;
				el.idxBegin = absCoord.FindIndex(nl[nlNdx].base, true);
				el.idxEnd = absCoord.FindIndex(nl[nlNdx].end, true);
				els.push_back(el);
			}
		}
		// ����
		else
		{
			Element el;
			el.valueBegin = 0;
			el.valueEnd = 0;
			el.idxBegin = 0;
			if (level)
			{
				el.idxEnd = crv.front()->GetLevelMark(0).index;
			}
			else
			{
				el.idxEnd = crv.front()->GetPlanMark(0).index;
			}

			els.push_back(el);
		}

		// �� ������
		for (size_t crvNdx = 0; crvNdx < crv.size(); ++crvNdx)
		{
			// ����� �� ������� �� ������� ������
			if (crvNdx > 0)
			{
				// ������ ���� ����������
				if (use_nl)
				{
					nl = aprInfo->GetStraightNormaLevelList(
						absCoord.GetItem(crv[crvNdx-1]->GetLevelMark(crv[crvNdx-1]->GetMarkCount()-1).index),
						absCoord.GetItem(crv[crvNdx]->GetLevelMark(0).index));

					for (size_t nlNdx = 0; nlNdx < nl.size(); ++nlNdx)
					{
						Element el;
						el.valueBegin = (double) nl[nlNdx].value;
						el.valueEnd = el.valueBegin;
						el.idxBegin = absCoord.FindIndex(nl[nlNdx].base, true);
						el.idxEnd = absCoord.FindIndex(nl[nlNdx].end, true);
						els.push_back(el);
					}
				}
				// ����
				else
				{
					Element el;
					el.valueBegin = 0;
					el.valueEnd = 0;
					if (level)
					{
						el.idxBegin = crv[crvNdx-1]->GetLevelMark(crv[crvNdx-1]->GetMarkCount()-1).index;
						el.idxEnd = crv[crvNdx]->GetLevelMark(0).index;
					}
					else
					{
						el.idxBegin = crv[crvNdx-1]->GetPlanMark(crv[crvNdx-1]->GetMarkCount()-1).index;
						el.idxEnd = crv[crvNdx]->GetPlanMark(0).index;
					}

					els.push_back(el);
				}
			}

			for (size_t markNdx = 1; markNdx < crv[crvNdx]->GetMarkCount(); ++markNdx)
			{
				Element el;
				
				// ������ �������
				if (level)
				{
					el.idxBegin = crv[crvNdx]->GetLevelMark(markNdx-1).index;
				}
				else
				{
					el.idxBegin = crv[crvNdx]->GetPlanMark(markNdx-1).index;
				}

				// �������� � ������ �������
				if (markNdx == 1)
				{
					if (use_nl)
					{
						el.valueBegin = aprInfo->GetStraightNormaLevel(absCoord.GetItem(el.idxBegin));
					}
					else
					{
						el.valueBegin = 0;
					}
				}
				else if (level)
				{
					el.valueBegin = crv[crvNdx]->GetLevelMark(markNdx-1).value;
				}
				else
				{
					el.valueBegin = crv[crvNdx]->GetPlanMark(markNdx-1).value;
				}

				// ����� �������
				if (level)
				{
					el.idxEnd = crv[crvNdx]->GetLevelMark(markNdx).index;
				}
				else
				{
					el.idxEnd = crv[crvNdx]->GetPlanMark(markNdx).index;
				}

				// �������� � ����� �������
				if (markNdx == crv[crvNdx]->GetMarkCount() - 1)
				{
					if (use_nl)
					{
						el.valueEnd = aprInfo->GetStraightNormaLevel(absCoord.GetItem(el.idxEnd));
					}
					else
					{
						el.valueEnd = 0;
					}
				}
				else if (level)
				{
					el.valueEnd = crv[crvNdx]->GetLevelMark(markNdx).value;
				}
				else
				{
					el.valueEnd = crv[crvNdx]->GetPlanMark(markNdx).value;
				}

				els.push_back(el);
			}

		}

		if (use_nl)
		{
			nl = aprInfo->GetStraightNormaLevelList(
				absCoord.GetItem(crv.back()->GetLevelMark(crv.back()->GetMarkCount()-1).index),
				absCoord.GetLastItem());

			for (size_t nlNdx = 0; nlNdx < nl.size(); ++nlNdx)
			{
				Element el;
				el.valueBegin = (double) nl[nlNdx].value;
				el.valueEnd = el.valueBegin;
				el.idxBegin = absCoord.FindIndex(nl[nlNdx].base, true);
				el.idxEnd = absCoord.FindIndex(nl[nlNdx].end, true);

				els.push_back(el);
			}
		}
		else
		{
			Element el;
			el.valueBegin = 0;
			el.valueEnd = 0;
			if (level)
			{
				el.idxBegin = crv.back()->GetLevelMark(crv.back()->GetMarkCount()-1).index;
			}
			else
			{
				el.idxBegin = crv.back()->GetPlanMark(crv.back()->GetMarkCount()-1).index;
			}
			el.idxEnd = absCoord.GetCount() - 1;

			els.push_back(el);
		}
	}

	this->Create(0xFFFFFFFF, absCoord.GetCount(), 0, 0.0f);
	for (size_t elNdx = 0; elNdx < els.size(); ++elNdx)
	{
		if (els[elNdx].idxEnd <= els[elNdx].idxBegin) continue;
		for (size_t kadrNdx = els[elNdx].idxBegin; kadrNdx < els[elNdx].idxEnd; ++kadrNdx)
		{
			this->GetItem(kadrNdx) = float(els[elNdx].valueBegin + 
				(els[elNdx].valueEnd - els[elNdx].valueBegin) * 
				(absCoord.GetItem(kadrNdx) - absCoord.GetItem(els[elNdx].idxBegin)) / 
				(absCoord.GetItem(els[elNdx].idxEnd) - absCoord.GetItem(els[elNdx].idxBegin))
			);
		}
	}

	//std::ofstream debug("d:\\v3_zl.txt";
	//if (debug.good())
	//{
	//	for (size_t i = 0; i < this->GetCount(); ++i)
	//	{
	//		debug << this->GetItem(i) << std::endl;
	//	}
	//	debug.close();
	//}

	this->message = ERRMSG_OK;
	this->SetLoaded(true);
	return true;
}


/// ��������
bool VDiffLevel::Load(const VCurveList& in_curves, const VParameter& in_level, const VAbsCoordData& in_abscoord,
	const VAprinfo& in_aprinfo)
{
	if (this->IsLoaded()) return true;

	if (!in_curves.IsLoaded() || !in_abscoord.IsLoaded())
	{
		this->message = ERRMSG_MISSING_PARAMETER;
		return false;
	}	
	this->Create(0xFFFFFFFF, in_abscoord.GetCount(), 0, UNDEFINED_VALUE);
	//FixMe!!! ��������� ����� ���������� �� ������
	for (size_t i = 0; i < in_abscoord.GetCount(); ++i)
	{		
		this->GetItem(i) = in_level.GetItem(i) - in_curves.AproxLevel(i, in_abscoord);
	}
	return true;
}

/// ����������
bool VDiffLevel::Update(const VCurveList& in_curves, const VParameter& in_level, const VAbsCoordData& in_abscoord, 
	const VAprinfo& in_aprinfo, size_t in_index)
{
	if (!this->IsLoaded())
	{
		Load(in_curves, in_level, in_abscoord, in_aprinfo);
	}
	else if (in_index == BAD_INDEX)
	{
		for (size_t i = 0; i < in_abscoord.GetCount(); ++i)
		{		
			this->GetItem(i) = in_level.GetItem(i) - in_curves.AproxLevel(i, in_abscoord);
		}
	}
	else if (in_curves.GetItem(in_index).GetLevelMarkCount()>0)
	{
		size_t start = in_curves.GetItem(in_index).GetLevelMark(0).index;
		size_t end = in_curves.GetItem(in_index).GetLevelMark(in_curves.GetItem(in_index).GetLevelMarkCount()-1).index;
		//FixMe!!! ��������� ����� ���������� �� ������
		for (size_t i = start; i <= end; ++i)
		{		
			this->GetItem(i) = in_level.GetItem(i) - in_curves.AproxLevel(i, in_abscoord);
		}
	}
	return true;
}