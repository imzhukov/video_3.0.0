#include "VZazor.h"

// Загрузка из параметра
bool VZazorList::Load(const VParameter & prm, VParameterCorruptPtr corr)
{
	bool completeZeros = true;
	for (size_t i = 0; i < prm.GetCount(); ++i)
	{
		if (prm.GetItem(i) != 0.0f)
		{
			completeZeros = false;
			break;
		}
	}

	float prev_value = -1.0f;
	for (size_t i = 0; i < prm.GetCount(); ++i)
	{
		float value = prm.GetItem(i);
		if (value >= 0.0f)
		{
			if (value != prev_value)
			{
				VZazor zazor;
				zazor.index = i;
				zazor.value = value;
				this->records.push_back(zazor);
			}
		}
		prev_value = value;
	}
	
	this->kadrCount = prm.GetCount();
	this->corrupt = corr;
	this->SetLoaded(true);

/*	std::ofstream dz("d:\\dump_zaz.txt";
	if (dz.is_open())
	{
		dz << "index value corrupt" << std::endl;
		for (size_t i = 0; i < this->records.size(); ++i)
		{
			dz << this->records.at(i).index << " ";
			dz << this->records.at(i).value << " ";
			dz << (this->corrupt->IsCorrupt(this->records.at(i).index) ? "+" : "-";
			dz << std::endl;
		}
		dz.close();
	}*/

	this->message = ERRMSG_OK;
	return true;
}

// Загрузка из файла
bool VZazorList::Load(VMainFile & file, uint32_t type, VParameterCorruptPtr corr)
{
	if (this->IsLoaded()) return true;

	VParameter zazorOnDisk;
	if (!zazorOnDisk.Load(file, type))
	{
		this->message = zazorOnDisk.GetErrorMessage();
		return false;
	}

	return this->Load(zazorOnDisk, corr);
}

// Загрузка из файла
bool VZazorList::Load(VRemoteFile & remote, uint32_t type, VParameterCorruptPtr corr)
{
	if (this->IsLoaded()) return true;

	VParameter zazorOnDisk;
	if (!zazorOnDisk.Load(remote, type))
	{
		this->message = zazorOnDisk.GetErrorMessage();
		return false;
	}

	return this->Load(zazorOnDisk, corr);
}

// Сохранение в файл
bool VZazorList::Save(VMainFile & file, uint32_t type)
{
	if (!this->IsLoaded() || this->kadrCount == 0) return false;

	VParameter zazorOnDisk;
	zazorOnDisk.Create(type, this->kadrCount, 1, -1.0f);
	for (size_t i = 0; i < this->GetCount(); ++i)
		zazorOnDisk.GetItem(this->GetItem(i).index) = this->GetItem(i).value;

	if (!zazorOnDisk.Save(file, type))
	{
		this->message = zazorOnDisk.GetErrorMessage();
		return false;
	}
	else
	{
		this->message = ERRMSG_OK;
		return true;
	}
}

// Копирование из другого списка, с пересчетом координат
bool VZazorList::CopyFrom(const VZazorList & src, 
						  const VAbsCoordData & srcAbs, 
						  const VAbsCoordData & destAbs)
{
	if (VIndexList<VZazor>::CopyFrom(src, srcAbs, destAbs))
	{
		this->kadrCount = destAbs.GetCount();
		return true;
	}
	else
	{
		this->kadrCount = 0;
		return false;
	}
}

VZazorList VZazorList::FindZazor(uint32_t index, double range, const VAbsCoordData & absCoordData)
{
	VZazorList zazorList;
	double absCoord = absCoordData.GetItem(index);
	for(size_t i = 0; i < GetCount(); i++)
	{
		if(abs(absCoordData.GetItem(GetItem(i).index) - absCoord) < range)
			zazorList.AddItem(GetItem(i));
	}
	return zazorList;
}

VZazorList VZazorList::FindZazor(uint32_t index, TYPE_FINDING type)
{
	VZazorList zazorList;
	switch(type)
	{
		case NEXT_ZAZOR:
			for(size_t i = 0; i < GetCount(); i++)
			{
				int delta_index = GetItem(i).index - index;
				if(delta_index > 0)
				{
					zazorList.AddItem(GetItem(i));
					break;
				}
			}
			break;
		case PREVIOUS_ZAZOR:
			for(size_t i = GetCount()-1; i > 0; i--)
			{
				int delta_index = GetItem(i).index - index;
				if(delta_index < 0)
				{
					zazorList.AddItem(GetItem(i));
					break;
				}
			}
			break;
		case CLOSE_ZAZOR:
			int radius = 20; //здесь задаём радиус поиска ближайшего зазора
			for(size_t i = 0; i < GetCount(); i++)
			{
				int delta_index = GetItem(i).index - index;
				if(abs(delta_index) < radius)
					zazorList.AddItem(GetItem(i));
				else if (delta_index > 0)
					break;
			}
			break;
	}
	return zazorList;
}

// Вернуть сбойность указанного зазора (номер зазора, не кадра!!!!)
bool VZazorList::IsCorrupt(size_t index) const
{
	if (this->GetCount() <= index) return false;
	if (this->corrupt->GetCount() <= this->GetItem(index).index) return false;
	return this->corrupt->IsCorrupt(this->GetItem(index).index);
}

// Установить сбойность указанного зазора (номер зазора, не кадра!!!!)
void VZazorList::SetCorrupt(size_t index, bool value)
{
	if (this->GetCount() <= index) return;
	if (this->corrupt->GetCount() <= this->GetItem(index).index) return;
	this->corrupt->SetCorrupt(this->GetItem(index).index, value);
}
