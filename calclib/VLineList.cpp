#include "VLineList.h"
#include "VFileMark.h"

// загрузка из отдельных элементов
bool VLineList::Load(const VCurveList & curves, 
					 const VLineMarkList & linemarks, 
					 const VSidewayList & sideWays, 
					 const VAprinfo & aprInfo, 
					 const VAbsCoordData & absCoord)
{
	VWaySection section(aprInfo.GetRealCoord(absCoord.GetItem(0)), 
		aprInfo.GetRealCoord(absCoord.GetLastItem()));

	if (!aprInfo.IsLoaded() || !absCoord.IsLoaded())
	{
		this->message = ERRMSG_MISSING_PARAMETER;
		this->SetLoaded(false);
		return false;
	}

	VPlanElemList peList;
	if (!peList.Load(section, curves, linemarks, sideWays, aprInfo, absCoord, false, true))
	{
		this->message = peList.GetErrorMessage();
		this->SetLoaded(false);
		return false;
	}

	for (size_t i = 0; i < peList.GetCount(); ++i)
	{
		if (peList.GetItem(i).type == PLAN_ELEM_LINE ||
			peList.GetItem(i).type == PLAN_ELEM_HALFLINE)
		{
			this->AddItem(VLine(peList.GetItem(i).idxStart,  peList.GetItem(i).idxEnd));
		}
	}
	this->message = ERRMSG_OK;
	this->SetLoaded(true);
	return true;
}

// ---------------------------------------------------------------------------
// Структуры в файле
// ---------------------------------------------------------------------------
#pragma pack (push, 1)

struct VLineRecHeader		// Заголовок прямой в файле
{
	uint32_t dwSize;		// Размер структуры
	uint32_t dwVer;			// Версия структуры	
	uint32_t dwMarkSize;	// Размер структуры, описывающей метку	
	uint8_t cReserve[8];	// РЕЗЕРВ
};

struct VLineMarkArrHdr		// Заголовок массива меток в файле
{
	uint32_t dwSize;		// Размер структуры
	uint32_t dwVer;			// Версия структуры
	uint32_t dwParamType;	// Тип параметра в файле
	uint32_t dwMarkCount;	// Количество меток
	uint8_t cReserve[8];	// РЕЗЕРВ
};

struct VFileLineMark
{
	DWORD dwSize;			// Размер структуры
	VWayCoord wc;			// Координаты метки
};
#pragma pack (pop)

// загрузка меток
bool VLineMarkList::Load(const VBasicParameter & data, 
						 const VAbsCoordData & absCoord, 
						 const VAprinfo & aprInfo)
{
	this->Unload();
	size_t position = 0;	
	std::vector<VLineMark> tmp_marks;
	if (data.GetDataSize() > 0)
	{
		this->message.clear();
		
		// старая запись, версия 0 (v2) или 1 (v3), размер записи 12 (v2) или 0 (v3)
		if (data.GetVersion() <= 1 && data.GetRecSize() <= 12)
		{
			for (size_t lineNdx = 0; lineNdx < data.GetRecCount(); ++lineNdx)
			{
				// Заголовок прямой
				const VLineRecHeader * recHeader = (VLineRecHeader *) data.GetPtr(position);
				position += sizeof(VLineRecHeader);

				// Заголовок массива
				const VLineMarkArrHdr * markHdr = (VLineMarkArrHdr *) data.GetPtr(position);
				position += sizeof(VLineMarkArrHdr);

				for (size_t markNdx = 0; markNdx < markHdr->dwMarkCount; ++markNdx)
				{
					const VFileLineMark * fileMark = (VFileLineMark *) data.GetPtr(position);
					position += sizeof(VFileLineMark);

					VLineMark mark;
					mark.index = absCoord.FindIndex(aprInfo.GetAbsCoord(fileMark->wc), true);
					this->AddItem(mark);
				}
			}
		}
		else if (data.GetVersion() == 2 && data.GetRecSize() == sizeof(VFileMark))
		{
			for (size_t markNdx = 0; markNdx < data.GetRecCount(); ++markNdx)
			{
				const VFileMark * fileMark = (VFileMark *) data.GetPtr(position);
				position += sizeof(VFileMark);
				if (fileMark->mark_type != FILE_LABEL_LINE)
				{
					this->message = ERRMSG_RECORD_INCORRECT;
					this->SetLoaded(false);
					return false;
				}

				this->AddItem(VLineMark(fileMark->index));
			}
		}
	}

	this->message = ERRMSG_OK;
	this->SetLoaded(true);
	return true;
}

std::vector<VLineMark> VLineMarkList::ExportMarks(size_t idxFrom, size_t idxTo) const
{
	std::vector<VLineMark> lm;
	for (size_t i = 0; i < this->GetCount(); ++i)
	{
		if (this->GetItem(i).index > idxFrom && this->GetItem(i).index < idxTo)
		{
			lm.push_back(this->GetItem(i));
		}
	}
	return lm;
}

// копирование меток
bool VLineMarkList::CopyFrom(const VLineMarkList & src,
							 const VAbsCoordData & srcAbs, 
							 const VAbsCoordData & destAbs)
{
	this->Unload();

	for (size_t i = 0; i < src.GetCount(); ++i)
	{
		VLineMark mark = src.GetItem(i);
		if (destAbs.UpdateIndex(mark.index, srcAbs)) 
		{
			this->AddItem(mark);
		}
	}

	this->message = ERRMSG_OK;
	this->SetLoaded(true);
	return true;
}

// запись в файл
bool VLineMarkList::Save(VMainFile & file, uint32_t type, 
						 const VAbsCoordData & absCoord, 
						 const VAprinfo & aprInfo)
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

	VBasicParameter data;
	size_t position = 0;

	data.Create(type, VCT_USER, this->GetCount(), sizeof(VFileMark), 2);
	for (size_t mark = 0; mark < this->GetCount(); ++mark)
	{
		VFileMark fileMark;
		fileMark.index = this->GetItem(mark).index;
		fileMark.mark_type = FILE_LABEL_LINE;
		data.Write<VFileMark>(fileMark, position);
	}

	if (data.Save(file, this->GetCount(), sizeof(VFileMark)))
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
