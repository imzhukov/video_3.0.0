#pragma once

#include <set>
#include <map>
#include <ostream>

//boost includes
#include <boost/shared_ptr.hpp>

#include "parameters.h"
#include "VAprinfo.h"
#include "VParameter.h"
#include "VCorrupt.h"
#include "VCurveList.h"
#include "VOverstep.h"
#include "VProfile.h"
#include "VOpMark.h"
#include "VZazor.h"
#include "VSidewayList.h"
#include "VControlArea.h"
#include "VWear.h"
#include "VStatistics.h"
#include "VPlan.h"
#include "VPlanElemList.h"
#include "VHistory.h"
#include "VComment.h"
#include "VUklon.h"
#include "VPer.h"
#include "VPerLine.h"
#include "VCoordPoint.h"
#include "VFileName.h"
#include "VRemoteFile.h"
#include "VLineList.h"
#include "oversteps.h"
#include "VDiffWidth.h"
#include "canting.h"
#include "aui.h"
#include "VPrNgrAreas.h"
#include "rtparam.h"
#include "markers.h"

class VIBDatabase;
class VProgressBox;

/// Обработка боковых путей при сохранении в CSV
enum SIDEWAYS_PROCESSING
{
	SIDEWAYS_KEEP,		///< сохранить без изменений
	SIDEWAYS_ZERO,		///< занулить
	SIDEWAYS_REMOVE		///< удалить
};

/// Консолидированный набор данных из файла
class VDataSet
{
	VMainFile file;						///< файл параметров
	VRemoteFile remote;					///< удаленный файл параметров
	int open_mode;						///< режим, в котором открыт файл
	VFileName fn;						///< имя файла
	VMainFileHeader header;				///< заголовок файла

	VAbsCoordData absCoord;				///< абс.координата
	VCorruptData corrupt;				///< сбои измерений
	VSpeedData speed;					///< скорость вагона
	VAprinfo aprInfo, fileAprinfo;		///< текущая и минимальная априорная информация
	VComment comment;					///< комментарий к файлу

	VOvr2List ovrs2;					///< отступления версии2
	VOvr3List ovrs3;					///< отступления версии3
	VOpMarkList opMarks;				///< метки оператора
	VMagZazorList magZazor;				///< магнитные зазоры
	VZazorList zazorLeft, zazorRight;	///< оптические зазоры
	VZazorList magZazorLeft, magZazorRight; ///<магнтиные зазоры с величиной
	VUgonList ugonLeft, ugonRight;		///< угоны
	VSpalaMarkerList spalaMarker;		///< метки шпал
	VCoordPointAutoCorr coordPoint;		///< объекты пути

	// измеренные параметры, возвращаемые как VParameter
	std::map<PARAMETER_TYPE, VParameterPtr> params;

	VParameter midLevel;				///< ср. уровень 
	VParameter midCurv;					///< ср. кривизна 
	VParameter midWidth;				///< ср. ШК
	VParameter midRichLeft;				///< ср. левая рихтовка 
	VParameter midRichRight;			///< ср. правая рихтовка 

	VParameter curv_from_kurs;			///< Кривизна из Курса

	VDoubleParameter gps_time;			///< GPS: время 
	VDoubleParameter gps_latitude;		///< GPS: широта 
	VDoubleParameter gps_longitude;		///< GPS: долгота
	VDoubleParameter gps_height;		///< GPS: возвышение

	VUlongParameter dpp;				///< Счетчик импульсов DPP

	VCurveList curves, modelCurves;		///< кривые и модельные кривые
	VLineList lines;					///< прямые
	VLineMarkList lineMarks;			///< метки прямых
	VProfileMarkList profMarks;			///< метки профиля
	VSidewayList sideways;				///< боковые пути
	VControlAreaList ctrlAreas;			///< контрольные участки
	VHistory history;					///< история операций
	
	VProfile profile;					///< профиль
	VDiffProfile diff_profile;			///< отклонения от идеального профиля
	VAproxProfile aprox_profile;		///< спрямленный профиль
	VJaggyProfile jaggy_profile;		///< неровности профиля
	VSlopeList profile_slope_element;   ///< уклон профиля по элементам
	VSlopeList profile_slope_km;		///< уклон профиля по километрам
	VSlopeList profile_slope_pk;		///< уклон профиля по пикетам
	VSlopeList profile_slope_hpk;		///< уклон профиля по полупикетам

	VWear wear;							///< Объединенный износ
	VStatistics statistic;				///< параметры для расчёта статистических данных
	VPlan plan;							///< план пути	
	VIdealPlanReal ideal_plan;			///< "идеальный" план
	VIdealPlanModel model_ideal_plan;	///< модельный "идеальный" план
	VPlanElemList planElem;				///< список элементов плана
	VPlanElemList  modelPlanElem;		///< модельный список элементов плана	

	VUklonList uklon;					///< максимальные изменения уклона
	VPerKuzList perkuz;					///< перекосы на базе кузова
	VPerTelList pertel;					///< перекосы на базе тележки	
	VPerLineList perline;               ///< возвышения в прямой

	VPlanOffsetReal plan_offset_fictional;///< отклонения плана от идеального без учета разметки прямых

	VPlanOffsetReal	plan_offset;		///< отклонения плана от идеального	
	VPlanOffsetModel model_plan_offset;	///< отклонения плана от идеального	(пасп.)	
	VJaggyPlan jaggy_plan;		        ///< неровности плана
	VJaggyPlan jaggy_plan_on_lines;		///< неровности плана на прямых
	VZLine zl;							///< нулевая линия по уровню

	VAnpReal anp_real_high;				///< непогашенное ускорение фактическое для скоростных поездов
	VAnpReal anp_real_high_bt;			///< непогашенное ускорение фактическое для скоростных поездов с наклоном кузова
	VAnpReal anp_real_pass;				///< непогашенное ускорение фактическое для пассажирских поездов
	VAnpReal anp_real_truck;			///< непогашенное ускорение фактическое для грузовых поездов
	
	VAnpModel anp_model_high;			///< непогашенное ускорение модельное для скоростных поездов
	VAnpModel anp_model_high_bt;		///< непогашенное ускорение модельное для скоростных поездов с наклоном кузова
	VAnpModel anp_model_pass;			///< непогашенное ускорение модельное для пассажирских поездов
	VAnpModel anp_model_truck;			///< непогашенное ускорение модельное для грузовых поездов

	VPsiReal psi_real_high;				///< скорость нарастания непогашенного ускорения (факт. скор.)
	VPsiReal psi_real_high_bt;			///< скорость нарастания непогашенного ускорения (факт. скор. с НК)
	VPsiReal psi_real_pass;				///< скорость нарастания непогашенного ускорения (факт. пасс.)
	VPsiReal psi_real_truck;			///< скорость нарастания непогашенного ускорения (факт. груз.)
	
	VDiffLevel diff_level;				///< отклонения уровеня от разметки
	VDiffLevel diff_level_model;		///< отклонения уровеня от разметки модели

	VDiffWidth width_diff_front;				///< разность ШК с нагрузкой и без
	VDiffWidth width_diff_back;				///< разность ШК с нагрузкой и без

	VJaggyProfileOverstepList jaggy_profile_overstep; ///< Сверхнормативные неровности профиля
	VJaggyPlanOverstepList jaggy_plan_overstep; ///< Сверхнормативные неровности плана

	VDiffWidthOverstepList diff_width_front_overstep; ///< Отжатия
	VDiffWidthOverstepList diff_width_back_overstep; ///< Отжатия
	
	VCantingOverstepList canting_left_overstep; ///< Левая подуклонка
	VCantingOverstepList canting_right_overstep; ///< Правая подуклонка
	
	VExtWearOverstepList ext_wear_left_overstep; ///< Левый износ нерабочей грани
	VExtWearOverstepList ext_wear_right_overstep; ///< Правый износ нерабочей грани

	VPrNgrAreas prNgrAreas; ///< Участки ПР/НГР
	
	VWayMarkerList markers; ///< Маркеры

	std::map<PARAMETER_TYPE, bool> piList;	///< список загруженных параметров
	std::set<PARAMETER_TYPE> changed;	///< измененные параметры
	std::wstring message;				///< сообщение об ошибке

	std::map<std::wstring, std::wstring> options;///< опции

	/// параметр возвращается как VParameter
	static bool IsParameter(PARAMETER_TYPE param); 

	/// параметр загружается из файла
	static bool IsFileParameter(PARAMETER_TYPE param);

	/// параметр вычисляется на лету
	static bool IsCalcParameter(PARAMETER_TYPE param);

	/// параметр возвращается как VDoubleParameter
	static bool IsDoubleParameter(PARAMETER_TYPE param); 

	/// параметр возвращается как VZazorList
	static bool IsZazor(PARAMETER_TYPE param);

	/// Сообщение об ошибке при загрузке параметра
	std::wstring FormatErrorMessage(PARAMETER_TYPE param, const std::wstring & msg);

public:

	/// Конструктор
	VDataSet();

	/// Конструктор - с открытием локального файла
	VDataSet(const VFileName & fn);

	/// Конструктор - с открытием удаленного файла
	VDataSet(VIBDatabase & db, const VFileName & fn);

	/// Деструктор
	~VDataSet()
	{
		this->CloseFile();
	}

	/// Сообщение об ошибке
	const std::wstring & GetErrorMessage() const
	{
		return this->message;
	}

	// --------------------------------------------------------------------
	// Операции с файлом
	// --------------------------------------------------------------------

	/// Открыть локальный файл для чтения
	bool OpenFile(const VFileName & fileName);

	/// Создать новый локальный файл
	bool CreateFile(const VFileName & fileName);

	/// Открыть удаленный файл для чтения
	bool OpenFile(VIBDatabase & db, const VFileName & fileName);

	/// Добавить параметр в список измененных
	void SetChanged(PARAMETER_TYPE param)
	{
		this->changed.insert(param);
		this->piList.insert(std::pair<PARAMETER_TYPE, bool>(param, true));
	}

	/// Удалить параметр из списка удаленных
	void UnsetChanged(PARAMETER_TYPE param)
	{
		this->changed.erase(param);
	}

	/// Проверить "измененность" параметра
	bool IsChanged(PARAMETER_TYPE param) const
	{
		return this->changed.find(param) != this->changed.end();
	}

	/// Проверить были ли изменены какие-либо данные
	bool IsChanged()
	{
		return !this->changed.empty();
	}

	/// Копирование из DataSet
	void CopyFrom(VDataSet & source, const VWaySection & section, 
		VProgressBox * progress);

	/// Копирование из DataSet - границы в виде абс. координат
	void CopyFrom(VDataSet & source, double absFrom, double absTo,
		VProgressBox * progress);

	/// Сохранить изменения в файле
	bool Save(const std::wstring & fileName = L"");

	/// Сохранить как inm
	bool SaveToInm(const std::wstring & fileName, VProgressBox * progress);

	/// Получить кадр для расшифровки
	bool GetRtParameter(VRtParameter & kadr, size_t kadrNdx);

	/// Закрыть файл
	void CloseFile()
	{
		if (this->FileIsLoaded()) 
			this->file.Close();
	}

	/// Файл открыт для чтения
	/// FIXME!!!! для удаленных файлов нужна процедура проверки доступности файла
	bool FileIsLoaded()
	{
		return this->file.Loaded() || this->remote.Loaded();
	}

	bool FileIsRemote() const
	{
		return this->remote.Loaded();
	}

	/// Проверка наличия данных в файле
	bool DataExists(PARAMETER_TYPE param);

	/// Имя загруженного файла с полным путем
	const VFileName & GetFileName() const
	{
		return this->fn;
	}

	/// Имя загруженного файла
	const std::wstring GetFileShortName() const
	{
		return this->GetFileName().GetShortName();
	}

	// --------------------------------------------------------------------
	// Операции с заголовком файла
	// --------------------------------------------------------------------

	const VMainFileHeader * GetFileHeader() const
	{
		return &this->header;
	}

	/// Вернуть число кадров прописанное в заголовке
	uint32_t GetParamCount() const
	{
		return this->header.ParamCount;
	}

	// Число кадров
	void SetParamCount(uint32_t count) 
	{
		this->header.ParamCount = count;
	}

	/// Вернуть дату поездки
	const VTime & GetRailDate() const
	{
		return this->header.RailDate;
	}

	/// Установить дату поездки
	void SetRailDate(const VTime & t)
	{
		this->header.RailDate = t;
	}

	/// Вернуть номер дороги
	uint32_t GetRoadNum() const
	{
		return this->header.RoadNum;
	}

	/// Установить номер дороги
	void SetRoadNum(uint32_t rn)
	{
		this->header.RoadNum = rn;
	}

	/// Вернуть номер направления
	uint32_t GetDir() const
	{
		return this->header.Dir;
	}

	/// Вернуть код станции
	uint32_t GetStationCode() const
	{
		return this->header.StCode;
	}

	/// Вернуть имя пути
	std::string GetWay() const;

	/// Вернуть направление движения
	int8_t GetMoveDirection() const
	{
		return this->header.MoveDirection;
	}

	/// Установить направление движение
	void SetMoveDirection(int8_t md)
	{
		this->header.MoveDirection = md;
	}

	/// Вернуть подразделение
	std::string GetDivision() const
	{
		return std::string(this->header.Division);
	}

	/// Установить подразделение
	void SetDivision(const std::string & div)
	{
		strncpy_s(this->header.Division, 16, div.c_str(), 15);
		this->header.Division[15] = 0;
	}

	/// Вернуть источник данных
	std::string GetDataSource() const
	{
		return std::string(this->header.DataSource);
	}

	/// Устанвить источник данных
	void SetDataSource(const std::string & ds)
	{
		strncpy_s(this->header.DataSource, 256, ds.c_str(), 255);
		this->header.DataSource[255] = 0;
	}

	/// Вернуть версию источника данных
	uint32_t GetSourceVersion() const
	{
		return this->header.SourceVersion;
	}

	/// Установить версию источника данных
	void SetSourceVersion(uint32_t sv)
	{
		this->header.SourceVersion = sv;
	}

	/// Вернуть название перегона
	std::string GetPeregon() const
	{
		return std::string(this->header.Peregon);
	}

	/// Вернуть название парка
	std::string GetParkName() const
	{
		return std::string(this->header.Peregon);
	}

	/// Вернуть имя направления
	std::string GetDirName() const
	{
		return std::string(this->header.DirName);
	}

	/// Вернуть имя станции
	std::string GetStationName() const
	{
		return std::string(this->header.DirName);
	}

	/// Вернуть время создания файла
	const VTime & GetCreateTime() const
	{
		return this->header.CreateTime;
	}

	/// Вернуть время обновления файла
	const VTime & GetUpdateTime() const
	{
		return this->header.UpdateTime;
	}

	/// Вернуть границы файла
	const VWaySection & GetWaySection() const
	{
		return this->header.WaySection;
	}

	/// Установить границы файла
	void SetWaySection(const VWaySection & ws)
	{
		this->header.WaySection = ws;
	}

	/// Проезд по магистрали
	bool IsMagistral() const
	{
		return this->header.Dir != 0 && this->header.StCode == 0;
	}

	/// Проезд по станциям
	bool IsStation() const
	{
		return this->header.Dir == 0 && this->header.StCode != 0;
	}

	/// Широта
	float GetStartLatitude() const
	{
		return this->header.Latitude;
	}

	/// Долгота
	float GetStartLongitude() const
	{
		return this->header.Longitude;
	}

	/// Информация о магистральном проезде
	bool SetMagistralInfo(uint32_t dirCode, const std::string & way, const std::string & dirName, const std::string & peregon);

	/// Информация о станционном проезде
	bool SetStationInfo(uint32_t stCode, const std::string & stName, const std::string & park, const std::string & way);

	// --------------------------------------------------------------------
	// Операции с данными
	// --------------------------------------------------------------------

	/// Загрузка параметров
	bool Load(PARAMETER_TYPE param);

	/// Проверка загруженности параметра
	bool IsLoaded(PARAMETER_TYPE param) const;

	/// Проверка наличия блока
	bool CheckDataBlock(PARAMETER_TYPE param);

	/// Таймштамп блока данных в файле
	VTime GetDataBlockTime(PARAMETER_TYPE param);

	/// Количество записей в блоке данных в файле
	size_t GetDataBlockRecCount(PARAMETER_TYPE param);

	/// Выгрузка
	std::set<PARAMETER_TYPE> Unload(PARAMETER_TYPE param);

	/// Список доступных для записи в CSV параметров
	std::set<PARAMETER_TYPE> GetCSVParameters();

	/// Записать в поток таблицу CSV
	size_t GetCSV(std::ostream & output, 
		const std::set<PARAMETER_TYPE> params,
		size_t step_kadr, float step_m, 
		const VWaySection & section,
		SIDEWAYS_PROCESSING swp,
		VProgressBox * progress);
		
	/// Записать в поток метки оператора
	size_t GetOpMarkCSV(std::ostream & output);		

	/// Записать в поток метки кривых 
	size_t GetCurveMarkCSV(std::ostream & output, bool model = false);

	/// Записать в поток метки профиля 
	size_t GetProfMarkCSV(std::ostream & output);

	/// Записать в поток мат. ожидание и СКО
	size_t GetMoSkoCSV(std::ostream & output, int filter);

	/// Список доступных для коррекции параметров
	std::set<PARAMETER_TYPE> GetCorrectableParameters();

	/// Коррекция абс.к-ты
	void CorrectCoord(double offset_m, double expand_m_per_km);

	/// Границы поездки
	VWaySection GetSection();

	/// Количество кадров в поездке
	size_t GetKadrCount();

	/// Абсолютная координата 
	VAbsCoordData & GetAbsCoord()
	{
		return this->absCoord;
	}

	/// Сбои измерений - глазков
	VCorruptData & GetCorrupt()
	{
		return this->corrupt;
	}

	/// Скорость ВПС
	VSpeedData & GetSpeed()
	{
		return this->speed;
	}

	/// Апр. информация 
	VAprinfo & GetAprinfo()
	{
		return this->aprInfo;
	}

	/// Минимальная апринфо из файла
	const VAprinfo & GetFileAprinfo() const
	{
		return this->fileAprinfo;
	}

	/// Загрузка апр.информации из файла
	bool AprinfoLoadFromFile()
	{
		return this->aprInfo.Load(this->file, DHT_APRINFO);
	}

	/// Загрузка апр.информации из базы
	bool Makeinfo(VIBDatabasePtr base, bool repairs, bool mea_curves);

	/// Счетчик импульсов DPP
	VUlongParameter & GetDPP()
	{
		return this->dpp;
	}

	/// Отступления
	VOvr2List & GetOvrs2()
	{
		return this->ovrs2;
	}

	/// Новые отступления
	VOvr3List & GetOvrs3()
	{
		return this->ovrs3;
	}

	/// Метки оператора
	VOpMarkList & GetOpMarks()
	{
		return this->opMarks;
	}
	
	/// Магнитные зазоры
	VMagZazorList & GetMagZazors()
	{
		return this->magZazor;
	}

	/// Зазор левый
	VZazorList & GetZazorLeft()
	{
		return this->zazorLeft;
	}

	/// Зазор правый
	VZazorList & GetZazorRight()
	{
		return this->zazorRight;
	}

	// Магнитный зазор левый
	VZazorList & GetMagZazorLeft()
	{
		return this->magZazorLeft;
	}

	// Магнитный зазор правый
	VZazorList & GetMagZazorRight()
	{
		return this->magZazorRight;
	}

	// Зазоры - все в куче
	VZazorList & GetZazor(PARAMETER_TYPE param);

	/// Изменение уклона
	VUklonList & GetUklon()
	{
		return this->uklon;
	}

	/// Угон левый
	VUgonList & GetUgonLeft()
	{
		return this->ugonLeft;
	}

	/// Угон правый
	VUgonList & GetUgonRight()
	{
		return this->ugonRight;
	}

	/// Метки шпал
	VSpalaMarkerList & GetSpalaMarker()
	{
		return this->spalaMarker;
	}

	/// Разметка кривых
	VCurveList & GetCurves(PARAMETER_TYPE in_type=PRM_CURVE_MARKS)
	{
		switch (in_type)
		{
			case PRM_CURVE_MARKS:
				return this->curves;
			case PRM_MODEL_CURVES:
				return this->modelCurves;
			default:
				throw VSimpleException(L"Некорректный код параметра", boost::str(boost::wformat(L"(%d)") % static_cast<uint32_t>(in_type)), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		}
	}	

	/// Боковые пути
	VSidewayList & GetSideways()
	{
		return this->sideways;
	}

	/// прямые
	VLineList & GetLines()
	{
		return this->lines;
	}

	/// метки прямых
	VLineMarkList & GetLineMarks()
	{
		return this->lineMarks;
	}

	/// Метки профиля
	VProfileMarkList & GetProfMarks()
	{
		return this->profMarks;
	}

	/// Отклонения от профиля
	VDiffProfile & GetDiffProfile()
	{
		return this->diff_profile;
	}

	/// Идеальный профиль
	VAproxProfile & GetAproxProfile()
	{
		return this->aprox_profile;
	}

	/// Уклоны
	VSlopeList & GetProfileSlope(PARAMETER_TYPE in_data_type)
	{
		switch (in_data_type)
		{
			default:
			case PRM_PROFILE_SLOPE_ELEMENT:			
				return this->profile_slope_element;
			case PRM_PROFILE_SLOPE_KM:
				return this->profile_slope_km;
			case PRM_PROFILE_SLOPE_PK:
				return this->profile_slope_pk;
			case PRM_PROFILE_SLOPE_HPK:
				return this->profile_slope_hpk;
		}
	}

	//Перекосы на базе кузова
	VPerKuzList & GetPerKuz ()
	{
		return this->perkuz;
	}

	//Перекосы на базе тележки
	VPerTelList & GetPerTel ()
	{
		return this->pertel;
	}

	VPerLineList & GetPerLine ()
	{
		return this->perline;
	}

	// TODO: прямые, боковые пути, объединенный износ, история

	/// Параметр по индексу
	VParameter & GetParameter(PARAMETER_TYPE param);

	/// Параметр из строки с разбором
	void ParseParameter(wstring in_command, VParameter & parameter);

	/// Параметр double по индексу
	VDoubleParameter & GetDoubleParameter(PARAMETER_TYPE param);

	/// Возвращает ссылку на статистику
	VStatistics & GetStatistics()
	{
		return this->statistic;
	}

	/// Возвращает ссылку на план
	VPlan & GetPlan()
	{
		return this->plan;
	}	

	/// Возвращает ссылку на идеальный план
	VIdealPlan & GetIdealPlan(PARAMETER_TYPE in_type=PRM_IDEAL_PLAN)
	{		
		switch (in_type)
		{
			case PRM_IDEAL_PLAN:
				return this->ideal_plan;
			case PRM_MODEL_IDEAL_PLAN:
				return this->model_ideal_plan;
			default:
				throw VSimpleException(L"Некорректный код параметра", boost::str(boost::wformat(L"(%d)") % static_cast<uint32_t>(in_type)), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		}
	}

	/// Отклонения плана от идеального
	VPlanOffset & GetPlanOffset(PARAMETER_TYPE in_type=PRM_PLAN_OFFSET)
	{	
		switch (in_type)
		{
			case PRM_PLAN_OFFSET:
				return this->plan_offset;
			case PRM_MODEL_PLAN_OFFSET:
				return this->model_plan_offset;
			default:
				throw VSimpleException(L"Некорректный код параметра", boost::str(boost::wformat(L"(%d)") % static_cast<uint32_t>(in_type)), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		}
	}	
	
	/// Непогашенное ускорение фактическое для скоростных поездов
	VAnpReal & GetAnpHighPass()
	{
		return this->anp_real_high;		
	}

	/// Непогашенное ускорение фактическое для скоростных поездов с наклоном кузова
	VAnpReal & GetAnpHighBTTruck()
	{
		return this->anp_real_high_bt;		
	}

	/// Непогашенное ускорение фактическое для пассажирских поездов
	VAnpReal & GetAnpRealPass()
	{
		return this->anp_real_pass;		
	}

	/// Непогашенное ускорение фактическое для грузовых поездов
	VAnpReal & GetAnpRealTruck()
	{
		return this->anp_real_truck;		
	}
	
	/// Непогашенное ускорение модельное для скоростных поездов
	VAnpModel & GetAnpModelHigh()
	{
		return this->anp_model_high;
	}

	/// Непогашенное ускорение модельное для скоростных поездов с наклоном кузова
	VAnpModel & GetAnpModelHighBT()
	{
		return this->anp_model_high_bt;
	}

	/// Непогашенное ускорение модельное для пассажирских поездов
	VAnpModel & GetAnpModelPass()
	{
		return this->anp_model_pass;
	}

	/// Непогашенное ускорение модельное для грузовых поездов
	VAnpModel & GetAnpModelTruck()
	{
		return this->anp_model_truck;
	}

	/// Скорость нарастания непогашенного ускорения (факт. пасс.)
	VPsiReal & GetPsiRealPass()
	{
		return this->psi_real_pass;
	}

	/// Скорость нарастания непогашенного ускорения (факт. груз.)
	VPsiReal & GetPsiRealTruck()
	{
		return this->psi_real_truck;
	}

	/// Список контрольных участков
	VControlAreaList & GetControlAreas()
	{
		return this->ctrlAreas;
	}

	/// История операция
	VHistory & GetHistory()
	{
		return this->history;
	}

	/// Список элементов плана
	VPlanElemList & GetPlanElemList(PARAMETER_TYPE in_type=PRM_PLAN_ELEM_LIST)
	{		
		switch (in_type)
		{
			case PRM_PLAN_ELEM_LIST:
				return this->planElem;
			case PRM_MODEL_PLAN_ELEM_LIST:
				return this->modelPlanElem;
			default:
				throw VSimpleException(L"Некорректный код параметра", boost::str(boost::wformat(L"(%d)") % static_cast<uint32_t>(in_type)), boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		}
	}

	/// Модельный список элементов плана
	VPlanElemList & GetModelPlanElemList()
	{
		return this->modelPlanElem;
	}

	/// Комментарий к файлу
	VComment & GetComment()
	{
		return this->comment;
	}

	/// Объекты пути
	VCoordPointAutoCorr & GetCoordPoint()
	{
		return this->coordPoint;
	}
	

	/// Сверхнормативные вертикальные неровности 
	VJaggyProfileOverstepList& GetJaggyProfileOversteps()
	{
		return this->jaggy_profile_overstep;
	}

	/// Сверхнормативные горизонтальные неровности 
	VJaggyPlanOverstepList& GetJaggyPlanOversteps()
	{
		return this->jaggy_plan_overstep;
	}

	/// Отжатия
	VDiffWidthOverstepList& GetDiffWidthFrontOversteps()
	{
		return this->diff_width_front_overstep;
	}

	/// Отжатия
	VDiffWidthOverstepList& GetDiffWidthBackOversteps()
	{
		return this->diff_width_back_overstep;
	}
	
	/// Правая подуклонка
	VCantingOverstepList& GetCantingOversteps(side_t in_side)
	{
		return in_side==SD_LEFT ? this->canting_left_overstep : this->canting_right_overstep;
	}
	
	/// Правая подуклонка
	VExtWearOverstepList& GetExtWearOversteps(side_t in_side)
	{
		return in_side==SD_LEFT ? this->ext_wear_left_overstep : this->ext_wear_right_overstep;
	}

	/// Участки ПР/НГР
	VPrNgrAreas & GetPrNgrAreas()
	{
		return this->prNgrAreas;
	}

	//FixMe!!! не факт, что этим функциям место внутри класса, но они воздействуют не только на один параметр
	//Изменение разметки профиля требует обновления отклонений профиля и поэлементных уклонов (когда они будут включены в VDataSet)
	//Это можно перенести в провайдер, если метод VDataSet::Load не будет загружать данные необходимые для, например, 
	//отклоненеий профиля, при запросе собственно самих отклонений. То есть в метод Load нужно добавить еще один
	//параметр bool in_requested_only, если параметр установлен в true, то загружается только
	//требуемый параметр, если - false, то загружаются также и необходимые для него параметры.	

	///Автоматически размечает профиль 
	void ProfileAutomark(VAUIProgressDialogPtr in_progress);	
	/// Перенос профиля из БД
	void ProfileFromDB(VIBDatabasePtr in_database, VAUIProgressDialogPtr in_progress);
	///Привязывает метки к профилю
	void AdjustProfileMarkHeight();
	/// Возвращает true, если в кадре можно ставить метку профиля
	bool AllowMarkHereProfile(size_t in_index, double in_min_length=200);
	/// Возвращает true, если в кадре можно передвинуть метку профиля
	bool AllowMarkHereProfile(size_t in_index, size_t in_mark_index, double in_min_length=200);	
	/// Обновление отклонений от идеального плана в окрестности точки
	void UpdatePlanOffset(size_t in_index, bool in_model, bool in_fast=false);
	/// Обновление отклонений от идеального плана
	void UpdatePlanOffset(bool in_model=false);
	/// Обновление модельного непогашенного ускорения
	void UpdateAnpModel();
	/// Удаление кривых
	void DeleteCurves(PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/// Удаление прямых
	void DeleteLines(PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/// Авторазметка кривой
	void AutomarkCurve(size_t in_start, size_t in_end, PARAMETER_TYPE in_mark_type);
	/// Возвращает маркеры (эксперимент)
	const VWayMarkerList& GetMarkers();
	/// Возвращает true, если в файле есть кривые
	bool HasCurves();
	/// Поиск и первоначальная разметка кривых
	void SearchCurves(VAUIProgressDialogPtr in_progress, PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/// Копирование кривых
	void CopyCurves(PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/// Перенос кривых из БД (априорные кривые)
	void TransferPassportCurves(VIBDatabasePtr in_database, VAUIProgressDialogPtr in_progress, PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/// Перенос кривых из БД (кривые из предыдущих проездов)
	void TransferMeasuredCurves(VIBDatabasePtr in_database, VAUIProgressDialogPtr in_progress, PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/// Оптимизация кривых
	void OptimizeCurves(VAUIProgressDialogPtr in_progress, PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/// Возвращает true, если кривая не пересекает метку перелома плана
	bool CheckCurveVsLine(size_t in_index, const VCurve& in_curve, PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/// Возвращает true если допустимо объявить на участке съезд.
	bool AllowSidewayHere(size_t in_index_start, size_t in_index_end, PARAMETER_TYPE in_mark_type);
	/// Возвращает true, если в кадре можно ставить метку
	bool AllowMarkHerePlan(size_t in_index, PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/// Возвращает true, если в кадре можно ставить метку
	bool AllowMarkHereLevel(size_t in_index, PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	///Автокоррекция координаты по километровым столбам
	void Autocorrection(VIBDatabase * database, POINT_TYPE type);
	///Поиск отклонений между файлами
	double CalcDelta(VDataSet & dataset, PARAMETER_TYPE param);
	///Коэффициент корреляции двух параметров (Пирсона)
	double GetCorrelation(PARAMETER_TYPE in_param1, PARAMETER_TYPE in_param2);
	///Коэффициент корреляции двух параметров (Фехнера)
	double GetSignCorrelation(PARAMETER_TYPE in_param1, PARAMETER_TYPE in_param2);
	///Возвращает true, если в кадре можно ставить метку перелома плана
	bool AllowMarkHereLine(size_t in_index, double in_min_length);
	///Обновляет отклонения уровня от разметки в окрестности точки
	void UpdateDiffLevel(bool in_is_model);

	// Функция подвижки магнитных зазоров
	void ShiftMagZazor(int value);	

	/// Вычисление путевой координаты по коду ДПП
	VWayCoord GetCoordByDPP(unsigned long in_dpp, float correction);

};
typedef boost::shared_ptr<VDataSet> VDataSetPtr;

std::wstring GetPrefix(const VAprinfo & aprInfo, time_t t);
