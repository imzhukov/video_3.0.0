#include "VDataSet.h"
#include "VCoordPoint.h"
#include "strlib.h"

bool VCoordPointList::Load(VDataSet & ds, POINT_TYPE type, bool load_apr_abscoord)
{
	if (this->IsLoaded()) 
		return true;
	if(!ds.Load(PRM_ABS_COORD)) 
	{
		this->message = ERRMSG_MISSING_PARAMETER;
		return false;
	}

	//Вспомогательная переменная
	VCoordPoint new_coord_point;

	// загрузка параметров
	ds.Load(PRM_OP_MARKS);
	VAbsCoordData * absCoord = &ds.GetAbsCoord();
	VAprinfo *aprinfo = &ds.GetAprinfo();
	VOpMarkList *opmark = &ds.GetOpMarks();
	size_t count = 0; //счётчик для проверки загрузки
	switch (type)
	{
		case POINT_BRIDGE: 
			for (size_t i = 0; i < opmark->GetCount(); i++)
			{
				if (opmark->GetItem(i).isBridge)
				{
					new_coord_point.abscoord = absCoord->GetItem(opmark->GetItem(i).index);
					new_coord_point.apr_abscoord = absCoord->GetItem(opmark->GetItem(i).index);
					new_coord_point.id = L"мост";
					new_coord_point.type = type;
					new_coord_point.index = i;
					new_coord_point.filename = ds.GetFileName().GetPath();
					new_coord_point.isIgnore = opmark->GetItem(i).isIgnore;
					records.push_back(new_coord_point);				
					count++;
				}
			}
			break;

		case POINT_POLE:
			for (size_t i = 0; i < opmark->GetCount(); i++)
			{
				if (opmark->GetItem(i).isPole)
				{
					new_coord_point.abscoord = absCoord->GetItem(opmark->GetItem(i).index);
					wchar_t value[256];
					double abscoord_1 = GetAprAbscoord(absCoord->GetItem(opmark->GetItem(i).index),*aprinfo);
					int km_1 = aprinfo->GetRealCoord(abscoord_1).km;
					swprintf(value, 255, L"%i",km_1);
					new_coord_point.apr_abscoord = aprinfo->GetCoordPoint(2,wxString(value));
					if(!load_apr_abscoord || !new_coord_point.apr_abscoord)
						new_coord_point.apr_abscoord = abscoord_1;
					new_coord_point.id = value;
					new_coord_point.type = type;
					new_coord_point.index = i;
					new_coord_point.filename = ds.GetFileName().GetPath();
					new_coord_point.isIgnore = opmark->GetItem(i).isIgnore;
					if(load_apr_abscoord || !new_coord_point.isIgnore)
					{
						this->records.push_back(new_coord_point);
						count++;
					}
				}
			}
			break;

		case POINT_SPALA_MARKER:
			ds.Load(PRM_SPALA_MARKER);
			VSpalaMarkerList *spalamarker = &ds.GetSpalaMarker();
			for(size_t i = 0; i < spalamarker->GetCount(); i++)
			{
				new_coord_point.abscoord = absCoord->GetItem(spalamarker->GetItem(i).index);			
				wchar_t value[256];
				swprintf(value, 255, L"%0#10x",spalamarker->GetItem(i).value);
				new_coord_point.apr_abscoord = aprinfo->GetCoordPoint(1,wxString(value));
				new_coord_point.type = type;
				new_coord_point.index = i;
				new_coord_point.filename = ds.GetFileName().GetPath();
				new_coord_point.id = value;
				new_coord_point.isIgnore = false;
				if(!load_apr_abscoord || new_coord_point.apr_abscoord)
				{
					this->records.push_back(new_coord_point);
					count++;
				}
			}
			break;
	}
	
	if(count > 1)
		return true;
	else
		return false;
}

void VCoordPointList::Unload(VDataSet & ds)
{
	this->records.erase(this->records.begin(),this->records.end());
}

// Возврашает максимальное значение
double VCoordPointList::GetMax(POINT_TYPE in_type) const
{
	double value = (GetItem(0).abscoord - GetItem(0).apr_abscoord);
	for (size_t i = 1; i < this->GetCount(); ++i)
	{
		if(GetItem(i).type==in_type)
			value = std::max<double>(value, (GetItem(i).abscoord - GetItem(i).apr_abscoord));
	}
	return value;
}

// Возврашает минимальное значение
double VCoordPointList::GetMin(POINT_TYPE in_type) const
{
	double value = (GetItem(0).abscoord - GetItem(0).apr_abscoord);
	for (size_t i = 0; i < this->GetCount(); ++i)
	{
		if(GetItem(i).type==in_type)
			value = std::min<double>(value, (GetItem(i).abscoord - GetItem(i).apr_abscoord));
	}
	return value;
}

void VCoordPointList::DeletePoint(size_t in_index)
{
	this->records.erase(records.begin()+in_index);
}

double VCoordPointList::CalcDelta(POINT_TYPE in_type, VCoordPointList coord_point_list)
{
	double sum = 0;
	int count = 0;
	if(this->GetCount())
	{
		for(size_t i = 0; i < this->GetCount(); i++)
		{
			if(this->GetItem(i).type==in_type)
			{
				for(size_t j = 0; j < coord_point_list.GetCount(); j++)
				{
					if(this->GetItem(i).id==coord_point_list.GetItem(j).id)
					{
						sum+=(coord_point_list.GetItem(j).abscoord - this->GetItem(i).abscoord);
						count++;
						break;
					}
				}
			}
		}
		return sum/count;
	}
	else
		return 0;
}


double VCoordPointList::GetAprAbscoord(double abscoord, VAprinfo aprinfo)
{
	double apr_abscoord = 0;
	VWayCoord waycoord1 = aprinfo.GetRealCoord(abscoord);
	VWayCoord waycoord2;
	if(waycoord1.m < 500)
		waycoord2.km = waycoord1.km;
	if(waycoord1.m > 500)
		waycoord2.km = waycoord1.km + 1;
	apr_abscoord = aprinfo.GetAbsCoord(waycoord2);
	return apr_abscoord;
}

//---------------------Описание методов для класса автокоррекции координаты по различным объектам пути-------------------------
void VCoordPointAutoCorr::CalcLine(POINT_TYPE in_type)
{	
	double SumDelta=0;
	double SumDelta2=0;
	double SumCoord=0;
	double SumCoord2=0;
	double SumCoordDelta=0;
	int N = 0;
	for (size_t i = 0; i < GetCount(); i++)
	{
		if(GetItem(i).apr_abscoord!=GetItem(i).abscoord && GetItem(i).type == in_type && !(GetItem(i).isIgnore))
		{
			SumDelta += (GetItem(i).abscoord - GetItem(i).apr_abscoord);
			SumDelta2 += pow((GetItem(i).abscoord - GetItem(i).apr_abscoord),2);
			SumCoord += GetItem(i).abscoord;
			SumCoord2 += pow(GetItem(i).abscoord,2);
			SumCoordDelta += (GetItem(i).abscoord - GetItem(i).apr_abscoord)*GetItem(i).abscoord;
			N++;
		}
	}	
	//FixMe!!! проверить
	line_t result((SumCoordDelta-(SumCoord*SumDelta)/N)/(SumCoord2-pow(SumCoord,2)/N), -1, (SumDelta - SumCoord*
		(SumCoordDelta-(SumCoord*SumDelta)/N)/(SumCoord2-pow(SumCoord,2)/N))/N);
	if(in_type == POINT_SPALA_MARKER)
		line_spala_marker = result;
	if(in_type == POINT_POLE)
		line_pole = result;
}

line_t VCoordPointAutoCorr::GetLine(POINT_TYPE in_type)
{
	if(in_type == POINT_SPALA_MARKER)
		return line_spala_marker;
	else //(in_type == POINT_POLE)
		return line_pole;
}

void VCoordPointAutoCorr::SetCorrupt(VDataSet & dataSet, POINT_TYPE type, size_t index)
{
	if(GetPoint(type, index).isIgnore)
		GetPoint(type, index).isIgnore=false;
	else
		GetPoint(type, index).isIgnore=true;
	dataSet.Load(PRM_OP_MARKS);
	size_t index_op_mark = this->GetItem(index).index; //индекс из массива меток оператора
	dataSet.GetOpMarks().GetItem(index_op_mark).isIgnore = GetItem(index).isIgnore;
	dataSet.SetChanged(PRM_OP_MARKS);
}

VCoordPoint & VCoordPointAutoCorr::GetPoint(POINT_TYPE in_type, size_t index)
{
	if(start_index[in_type] == NULL)
	{
		for(size_t i = 0; i < GetCount(); i++)
			if(GetItem(i).type == in_type)
			{
				start_index[in_type] = i;
				break;
			}
	}
	return GetItem(index+start_index[in_type]);
}

size_t VCoordPointAutoCorr::GetCountPoint(POINT_TYPE in_type)
{
	size_t count = 0;
	for(size_t i = 0; i < GetCount(); i++)
		if(GetItem(i).type == in_type)
			count++;
	return count;
}

size_t VCoordPointAutoCorr::GetCountNotCorrupt(POINT_TYPE in_type)
{
	size_t count = 0;
	for(size_t i = 0; i < GetCount(); i++)
		if(GetItem(i).type == in_type && !GetItem(i).isIgnore)
			count++;
	return count;
}
//---------------------Описание методов для класса реперныч точек-------------------------------------------------------------
bool VRepPointList::FirstCheckData()
{
	//Делаем проверку только для меток шпал
	//Километровые столбы измерены корректно
	//Сделано в целях экономии времени
	if(GetCount() && GetItem(0).type==POINT_SPALA_MARKER)
	{
		for(size_t i = 0; i < GetCount(); i++)
		{
			for(size_t j = i+1; j < GetCount(); j++)
			{
				if(GetItem(j).id==GetItem(i).id && abs(GetItem(i).abscoord - GetItem(j).abscoord) > 50)
					return false;
			}
		}
	}
	this->Sort();
	return true;
}

void VRepPointList::SortForGrid(std::map<std::wstring,int> Filenames)
{
	int current_row = 0;
	double sum = 0;
	double sum2 = 0;
	int count = 0;
	for(size_t k = 0; k < GetCount(); k++)
	{
		GetItem(k).row = current_row;
		GetItem(k).col = Filenames[GetItem(k).filename]+2;
		sum+=GetItem(k).abscoord;
		sum2+=GetItem(k).abscoord * GetItem(k).abscoord;
		count++;
		if(k == GetCount()-1 || GetItem(k).id != GetItem(k+1).id)
		{
			average.push_back(sum/count);
			if(sum2/count - (sum/count)*(sum/count) > 0)
				sko.push_back(sqrt(sum2/count - (sum/count)*(sum/count)));
			else
				sko.push_back(0);
			current_row++;
			sum = 0;
			sum2 = 0;
			count = 0;
		}
	}
}

double VRepPointList::GetAverage(int row)
{
	return average[row];
}

double VRepPointList::GetSKO(int row)
{
	return sko[row];
}

double VRepPointList::GetMaxSKO()
{
	double value = 0;
	if(sko.size()!=0)
	{
		for(size_t i = 0; i < sko.size(); i++)
			value = std::max<double>(value,sko[i]);
	}
	return value;
}

void VRepPointList::ReCalc(double porog)
{
	int current_row = 0;
	int count = 0;
	double sum = 0;
	double sum2 = 0;
	for(size_t i = 0; i < GetCount(); i++)
	{
		if(abs(GetItem(i).abscoord - average[GetItem(i).row]) > porog)
			GetItem(i).isIgnore = true;
		else if(!GetItem(i).isIgnore)
		{
			sum+=GetItem(i).abscoord;
			sum2+=GetItem(i).abscoord*GetItem(i).abscoord;
			count++;
		}
		if(i == GetCount()-1 || GetItem(i).id != GetItem(i+1).id)
		{
			if(count)
			{
				average[current_row] = sum/count;
				sko[current_row] = sqrt(sum2/count - (sum/count)*(sum/count));
			}
			else
			{
				average[current_row] = 0;
				sko[current_row] = 0;
			}
			current_row++;
			sum = 0;
			sum2 = 0;
			count = 0;
		}
	}
}

VCoordPoint VRepPointList::GetPoint(int row, int col)
{
	VCoordPoint coord_point;
	double sum = 0;
	double sum2 = 0;
	int count = 0;
	for(size_t i = 0; i < GetCount(); i++)
	{
		if(GetItem(i).row == row && GetItem(i).col == col)
		{
			if(GetItem(i).isIgnore)
			{
				sum+=GetItem(i).abscoord;
				sum2+=GetItem(i).abscoord*GetItem(i).abscoord;
				count++;
				GetItem(i).isIgnore = false;
			}
			else
				GetItem(i).isIgnore = true;
			coord_point = GetItem(i);
		}
		else if(GetItem(i).row == row && !GetItem(i).isIgnore)
		{
			sum+=GetItem(i).abscoord;
			sum2+=GetItem(i).abscoord*GetItem(i).abscoord;
			count++;
		}
	}
	if(count)
	{
		average[row] = sum/count;
		sko[row] = sqrt(sum2/count - (sum/count)*(sum/count));
	}
	else
	{
		average[row] = 0;
		sko[row] = 0;
	}
	return coord_point;
}

void VRepPointList::Reload()
{
	int current_row = 0;
	double sum = 0;
	double sum2 = 0;
	int count = 0;
	for(size_t i = 0; i < GetCount(); i++)
	{
		GetItem(i).isIgnore = false;
		sum+=GetItem(i).abscoord;
		sum2+=GetItem(i).abscoord * GetItem(i).abscoord;
		count++;
		if(i == GetCount()-1 || GetItem(i).id != GetItem(i+1).id)
		{
			average[current_row]=sum/count;
			sko[current_row]=sqrt(sum2/count - (sum/count)*(sum/count));
			current_row++;
			sum = 0;
			sum2 = 0;
			count = 0;
		}
	}
}