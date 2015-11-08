#include "VAprinfo.h"

// типы данных в файле
const long prmKm			= 100; ///< Километры
const long prmSpeed			= 101; ///< Скорости по приказу
const long prmSpeedTmp		= 102; ///< Временные скорости
const long prmNWidth		= 103; ///< Норма ШК
const long prmNLevel		= 104; ///< Норма возвышения
const long prmPch			= 105; ///< ПЧ
const long prmSpala			= 106; ///< Тип шпал
const long prmPnt			= 107; ///< Перегоны, станции и раздельные пункты
const long prmSwitch		= 108; ///< Стрелки
const long prmCurve			= 109; ///< Кривые
const long prmBridge		= 110; ///< Мосты
const long prmTonnel		= 111; ///< Тоннели
const long prmProf			= 112; ///< Метки профиля
const long prmRoute			= 113; ///< Точки съезда
const long prmWobLeft		= 114; ///< Бесстыковые плети левые
const long prmWobRight		= 115; ///< Бесстыковые плети правые
const long prmWobLeftEx		= 116; ///< Бесстыковые плети левые (с темп.на метках)
const long prmWobRightEx	= 117; ///< Бесстыковые плети правые (с темп.на метках)
const long prmSpeedEx		= 118; ///< Скорости по приказу (+speed3)
const long prmSpeedTmpEx	= 119; ///< Временные скорости (+speed3)
const long prmNWidthEx		= 120; ///< Норма ШК (сохр.знач.по умлолч.)
const long prmNLevelEx		= 121; ///< Норма возвышения (сохр.знач.по умлолч.)
const long prmSpeedProj		= 122; ///< Проектные скорости
const long prmSpeedProjEx	= 123; ///< Проектные скорости (+speed3)
const long prmCurveEx       = 124; ///< Кривые (+параметры элементарных кривых)
const long prmWayCat        = 125; ///< Категория пути
const long prmHole			= 126; ///< Отверстия в рельсах
const long prmPntEx         = 127; ///< Станции и перегоны, +ось +код станции
const long prmPchEx         = 128; ///< ПЧ+НОД
const long prmKmEx			= 129; ///< Нат.Км, длина как float
const long prmCross			= 130; ///< Переезды
const long prmMaxParam      = 130; ///< Максимальное значение. Дабы не искать его
const long prmEof			= 255; ///< Конец данных

bool VAprinfo::Load(const VBasicParameter & data)
{
	// позиция в блоке данных
	size_t position = 0;

	// сигнатура блока апринфо
	char signature[9] = {0};
	data.ReadBlock(signature, 8, position);
	if (strcmp(signature, "APRINFO ") != 0)
	{
		this->message = ERRMSG_VERSION_INCORRECT;
		return false;
	}

	long dataType = 0, count = 0, count2 = 0;
	VAprinfoKmRecord curKm;
	VAprinfoObjectRecord curObject;
	VAprinfoSwitchRecord curSwitch;
	VAprinfoIndexRecord curIndex;
	VAprinfoSpeedRecord curSpeed;
	VAprinfoPchRecord curPch;
	VAprinfoWobRecord curWob;
	VAprinfoWobLabel curWobLabel;
	VAprinfoPntRecord curPnt;
	VAprinfoCrvLabel curCrvLabel;
	VAprinfoCrvRadius curCrvRadius;
	VAprinfoCurve curCurve;
	VAprinfoProfLabel curProf;
	VAprinfoBasicRecord curRoute;
	
	// загрузка/незагрузка отдельных параметров (не везде)
	bool do_load = true; 

	// путь - короткая и длинная версия
	char WayShort[4], WayLong[32];
	// имя направления
	char DirName[81];

	// версия 
	long version;
	data.Read<long>(version, position);
	switch (version)
	{
	case 1:

		// границы участка, в абс. координатах
		//position += 2 * sizeof(double);
		data.Read<double>(this->absCoordBase, position);
		data.Read<double>(this->absCoordEnd, position);

		// пропускаем размер структуры VDirection
		position += sizeof(long);

		// код направления
		data.Read<long>(this->dirCode, position);

		// имя направления
		data.ReadBlock(DirName, 81, position);
		DirName[80] = 0;
		this->dirName = DirName;
		position += 3;

		// размер структуры VMainWay
		position += sizeof(long);

		// Секция
		data.Read<VWaySection>(this->section, position);

		// Путь из 4 символов
		data.ReadBlock(WayShort, 4, position);
		WayShort[3] = 0;

		// маска параметров
		position += sizeof(long);

		// длинный путь
		data.ReadBlock(WayLong, 32, position);
		WayLong[31] = 0;

		// в длинном пути нечто не соответствующее короткому
		if (strncmp(WayShort, WayLong, 3) != 0)
		{
			this->wayName = WayShort;
		}
		else
		{
			this->wayName = WayLong;
		}

		// хвост заголовка
		position += 72;

		while (true)
		{
			// тип данных и число записей
			data.Read<long>(dataType, position);
			if (dataType == prmEof)
			{
				this->SetDefaults();
				this->SetLoaded(true);
				this->message = ERRMSG_OK;
				return true;
			}

			data.Read<long>(count, position);
			switch (dataType)
			{

			// километры
			case prmKm:
			case prmKmEx:

				this->kmList.Reserve(count);
				position += sizeof(long);
				for (long i = 0; i < count; ++i)
				{
					data.Read<double>(curKm.base, position);
					data.Read<double>(curKm.end, position);
					data.Read<long>(curKm.km, position);
					if (dataType == prmKm)
					{
						long tmpValue;
						data.Read<long>(tmpValue, position);
						curKm.len = static_cast<float>(tmpValue);
					}
					else
					{
						data.Read<float>(curKm.len, position);
					}
					this->kmList.Add(curKm);
				}
				
				break;

			// скорости
			case prmSpeed:
			case prmSpeedEx:
			case prmSpeedTmp:
			case prmSpeedTmpEx:
			case prmSpeedProj:
			case prmSpeedProjEx:

				switch(dataType)
				{
				case prmSpeed:
				case prmSpeedEx:	

					this->speedList.Reserve(count); 
					break;

				case prmSpeedTmp:
				case prmSpeedTmpEx:

					this->speedTmpList.Reserve(count); 
					break;

				case prmSpeedProj:
				case prmSpeedProjEx: 

					do_load = true;
					break;
				}

				for (long i = 0; i < count; ++i)
				{
					data.Read<double>(curSpeed.base, position);
					data.Read<double>(curSpeed.end, position);
					if (dataType == prmSpeed || dataType == prmSpeedTmp || dataType == prmSpeedProj)
					{
						unsigned char tmpValue1, tmpValue2;
						data.Read<unsigned char>(tmpValue1, position);
						data.Read<unsigned char>(tmpValue2, position);
						curSpeed.spd = VSpeeds(tmpValue1, tmpValue2, tmpValue2);
					}
					else // prmSpeed(Blah-Blah-Blah)Ex
					{
						long tmpValue1, tmpValue2, tmpValue3;
						data.Read<long>(tmpValue1, position);
						data.Read<long>(tmpValue2, position);
						data.Read<long>(tmpValue3, position);
						curSpeed.spd = VSpeeds(tmpValue1, tmpValue2, tmpValue3);
					}

					switch(dataType)
					{
					case prmSpeed:
					case prmSpeedEx:	
						this->speedList.Add(curSpeed); break;

					case prmSpeedTmp:
					case prmSpeedTmpEx:
						this->speedTmpList.Add(curSpeed); break;

					case prmSpeedProj:
					case prmSpeedProjEx: 
						this->speedProjList.Add(curSpeed); break;
					}
				}
				break;

			// нормы ШК
			case prmNWidth:
			case prmNWidthEx:

				// норма по умолчанию, плюс странный хвост
				if (dataType == prmNWidthEx) position += 2 * sizeof(long);
				this->nwidthStraightList.Reserve(count);
				for (long i = 0; i < count; ++i)
				{
					data.Read<double>(curIndex.base, position);
					data.Read<double>(curIndex.end, position);
					data.Read<long>(curIndex.value, position);
					this->nwidthStraightList.Add(curIndex);
				}
				break;
			
			// нормы возвышения
			case prmNLevel:
			case prmNLevelEx:

				// норма по умолчанию, плюс странный хвост
				if (dataType == prmNLevelEx) position += 8;
				this->nlevelStraightList.Reserve(count);
				for (long i = 0; i < count; ++i)
				{
					data.Read<double>(curIndex.base, position);
					data.Read<double>(curIndex.end, position);
					data.Read<long>(curIndex.value, position);
					this->nlevelStraightList.Add(curIndex);
				}
				break;

			// ПЧ
			case prmPch:
			case prmPchEx:

				this->pchList.Reserve(count);
				for (long i = 0; i < count; ++i)
				{
					data.Read<double>(curPch.base, position);
					data.Read<double>(curPch.end, position);
					data.Read<long>(curPch.pchCode, position);
					if (dataType == prmPchEx)
						data.Read<long>(curPch.nodCode, position);
					curPch.roadCode = 0;
					pchList.Add(curPch);
				}
				break;

			// плети
			case prmWobLeft:
			case prmWobLeftEx:
			case prmWobRight:
			case prmWobRightEx:

				switch (dataType)
				{
				case prmWobLeft:
				case prmWobLeftEx:		
					this->wobLeftList.Reserve(count);
					break;

				case prmWobRight:
				case prmWobRightEx:		
					this->wobRightList.Reserve(count); 
					break;
				}	
				
				for (long i = 0; i < count; ++i)
				{
					data.Read<double>(curWob.base, position);
					data.Read<double>(curWob.end, position);
					data.Read<bool>(curWob.isThere, position);
					// num, min.temp, max.temp, type
					position += 4 * sizeof(long);
					data.Read<long>(count2, position);
					for (long j = 0; j < count2; ++j)
					{
						data.Read<double>(curWobLabel.coord, position);
						data.Read<long>(curWobLabel.fixTemp, position);
						if (dataType == prmWobLeftEx || dataType == prmWobRightEx)
						{
							data.Read<long>(curWobLabel.minTemp, position);
							data.Read<long>(curWobLabel.maxTemp, position);
						}
						curWob.labels.push_back(curWobLabel);
					}
			
				switch (dataType)
					{
					case prmWobLeft:
					case prmWobLeftEx:		
						this->wobLeftList.Add(curWob); 
						break;

					case prmWobRight:
					case prmWobRightEx:		
						this->wobRightList.Add(curWob); 
						break;
					}
				}
				break;

			// станции и перегоны
			case prmPnt:
			case prmPntEx:

				this->pntList.Reserve(count);
				for (long i = 0; i < count; ++i)
				{
					data.Read<double>(curPnt.base, position);
					data.Read<double>(curPnt.end, position);
					data.Read<bool>(curPnt.isThere, position);
					if (dataType == prmPntEx)
					{
						data.Read<long>(curPnt.code, position);
						data.Read<double>(curPnt.axis, position);
					}
					else
					{
						curPnt.code = 0;
						curPnt.axis = 0.5 * (curPnt.base + curPnt.end);
					}
					data.Read<long>(count2, position);
					char * buf = (char *) ::GlobalAlloc(GPTR, count2+1);
					data.ReadBlock(buf, count2, position);
					curPnt.name = buf;
					data.ReadBlock(buf, count2, position);
					curPnt.nameReverse = buf;
					::GlobalFree(buf);
					this->pntList.Add(curPnt);
				}
				break;

			// кривые
			case prmCurve:
			case prmCurveEx:

				this->crvPassport.curveList.Reserve(count);
				for (long i = 0; i < count; ++i)
				{
					curCurve.Clear();

					data.Read<double>(curCurve.base, position);
					data.Read<double>(curCurve.end, position);
					data.Read<bool>(curCurve.isThere, position);
					position += sizeof(long) + sizeof(float);

					data.Read<long>(count2, position);
					curCurve.planLabel.reserve(count2);
					curCurve.levelLabel.reserve(count2);
					for (long j = 0; j < count2; ++j) 
					{
						data.Read<double>(curCrvLabel.coord, position);
						data.Read<float>(curCrvLabel.value, position);
						curCurve.planLabel.push_back(curCrvLabel);
						data.Read<double>(curCrvLabel.coord, position);
						data.Read<float>(curCrvLabel.value, position);
						curCurve.levelLabel.push_back(curCrvLabel);
						position += 2 * (sizeof(double) + sizeof(float));
					}

					if (dataType == prmCurve)
					{
						if (curCurve.planLabel.size() > 1)
						{
							count2 = (long) curCurve.planLabel.size() / 2 - 1;
							curCurve.radius.reserve(count2);
						}
						else
						{
							count2 = 0;
						}

						for (long j = 0; j < count2; ++j)
						{
							curCurve.radius.push_back(curCrvRadius);
						}
					}
					else
					{
						data.Read<long>(count2, position);
						curCurve.radius.reserve(count2);
						for (long j = 0; j < count2; ++j)
						{
							long w;
							data.Read<long>(w, position);
							curCrvRadius.width = (double) w;
							curCurve.radius.push_back(curCrvRadius);
						}
					}
					this->crvPassport.curveList.Add(curCurve);
				}
				break;

			// шпалы, мосты, тоннели, категории пути, отверстия в рельсах
			case prmSpala:
			case prmBridge:
			case prmTonnel:
			case prmWayCat:
			case prmHole:

				switch (dataType)
				{
				case prmSpala:	
					this->spalaList.Reserve(count); 
					break;

				case prmBridge:	
					this->bridgeList.Reserve(count);
					break;

				case prmTonnel:	
					this->tonnelList.Reserve(count);
					break;

				case prmWayCat:	
					this->wayCatList.Reserve(count);
					break;

				case prmHole:	
					this->holeList.Reserve(count); 
					break;
				}

				for (long i = 0; i < count; ++i)
				{
					data.Read<double>(curIndex.base, position);
					data.Read<double>(curIndex.end, position);
					data.Read<long>(curIndex.value, position);
					switch (dataType)
					{
					case prmSpala:	this->spalaList.Add(curIndex); break;
					case prmBridge:	this->bridgeList.Add(curIndex); break;
					case prmTonnel:	this->tonnelList.Add(curIndex); break;
					case prmWayCat:	this->wayCatList.Add(curIndex); break;
					case prmHole:	this->holeList.Add(curIndex); break;
					}
				}
				break;

			// стрелки
			case prmSwitch:

				this->switchList.Reserve(count);
				for (long i = 0; i < count; ++i)
				{
					data.Read<double>(curSwitch.base, position);
					data.Read<double>(curSwitch.end, position);
					data.Read<bool>(curSwitch.isThere, position);
					curSwitch.dir = SDIR_UNDEF;
					curSwitch.side = 0;
					curSwitch.o = 0;
					curSwitch.x = 0;
					this->switchList.Add(curSwitch);
				}
				break;

			// метки профиля
			case prmProf:

				this->profList.Reserve(count);
				for (long i = 0; i < count; ++i)
				{
					data.Read<double>(curProf.coord, position);
					data.Read<float>(curProf.value, position);
					this->profList.Add(curProf);
				}
				break;

			// куски маршрута
			case prmRoute:

				this->routeList.Reserve(count);
				for (long i = 0; i < count; ++i)
				{
					data.Read<double>(curRoute.base, position);
					data.Read<double>(curRoute.end, position);
					this->routeList.Add(curRoute);
				}
				break;

			// конец
			case prmEof:
				this->SetDefaults();
				this->SetLoaded(true);
				this->message = ERRMSG_OK;
				return true;

			// что-то забыли
			default:
				this->SetLoaded(false);
				this->message = ERRMSG_DATA_INCOMPLETE;
				return false;
			}
		}
	}

	this->SetLoaded(true);
	this->message = ERRMSG_OK;
	return true;
}

uint32_t VAprinfo::CalcBlockSize() const
{
	size_t dataBlkSize = 8 * sizeof(char) + sizeof(long) + 2 * sizeof(double);

	// заголовок
	dataBlkSize += sizeof(unsigned long) + sizeof(long) + 84 + sizeof(unsigned long) + sizeof(VWaySection) + 4 + sizeof(unsigned long) + 104;
	// prmKmEx
	dataBlkSize += 3 * sizeof(long) + this->kmList.GetCount() * (2 * sizeof(double) + sizeof(long) + sizeof(float));
	// prmSpeedEx
	dataBlkSize += 2 * sizeof(long) + this->speedList.GetCount() * (2 * sizeof(double) + 3 * sizeof(long));
	// prmSpeedTmpEx
	dataBlkSize += 2 * sizeof(long) + this->speedTmpList.GetCount() * (2 * sizeof(double) + 3 * sizeof(long));
	// prmSpeedProjEx
	dataBlkSize += 2 * sizeof(long) + this->speedProjList.GetCount() * (2 * sizeof(double) + 3 * sizeof(long));
	// prmNWidthEx
	dataBlkSize += 4 * sizeof(long) + this->nwidthStraightList.GetCount() * (2 * sizeof(double) + sizeof(long));
	// prmNLevelEx
	dataBlkSize += 4 * sizeof(long) + this->nlevelStraightList.GetCount() * (2 * sizeof(double) + sizeof(long));
	// prmPchEx
	dataBlkSize += 2 * sizeof(long) + this->pchList.GetCount() * (2 * sizeof(double) + 2 * sizeof(long));
	// prmSpala
	dataBlkSize += 2 * sizeof(long) + this->spalaList.GetCount() * (2 * sizeof(double) + sizeof(long));
	// prmSwitch
	dataBlkSize += 2 * sizeof(long) + this->switchList.GetCount() * (2 * sizeof(double) + sizeof(bool));
	// prmBridge
	dataBlkSize += 2 * sizeof(long) + this->bridgeList.GetCount() * (2 * sizeof(double) + sizeof(long));
	// prmTonnel
	dataBlkSize += 2 * sizeof(long) + this->tonnelList.GetCount() * (2 * sizeof(double) + sizeof(long));
	// prmRoute
	dataBlkSize += 2 * sizeof(long) + this->routeList.GetCount() * (2 * sizeof(double));
	// prmProf
	dataBlkSize += 2 * sizeof(long) + this->profList.GetCount() * (sizeof(double) + sizeof(float));
	// prmCurveEx
	dataBlkSize += 2 * sizeof(long);
	for (size_t i = 0; i < this->crvPassport.curveList.GetCount(); ++i)
	{
		dataBlkSize += 2 * sizeof(double) + sizeof(bool) + sizeof(long) + sizeof(float);
		dataBlkSize += sizeof(long) + this->crvPassport.curveList.GetItem(i).planLabel.size() * (sizeof(double) + sizeof(float));
		dataBlkSize += this->crvPassport.curveList.GetItem(i).levelLabel.size() * (sizeof(double) + sizeof(float));
		dataBlkSize += this->crvPassport.curveList.GetItem(i).planLabel.size() * (sizeof(double) + sizeof(float));
		dataBlkSize += this->crvPassport.curveList.GetItem(i).planLabel.size() * (sizeof(double) + sizeof(float));
		dataBlkSize += sizeof(long) + this->crvPassport.curveList.GetItem(i).radius.size() * (sizeof(long));
	}
	// prmPntEx
	dataBlkSize += 2 * sizeof(long);
	for (size_t i = 0; i < this->pntList.GetCount(); ++i)
	{
		dataBlkSize += 2 * sizeof(double) + sizeof(bool) + sizeof(long) + sizeof(double);
		dataBlkSize += sizeof(long) + 2 * this->pntList.GetItem(i).name.length();
	}
	// prmWobLeftEx
	dataBlkSize += 2 * sizeof(long);
	for (size_t i = 0; i < this->wobLeftList.GetCount(); ++i)
	{
		dataBlkSize += 2 * sizeof(double) + sizeof(bool) + 5 * sizeof(long);
		dataBlkSize += this->wobLeftList.GetItem(i).labels.size() * (sizeof(double) + 3 * sizeof(long));
	}
	// prmWobRightEx
	dataBlkSize += 2 * sizeof(long);
	for (size_t i = 0; i < this->wobRightList.GetCount(); ++i)
	{
		dataBlkSize += 2 * sizeof(double) + sizeof(bool) + 5 * sizeof(long);
		dataBlkSize += this->wobRightList.GetItem(i).labels.size() * (sizeof(double) + 3 * sizeof(long));
	}
	// prmWayCat
	dataBlkSize += 2 * sizeof(long) + this->wayCatList.GetCount() * (2 * sizeof(double) + sizeof(long));
	// prmHole
	dataBlkSize += 2 * sizeof(long) + this->holeList.GetCount() * (2 * sizeof(double) + sizeof(long));
	// prmEof
	dataBlkSize += 2 * sizeof(long);

	return dataBlkSize;
}

static void WriteSpeeds(VBasicParameter & data, 
						VAprinfoParameter<VAprinfoSpeedRecord> & spd,
						size_t & position)
{
	data.Write<long>(spd.GetCount(), position);
	for (size_t i = 0; i < spd.GetCount(); ++i)
	{
		data.Write<double>(spd.GetItem(i).base, position);
		data.Write<double>(spd.GetItem(i).end, position);
		data.Write<long>(spd.GetItem(i).spd.GetPassSpeed(), position);
		data.Write<long>(spd.GetItem(i).spd.GetTruckSpeed(), position);
		data.Write<long>(spd.GetItem(i).spd.GetEmptySpeed(), position);
	}
}

static void WriteNorma(VBasicParameter & data, 
					   VAprinfoParameter<VAprinfoIndexRecord> & norma,
					   long def_value,
					   size_t & position)
{
	data.Write<long>(norma.GetCount(), position);
	data.Write<long>(def_value, position);
	data.Write<long>(0L, position); //// ?????
	for (size_t i = 0; i < norma.GetCount(); ++i)
	{
		data.Write<double>(norma.GetItem(i).base, position);
		data.Write<double>(norma.GetItem(i).end, position);
		data.Write<long>(norma.GetItem(i).value, position);
	}
}

static void WriteIndexList(VBasicParameter & data,
						   VAprinfoParameter<VAprinfoIndexRecord> & il,
						   size_t & position)
{
	data.Write<long>(il.GetCount(), position);
	for (size_t i = 0; i < il.GetCount(); ++i)
	{
		data.Write<double>(il.GetItem(i).base, position);
		data.Write<double>(il.GetItem(i).end, position);
		data.Write<long>(il.GetItem(i).value, position);
	}
}

static void WriteObjectList(VBasicParameter & data,
						    VAprinfoParameter<VAprinfoObjectRecord> & ol,
						    size_t & position)
{
	data.Write<long>(ol.GetCount(), position);
	for (size_t i = 0; i < ol.GetCount(); ++i)
	{
		data.Write<double>(ol.GetItem(i).base, position);
		data.Write<double>(ol.GetItem(i).end, position);
		data.Write<bool>(ol.GetItem(i).isThere, position);
	}
}

static void WriteSwitchList(VBasicParameter & data,
						    VAprinfoParameter<VAprinfoSwitchRecord> & ol,
						    size_t & position)
{
	data.Write<long>(ol.GetCount(), position);
	for (size_t i = 0; i < ol.GetCount(); ++i)
	{
		data.Write<double>(ol.GetItem(i).base, position);
		data.Write<double>(ol.GetItem(i).end, position);
		data.Write<bool>(ol.GetItem(i).isThere, position);
	}
}

static void WriteWob(VBasicParameter & data,
					 VAprinfoParameter<VAprinfoWobRecord> & wob,
					 size_t & position)
{
	data.Write<long>(wob.GetCount(), position);
	for (size_t i = 0; i < wob.GetCount(); ++i)
	{
		data.Write<double>(wob.GetItem(i).base, position);
		data.Write<double>(wob.GetItem(i).end, position);
		data.Write<bool>(wob.GetItem(i).isThere, position);
		data.Write<long>(0, position); // fake num
		data.Write<long>(0, position); // fake minTemp
		data.Write<long>(0, position); // fake maxTemp
		data.Write<long>(0, position); // fake type
		data.Write<long>(wob.GetItem(i).labels.size(), position);
		for (size_t k = 0; k < wob.GetItem(i).labels.size(); ++k)
		{
			data.Write<double>(wob.GetItem(i).labels[k].coord, position);
			data.Write<long>(wob.GetItem(i).labels[k].fixTemp, position);
			data.Write<long>(wob.GetItem(i).labels[k].minTemp, position);
			data.Write<long>(wob.GetItem(i).labels[k].maxTemp, position);
		}
	}
}

bool VAprinfo::Save(VMainFile & file, uint32_t type)
{
	VBasicParameter data;
	if (!this->Save(data, type))
	{
		this->message = ERRMSG_DATA_CREATION_ERROR;
		return false;
	}

	if (data.Save(file, 0, 0))
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

bool VAprinfo::Save(VBasicParameter & data, uint32_t type)
{
	uint32_t blkSize = this->CalcBlockSize();
	if (!data.Create(type, VCT_USER, 1, blkSize, 1)) 
	{
		this->message = ERRMSG_BLOCK_CREATION_FAILED;
		return false;
	}

	const char * signature = "APRINFO ";
	long version = 1;
	unsigned long mask = 0xFFFFFFFF;
	size_t position = 0;

	data.WriteBlock(signature, 8, position);
	data.Write<long>(version, position);
	data.Write<double>(this->absCoordBase, position);
	data.Write<double>(this->absCoordEnd, position);

	// VDirection
	unsigned long dir_struct_size = 92;
	data.Write<unsigned long>(dir_struct_size, position);
	data.Write<long>(this->dirCode, position);
	char DirName[81] = {0};
	strncpy(DirName, this->dirName.c_str(), 80);
	data.WriteBlock(DirName, 81, position);
	char DirStructReserve[3] = {0};
	data.WriteBlock(DirStructReserve, 3, position);

	// VMainWay
	unsigned long mainway_struct_size = 28;
	data.Write<unsigned long>(mainway_struct_size, position);
	char WayShort[4] = {0}; 
	strncpy(WayShort, this->wayName.c_str(), 3);
	data.Write<VWaySection>(this->section, position);
	data.WriteBlock(WayShort, 4, position);

	// Маска
	data.Write<unsigned long>(mask, position);

	// длинный путь
	char WayLong[32] = {0};
	strncpy(WayLong, this->wayName.c_str(),31);
	data.WriteBlock(WayLong, 32, position);

	// Хвост
	char reserve[72] = {0};
	data.WriteBlock(reserve, 72, position);

	// Километры
	data.Write<long>(prmKmEx, position);
	data.Write<long>(this->kmList.GetCount(), position);
	data.Write<long>(this->kmList.GetItem(0).km, position);
	for (size_t i = 0; i < this->kmList.GetCount(); ++i)
	{
		data.Write<double>(this->kmList.GetItem(i).base, position);
		data.Write<double>(this->kmList.GetItem(i).end, position);
		data.Write<long>(this->kmList.GetItem(i).km, position);
		data.Write<float>(this->kmList.GetItem(i).len, position);
	}

	// Скорости по приказу
	data.Write<long>(prmSpeedEx, position);
	WriteSpeeds(data, this->speedList, position);

	// Временные скорости
	data.Write<long>(prmSpeedTmpEx, position);
	WriteSpeeds(data, this->speedTmpList, position);

	// Проектные скорости
	data.Write<long>(prmSpeedProjEx, position);
	WriteSpeeds(data, this->speedProjList, position);

	// Нормы ШК
	data.Write<long>(prmNWidthEx, position);
	WriteNorma(data, this->nwidthStraightList, 1520, position);

	// Нормы ШК
	data.Write<long>(prmNLevelEx, position);
	WriteNorma(data, this->nlevelStraightList, 0, position);

	// ПЧ
	data.Write<long>(prmPchEx, position);
	data.Write<long>(this->pchList.GetCount(), position);
	for (size_t i = 0; i < this->pchList.GetCount(); ++i)
	{
		data.Write<double>(this->pchList.GetItem(i).base, position);
		data.Write<double>(this->pchList.GetItem(i).end, position);
		data.Write<long>(this->pchList.GetItem(i).pchCode, position);
		data.Write<long>(this->pchList.GetItem(i).nodCode, position);
	}

	// Типы шпал
	data.Write<long>(prmSpala, position);
	WriteIndexList(data, this->spalaList, position);

	// Стрелки
	data.Write<long>(prmSwitch, position);
	WriteSwitchList(data, this->switchList, position);

	// Мосты
	data.Write<long>(prmBridge, position);
	WriteIndexList(data, this->bridgeList, position);

	// Тоннели
	data.Write<long>(prmTonnel, position);
	WriteIndexList(data, this->tonnelList, position);

	// Точки съезда
	data.Write<long>(prmRoute, position);
	data.Write<long>(this->routeList.GetCount(), position);
	for (size_t i = 0; i < this->routeList.GetCount(); ++i)
	{
		data.Write<double>(this->routeList.GetItem(i).base, position);
		data.Write<double>(this->routeList.GetItem(i).end, position);
	}

	// Метки профиля
	data.Write<long>(prmProf, position);
	data.Write<long>(this->profList.GetCount(), position);
	for (size_t i = 0; i < this->profList.GetCount(); ++i)
	{
		data.Write<double>(this->profList.GetItem(i).coord, position);
		data.Write<float>(this->profList.GetItem(i).value, position);
	}

	// Кривые
	data.Write<long>(prmCurveEx, position);
	data.Write<long>(this->crvPassport.curveList.GetCount(), position);
	for (size_t i = 0; i < this->crvPassport.curveList.GetCount(); ++i)
	{
		data.Write<double>(this->crvPassport.curveList.GetItem(i).base, position);
		data.Write<double>(this->crvPassport.curveList.GetItem(i).end, position);
		data.Write<bool>(this->crvPassport.curveList.GetItem(i).isThere, position);
		data.Write<long>(0, position); // fake orient
		data.Write<float>(0, position); // fake angle

		data.Write<long>(this->crvPassport.curveList.GetItem(i).planLabel.size(), position);
		for (size_t k = 0; k < this->crvPassport.curveList.GetItem(i).planLabel.size(); ++k)
		{
			data.Write<double>(this->crvPassport.curveList.GetItem(i).planLabel[k].coord, position);
			data.Write<float>(this->crvPassport.curveList.GetItem(i).planLabel[k].value, position);
			data.Write<double>(this->crvPassport.curveList.GetItem(i).levelLabel[k].coord, position);
			data.Write<float>(this->crvPassport.curveList.GetItem(i).levelLabel[k].value, position);
			// fake rich-labels
			data.Write<double>(this->crvPassport.curveList.GetItem(i).planLabel[k].coord, position);
			data.Write<float>(this->crvPassport.curveList.GetItem(i).planLabel[k].value, position);
			data.Write<double>(this->crvPassport.curveList.GetItem(i).planLabel[k].coord, position);
			data.Write<float>(this->crvPassport.curveList.GetItem(i).planLabel[k].value, position);
		}

		data.Write<long>(this->crvPassport.curveList.GetItem(i).radius.size(), position);
		for (size_t k = 0; k < this->crvPassport.curveList.GetItem(i).radius.size(); ++k)
		{
			long w = (long) this->crvPassport.curveList.GetItem(i).radius[k].width;
			data.Write<long>(w, position);
		}
	}

	// Станции и перегоны
	data.Write<long>(prmPntEx, position);
	data.Write<long>(this->pntList.GetCount(), position);
	for (size_t i = 0; i < this->pntList.GetCount(); ++i)
	{
		data.Write<double>(this->pntList.GetItem(i).base, position);
		data.Write<double>(this->pntList.GetItem(i).end, position);
		data.Write<bool>(this->pntList.GetItem(i).isThere, position);
		data.Write<long>(this->pntList.GetItem(i).code, position);
		data.Write<double>(this->pntList.GetItem(i).axis, position);

		long namelen = this->pntList.GetItem(i).name.length();
		data.Write<long>(namelen, position);
		data.WriteBlock(this->pntList.GetItem(i).name.c_str(), namelen, position);
		data.WriteBlock(this->pntList.GetItem(i).nameReverse.c_str(), namelen, position);
	}

	// Плети левые
	data.Write<long>(prmWobLeftEx, position);
	WriteWob(data, this->wobLeftList, position);

	// Плети правые
	data.Write<long>(prmWobRightEx, position);
	WriteWob(data, this->wobRightList, position);

	// Категории пути
	data.Write<long>(prmWayCat, position);
	WriteIndexList(data, this->wayCatList, position);

	// Отверстия
	data.Write<long>(prmHole, position);
	WriteIndexList(data, this->holeList, position);

	// Voila!
	data.Write<long>(prmEof, position);
	data.Write<long>(0, position);

	return true;
}

// Чтение из произвольного файла (НЕ c4f!!!!)
bool VAprinfo::DumpLoadFromFile(const VFileName & fn)
{
	__int64 blockSize = fn.GetFileSize();
	if (blockSize <= 0) return false;
		
	VBasicParameter prm;
	prm.Create(0xFFFFFFFF, VCT_USER, 1, blockSize, blockSize);

	std::ifstream dump(fn.GetPath().c_str(), std::ios::binary);
	if (!dump.is_open()) return false;
	dump.read((char*) prm.GetPtr(0), prm.GetDataSize());
	dump.close();

	this->Unload();
	return this->Load(prm);
}

// Сохренение в произвольный файл (НЕ c4f!!!!)
bool VAprinfo::DumpSaveToFile(const VFileName & fn)
{
	VBasicParameter prm;
	size_t blockSize = this->CalcBlockSize();
	prm.Create(0xFFFFFFFF, VCT_USER, 1, blockSize, blockSize);
	this->Save(prm, 0xFFFFFFFF);

	std::ofstream dump(fn.GetPath().c_str(), std::ios::binary);
	if (!dump.is_open()) return false;
	dump.write((const char*) prm.GetPtr(0), prm.GetDataSize());
	dump.close();
	return true;
}
