#pragma once

#include <string>
#include <wx/string.h>

#include "parameters.h"
#include "VParameter.h"
#include "VAprinfoRecord.h"
#include "VRemoteFile.h"
#include "markers.h"

/// Интерфейс БД
class VIBDatabase;

/// Обработчик данных из БД
class VIBDataHandlerParameter;

/// Участки с одним параметром
template <class T> class VAprinfoParameter
{
	std::vector<T> data;	
	bool isDefault;

public:

	// статические функции для проверки попадания записи в экспортируемый список

	/// Любая запись
	static bool FitsAlways(const T &)
	{
		return true;
	}

	/// ObjectRecord и потомки - выставлен флаг isThere
	static bool FitsObject(const T & object)
	{
		return object.isThere;
	}

	/// IndexRecord - объекты с охранной зоной
	static bool FitsIndex(const T & object)
	{
		return object.value == SZO_OBJECT;
	}

	/// IndexRecord - объекты с охранной зоной
	static bool FitsIndexAndZone(const T & object)
	{
		return object.value == SZO_OBJECT || object.value == SZO_SZONE;
	}

	/// KmRecord - километры с ненулевой длиной
	static bool FitsNonZeroKm(const T & object)
	{
		return object.len != 0;
	}
	
	/// валидные скорости
	static bool FitsValidSpeed(const T & object)
	{
		return 
			object.spd.GetPassSpeed().Valid() &&
			object.spd.GetTruckSpeed().Valid() &&
			object.spd.GetEmptySpeed().Valid();
	}

	/// конструктор
	VAprinfoParameter() : isDefault(false)
	{}

	/// Установить флаг "заполнено значениями по умолчанию
	void SetDefault(bool __def__)
	{
		this->isDefault = __def__;
	}

	/// Проверить флаг "заполнено значениями по умолчанию
	bool GetDefault() const
	{
		return this->isDefault;
	}

	/// зарезервировать память
	void Reserve(size_t limit)
	{
		this->data.reserve(limit);
	}

	/// добавить элемент
	void Add(const T & src)
	{
		this->data.push_back(src);
	}

	/// Добавить список элементов
	void Add(const std::vector<T> & src)
	{
		for (size_t i = 0; i < src.size(); ++i)
			this->Add(src[i]);
	}

	/// выровнять границы записей по указанным рамкам
	void SyncRanges(double begin, double end);

	/// выровнять границы, заполнив промежутки умолчаниями
	void SyncRanges(double begin, double end, long def_value);

	/// выровнять границы, заполнив промежутки умолчаниями (для участков действия)
	/// FIXME! это криво - надо делать через шаблон.
	void SyncRanges(double begin, double end, long def_width, long def_level);

	/// выровнять границы, заполнив промежутки умолчаниями
	void SyncRanges(double begin, double end, std::string def_value);

	/// выровнять границы, заполнив промежутки умолчаниями (для скоростей)
	/// FIXME! это криво - надо делать через шаблон.
	void SyncSpdRanges(double begin, double end, const VSpeeds & def_spd);

	/// выровнять границы скоростей, вернуть true для предупреждения
	/// о дырках
	bool SyncSpdRanges(double begin, double end);

	/// выровнять границы стрелок
	void SyncSwitchRanges(double begin, double end);

	/// выровнять границы плетей
	void SyncWobRanges(double begin, double end);

	/// вернуть число элементов
	size_t GetCount() const
	{
		return this->data.size();
	}

	/// константная ссылка на index-ный элемент
	const T & GetItem(size_t index) const
	{
		return this->data.at(index); 		
	}

	/// константная ссылка на последний элемент
	const T & GetLastItem() const
	{
		if (data.empty())
			throw VSimpleException(L"Список пуст", L"", boost::str(boost::wformat(L"%s") % __FILE__), __LINE__);
		return this->data.back();
	}

	/// ссылка на index-ный элемент
	T & GetItem(size_t index) 
	{
		return this->data.at(index);		
	}

	/// ссылка на последний элемент
	T & GetLastItem() 
	{
		return this->data.back();
	}

	/// индекс элемента, в границах которого лежит точке coord
	size_t FindItem(double coord) const;

	/// индекс километра 
	size_t FindKm(long km) const;

	/// экспорт данных (отрезки)
	std::vector<T> ExportAreas(double from, double to, 
		bool (*Fits)(const T &), bool cut_ranges_to_section) const;

	/// экспорт данных (метки)
	std::vector<T> ExportLabels(double from, double to) const;

	/// Сортировка
	void Sort()
	{
		std::sort(this->data.begin(), this->data.end());
	}

	/// выгрузить данные
	void Unload()
	{
		this->data.clear();
	}
};

/// Априорная информация
class VAprinfo
{
	bool isLoaded; ///< признак загруженности параметра
	std::wstring message; ///< сообщение об ошибке
	bool warning;

	long dirCode; ///< Код направления
	std::string dirName; ///< Имя направления
	std::string wayName; ///< Имя пути
	VWaySection section; ///< Границы пути
	bool fromDB;	/// источник данных
	double absCoordBase, absCoordEnd;	/// Границы в абс.координатах

	// адм.структура
	VAprinfoParameter<VAprinfoPchRecord> pchList; ///< участки дистанций
	VAprinfoParameter<VAprinfoKmRecord> kmList; ///< километровая сетка
	VAprinfoParameter<VAprinfoIndexRecord> wayCatList; ///< категории пути
	VAprinfoParameter<VAprinfoPntRecord> pntList; ///< станции и перегоны
	VAprinfoParameter<VAprinfoIndexRecord> bridgeList; ///< мосты
	VAprinfoParameter<VAprinfoIndexRecord> tonnelList; ///< тоннели
	VAprinfoParameter<VAprinfoSwitchRecord> switchList; ///< стрелки

	// скорости
	VAprinfoParameter<VAprinfoSpeedRecord> speedList; ///< скорости по приказу
	VAprinfoParameter<VAprinfoSpeedRecord> speedTmpList; ///< временные скорости
	VAprinfoParameter<VAprinfoSpeedRecord> speedProjList; ///< проектные скорости

	// нормативы
	VAprinfoParameter<VAprinfoIndexRecord> nwidthStraightList; ///< нормы ШК на прямых
	VAprinfoParameter<VAprinfoIndexRecord> nlevelStraightList; ///< нормы возвышения на прямых
	VAprinfoParameter<VAprinfoIndexRecord> spalaList; ///< типы шпал
	VAprinfoParameter<VAprinfoIndexRecord> holeList; ///< отверстия в рельсах
	VAprinfoParameter<VAprinfoIndexRecord> threadStraightList;	///< Рихт.нити на прямых

	// прочие
	VAprinfoParameter<VAprinfoBasicRecord> routeList; ///< участки маршрута
	VAprinfoParameter<VAprinfoWobRecord> wobLeftList;	///< плети бесстыкового пути - левые
	VAprinfoParameter<VAprinfoWobRecord> wobRightList;	///< плети бесстыкового пути - правые
	VAprinfoParameter<VAprinfoProfLabel> profList; ///< метки профиля
	VAprinfoParameter<VAprinfoCoordPointRecord> coordPointList; //< реперные точки
	VAprinfoParameter<VAprinfoControlAreaRecord> controlAreaList; //< контрольные участки
	VAprinfoParameter<VAprinfoRepairRecord> repairList; //< ремонты
	VAprinfoParameter<VAprinfoCrossing> crossingList; ///< переезды
	VAprinfoParameter<VAprinfoObjectRecord> chamJointList; ///< уравнительные приборы
	VAprinfoParameter<VAprinfoTestrunRecord> testrunList; //< участки обкатки пути

	// кривые и нулевые линии
	struct CurvePack
	{
		VAprinfoParameter<VAprinfoCurve> curveList; ///< кривые
		VAprinfoParameter<VAprinfoZLRecord> zlLevelList; ///< нулевая линия на уровне
		VAprinfoParameter<VAprinfoZLRecord> zlPlanList; ///< нулевая линия на плане (кривизне)
		VAprinfoParameter<VAprinfoZLRecord> zlRichList; ///< нулевая линия на плане (рихтовке)
		VAprinfoParameter<VAprinfoWidthRecord> widthList; ///< нормы шк+износ
		VAprinfoParameter<VAprinfoIndexRecord> threadList; ///< рихтовочная нить

		/// Построение нулевых линий
		std::vector<std::wstring> BuildZL(double absStart, double absEnd); 

		/// Выгрузка данных по кривым
		void Unload();

		// Значения по умолчанию
		void SetDefaults(double absStart, double absEnd);

		VAprinfo * aprInfo;
	}
	crvFact, crvPassport;

	/// Точка предупреждения о съезде - граница участка
	VAprinfoParameter<VAprinfoIndexRecord> chrouteList; 

	/// Тип рельсов
	VAprinfoParameter<VAprinfoRailRecord> railsType;
	
	/// Тип скреплений
	VAprinfoParameter<VAprinfoBraceRecord> railsBrace;
	
	/// Путевые маркеры
	VWayMarkerList markers;

	/// Размер блока данных, необходимого для записи апринфо
	uint32_t CalcBlockSize() const;

	/// Чтение из блока
	bool Load(const VBasicParameter & data);

public:

	/// Конструктор
	VAprinfo() : isLoaded(false), warning(false), 
		message(ERRMSG_DATA_NOT_LOADED), dirCode(0), fromDB(false)
	{
		ZeroMemory(&this->section, sizeof(this->section));
		this->crvFact.aprInfo = this;
		this->crvPassport.aprInfo = this;
	}

	/// Сообщение об ошибке
	const std::wstring & GetErrorMessage() const
	{
		return this->message;
	}

	// ------------------------------------------------------------------
	// Загрузка и выгрузка данных
	// ------------------------------------------------------------------

	/// Загрузка
	template <class FILETYPE> bool Load(FILETYPE & file, uint32_t type);

	/// Загрузка из БД информации для магистралей
	bool Makeinfo(VIBDatabase & db, 
		long dirCode, const std::string & wayName, const VWaySection & section, 
		const VTime & rideDate, bool repairs, bool mea_curves);

	bool GetWarning() const
	{
		return this->warning;
	}

	/// Подмена измеренных кривых на внешние
	bool ReplaceMeasuredCurves(const std::vector<VAprinfoCurve> & curves);

	/// Обновление из БД инфорации для станций - профиль
	bool UpdateStTrailInfo(VIBDatabase & db, const std::wstring & fileName);

	/// Загрузка из БД информации для станций - по пути
	bool MakeStWayInfo(VIBDatabase & db, 
		int stCode, const std::string & parkName, const std::string & wayName,
		const std::wstring & fileName);

	/// Загрузка из БД информации для станций - по пути - со списком нестандартных км 
	bool MakeStWayInfo(VIBDatabase & db, 
		int stCode, const std::string & parkName, const std::string & wayName, 
		const std::vector<VAprinfoKmRecord> & nstKm, ///< интересует только номер и длина
		const std::wstring & fileName);

	/// Станционное APRINFO "по умолчанию"
	bool MakeStWayInfo(double begin, double end, int startKm = 0);

	/// Данные загружены
	bool IsLoaded() const
	{
		return this->isLoaded;
	}

	/// Установить флаг загрузки
	void SetLoaded(bool value)
	{
		this->isLoaded = value;
	}

	/// Сохранение в VBasicParameter
	bool Save(VBasicParameter & prm, uint32_t type);

	/// Сохранение в файл
	bool Save(VMainFile & file, uint32_t type);

	/// Выгрузка данных
	void Unload();

	/// Заполнение незагруженных данных умолчаниями
	void SetDefaults();

	/// Отчет о содержании апринфо
	std::string GetReport(bool addHtmlTag, bool fullReport) const;

	/// Источник данных
	bool IsFromDB() const
	{
		return this->fromDB;
	}

	/// Чтение из произвольного файла (НЕ c4f!!!!)
	bool DumpLoadFromFile(const VFileName & fn);

	/// Сохренение в произвольный файл (НЕ c4f!!!!)
	bool DumpSaveToFile(const VFileName & fn);

	//------------------------------------------------------------------
	// Направление и путь
	//------------------------------------------------------------------

	///Возвращает код направления
	long GetDirCode() const
	{
		return dirCode;
	};

	///Возвращает номер пути
	std::string GetWayCode() const
	{
		return wayName;
	};

	///Возвращает имя направления
	std::string GetDirName() const
	{
		return this->dirName;
	};

	// ------------------------------------------------------------------
	// Километры
	// ------------------------------------------------------------------
	
	/// Абс. к-та начала участка пути
	double GetAbsCoordBase() const 
	{ 
		return this->absCoordBase;
	}

	/// Абс. к-та конца участка пути
	double GetAbsCoordEnd() const
	{ 
		return this->absCoordEnd;
	}

	/// Точка лежит на пути
	bool FitsWay(double coord) const
	{
		return coord >= this->GetAbsCoordBase() && coord <= this->GetAbsCoordEnd();
	}

	/// Первый километр на участке пути
	long GetFirstKm() const
	{
		return this->kmList.GetItem(0).km;
	}

	/// Число километров на участке пути
	size_t GetKmCount() const
	{
		return this->kmList.GetCount();
	}

	/// направление изменения километровой сетки
	long GetKmDirection() const;

	/// Проверка наличия километра на участке пути
	bool CheckKm(long km) const;

	/// Поиск километра
	size_t FindKm(long km) const;

	/// Вернуть запись о километре - включая лежащие за пределами км сетки
	VAprinfoKmRecord GetKm(long km) const;

	/// Длина километра
	float GetKmLength(long km) const;

	/// Список километров на участке
	std::vector<VAprinfoKmRecord> GetKmList(double from, double to, bool cut_ranges_to_section) const;

	/// Список километров на пути
	std::vector<VAprinfoKmRecord> GetKmList() const;

	/// Преобразование путевой к-ты в абсолютную
	double GetAbsCoord(const VWayCoord & in_coord) const;

	/// Преобразование абсолютной к-ты в путевую
	VWayCoord GetRealCoord(double coord) const;

	/// Список пикетов на километре
	std::vector<VPicket> GetPkList(long km) const;

	/// Список пикетов на отрезке
	std::vector<VPicket> GetPkList(double from, double to, bool cut_ranges_to_section) const;

	/// ПК на координате
	VPicket GetPk(double coord) const;

	// ------------------------------------------------------------------
	// Съезд
	// ------------------------------------------------------------------

	/// Установка точки предупреждения о съезде
	void SetChrouteWarningPoint(double coord);

	/// Индекс участка съезда
	/// По разные стороны от точки warning будут разные значения
	long GetChrouteIndexValue(double coord) const;

	// ------------------------------------------------------------------
	// Нормы ШК на прямых
	// ------------------------------------------------------------------

	/// Список норм ШК на отрезке
	std::vector<VAprinfoIndexRecord> GetStraightNormaWidthList(double from, double to, bool cut_ranges_to_section = true) const;

	/// Список норм ШК на пути
	std::vector<VAprinfoIndexRecord> GetStraightNormaWidthList() const;

	/// Норма ШК на прямых в точке 
	long GetStraightNormaWidth(double coord) const;

	// ------------------------------------------------------------------
	// Нормы возвышения
	// ------------------------------------------------------------------

	/// Список норм возвышения на отрезке
	std::vector<VAprinfoIndexRecord> GetStraightNormaLevelList(double from, double to, bool cut_ranges_to_section = true) const;

	/// Список норм возвышения на пути
	std::vector<VAprinfoIndexRecord> GetStraightNormaLevelList() const;

	/// Норма возвышения на прямой в точке
	long GetStraightNormaLevel(double coord) const;

	// ------------------------------------------------------------------
	// ШК и возвышения на контрольных участках
	// ------------------------------------------------------------------

	bool IsControlArea(double coord) const;
	float GetControlAreaWidth(double coord) const;
	float GetControlAreaWidthSKO(double coord) const;
	float GetControlAreaLevel(double coord) const;
	float GetControlAreaLevelSKO(double coord) const;

	/// ШК и Возвышение на контрольных участках на всё пути
	std::vector<VAprinfoControlAreaRecord> GetControlArea() const;

	// ------------------------------------------------------------------
	// Плети
	// ------------------------------------------------------------------

	/// Список плетей бесстыкового пути на отрезке (левых)
	std::vector<VAprinfoWobRecord> GetWobLeftList(double from, double to) const;

	/// Список плетей бесстыкового пути на пути (левых)
	std::vector<VAprinfoWobRecord> GetWobLeftList() const;

	/// Список плетей бесстыкового пути на отрезке (правых)
	std::vector<VAprinfoWobRecord> GetWobRightList(double from, double to) const;

	/// Список плетей бесстыкового пути на пути (правых)
	std::vector<VAprinfoWobRecord> GetWobRightList() const;

	/// Наличие плети на левой нити в точке
	bool GetWobLeft(double coord) const;

	/// Номер плети на левой нити в точке
	std::string GetWobLeftNum(double coord) const;

	/// Наличие плети на правой нити в точке
	bool GetWobRight(double coord) const;

	/// Номер плети на правой нити в точке
	std::string GetWobRightNum(double coord) const;


	// ------------------------------------------------------------------
	// Станции и перегоны
	// ------------------------------------------------------------------

	/// Список станций и перегонов на отрезке
	std::vector<VAprinfoPntRecord> GetPntList(double from, double to) const;

	/// Список станций и перегонов на пути
	std::vector<VAprinfoPntRecord> GetPntList() const;

	/// Список станций на отрезке
	std::vector<VAprinfoPntRecord> GetStationList(double from, double to) const;

	/// Список станций на пути
	std::vector<VAprinfoPntRecord> GetStationList() const;

	/// Имя ПЧ в точке
	std::string GetPchName(double coord) const;

	/// Название перегона
	std::string GetPeregon(double coord, int direction = 1) const;

	/// Название перегона на участке
	std::string GetAreaPeregon(double begin, double end) const;

	/// Код станции (для перегонов будет 0)
	long GetStationCode(double coord) const;

	// ------------------------------------------------------------------
	// Паспортные кривые и нормы с их учетом
	// ------------------------------------------------------------------

	/// Список кривых на отрезке
	std::vector<VAprinfoCurve> GetPassportCurveList(double from, double to) const;

	/// Список кривых на пути
	std::vector<VAprinfoCurve> GetPassportCurveList() const;

	/// Текущий радиус (если радиуса нет, будет exception)
	const VAprinfoCrvRadius & GetPassportCurveRadiusRecord(double coord) const;

	/// Ориентация кривой в точке
	long GetPassportCurveOrientation(double coord) const;

	/// Статус текущего участка - на плане
	ZLINE_TYPE GetPassportCurveStatusPlan(double coord) const;

	/// Наличие кривой в текущей точке - на плане
	bool IsPassportCurvePlan(double coord) const;

	/// Статус текущего участка - на уровне
	ZLINE_TYPE GetPassportCurveStatusLevel(double coord) const;

	/// Наличие кривой в текущей точке - на уровне
	bool IsPassportCurveLevel(double coord) const;

	/// id кривой в точке
	__int64 GetPassportCurveId(double coord) const;

	/// Радиус
	float GetPassportCurveRadius(double coord) const;

	/// Возвышение с учетом кривых
	float GetPassportCurveNormaLevel(double coord) const;

	/// Кривизна с учетом кривых
	float GetPassportCurveNormaCurve(double coord) const;

	/// Рихтовка с учетом кривых
	float GetPassportCurveNormaRich(double coord) const;

	/// Норма ШК для уширений
	float GetPassportCurveNormaWidthPlus(double coord) const;

	/// Норма ШК для сужений
	float GetPassportCurveNormaWidthMinus(double coord) const;

	/// Рихтовочная нить
	long GetPassportCurveThreadSide(double coord) const;

	/// Список нулевых линий на плане (кривизне)
	std::vector<VAprinfoZLRecord> GetPassportCurvePlanZLList(double from, double to) const;

	/// Список нулевых линий на плане (кривизне)
	std::vector<VAprinfoZLRecord> GetPassportCurvePlanZLList() const;

	/// Список нулевых линий на плане (рихтовке)
	std::vector<VAprinfoZLRecord> GetPassportCurveRichZLList(double from, double to) const;

	/// Список нулевых линий на плане (рихтовке)
	std::vector<VAprinfoZLRecord> GetPassportCurveRichZLList() const;

	/// Список нулевых линий на уровне, на отрезке
	std::vector<VAprinfoZLRecord> GetPassportCurveLevelZLList(double from, double to) const;

	/// Список нулевых линий на уровне, на пути
	std::vector<VAprinfoZLRecord> GetPassportCurveLevelZLList() const;

	/// Список норм ШК и износа на отрезке
	std::vector<VAprinfoWidthRecord> GetPassportCurveWidthList(double from, double to, bool cut_ranges_to_section = true) const;

	/// Список норм ШК и износа на пути
	std::vector<VAprinfoWidthRecord> GetPassportCurveWidthList() const;

	/// Список рихтовочных нитей с учетом кривых на отрезке
	std::vector<VAprinfoIndexRecord> GetPassportCurveStraightList(double from, double to, bool cut_ranges_to_section = true) const;

	/// Список рихтовочных нитей с учетом кривых на пути
	std::vector<VAprinfoIndexRecord> GetPassportCurveStraightList() const;

	/// Возвращает минимальные из временных и установленных скоростей с учетом кривых
	VSpeeds GetPassportCurveSpeeds(double coord) const;

	/// Норма ШК в точке 
	float GetPassportCurveNormaWidth(double coord) const;

	/// Норма износа в точке
	float GetPassportCurveNormaWear(double coord) const;

	/// Суммарная рихтовочная нить с учетом паспортных кривых
	long GetPassportCurveSummaryThreadSide(double coord) const;

	// ------------------------------------------------------------------
	// Фактические кривые и нормы с их учетом
	// ------------------------------------------------------------------

	/// Список кривых на отрезке
	std::vector<VAprinfoCurve> GetFactCurveList(double from, double to) const;

	/// Список кривых на пути
	std::vector<VAprinfoCurve> GetFactCurveList() const;

	/// Текущий радиус (если радиуса нет, будет exception)
	const VAprinfoCrvRadius & GetFactCurveRadiusRecord(double coord) const;

	/// Ориентация кривой в точке
	long GetFactCurveOrientation(double coord) const;

	/// Статус текущего участка - на плане
	ZLINE_TYPE GetFactCurveStatusPlan(double coord) const;

	/// Наличие кривой в текущей точке - на плане
	bool IsFactCurvePlan(double coord) const;

	/// Статус текущего участка - на уровне
	ZLINE_TYPE GetFactCurveStatusLevel(double coord) const;

	/// Наличие кривой в текущей точке - на уровне
	bool IsFactCurveLevel(double coord) const;

	/// id кривой в точке
	__int64 GetFactCurveId(double coord) const;

	/// Радиус
	float GetFactCurveRadius(double coord) const;

	/// Возвышение с учетом кривых
	float GetFactCurveNormaLevel(double coord) const;

	/// Кривизна с учетом кривых
	float GetFactCurveNormaCurve(double coord) const;

	/// Рихтовка с учетом кривых
	float GetFactCurveNormaRich(double coord) const;

	/// Норма ШК для уширений
	float GetFactCurveNormaWidthPlus(double coord) const;

	/// Норма ШК для сужений
	float GetFactCurveNormaWidthMinus(double coord) const;

	/// Рихтовочная нить
	long GetFactCurveThreadSide(double coord) const;

	/// Список нулевых линий на плане (кривизне)
	std::vector<VAprinfoZLRecord> GetFactCurvePlanZLList(double from, double to) const;

	/// Список нулевых линий на плане (кривизне)
	std::vector<VAprinfoZLRecord> GetFactCurvePlanZLList() const;

	/// Список нулевых линий на плане (рихтовке)
	std::vector<VAprinfoZLRecord> GetFactCurveRichZLList(double from, double to) const;

	/// Список нулевых линий на плане (рихтовке)
	std::vector<VAprinfoZLRecord> GetFactCurveRichZLList() const;

	/// Список нулевых линий на уровне, на отрезке
	std::vector<VAprinfoZLRecord> GetFactCurveLevelZLList(double from, double to) const;

	/// Список нулевых линий на уровне, на пути
	std::vector<VAprinfoZLRecord> GetFactCurveLevelZLList() const;

	/// Список норм ШК и износа на отрезке
	std::vector<VAprinfoWidthRecord> GetFactCurveWidthList(double from, double to, bool cut_ranges_to_section = true) const;

	/// Список норм ШК и износа на пути
	std::vector<VAprinfoWidthRecord> GetFactCurveWidthList() const;

	/// Список рихтовочных нитей с учетом кривых на отрезке
	std::vector<VAprinfoIndexRecord> GetFactCurveStraightList(double from, double to, bool cut_ranges_to_section = true) const;

	/// Список рихтовочных нитей с учетом кривых на пути
	std::vector<VAprinfoIndexRecord> GetFactCurveStraightList() const;

	/// Возвращает минимальные из временных и установленных скоростей с учетом кривых
	VSpeeds GetFactCurveSpeeds(double coord) const;

	/// Норма ШК в точке 
	float GetFactCurveNormaWidth(double coord) const;

	/// Норма износа в точке
	float GetFactCurveNormaWear(double coord) const;

	/// Суммарная рихтовочная нить с учетом паспортных кривых
	long GetFactCurveSummaryThreadSide(double coord) const;

	// ------------------------------------------------------------------
	// Участки ПЧ
	// ------------------------------------------------------------------

	/// Список участков ПЧ на отрезке
	std::vector<VAprinfoPchRecord> GetPchList(double from, double to, bool cut_ranges_to_section = true) const;

	/// Список участков ПЧ на пути
	std::vector<VAprinfoPchRecord> GetPchList() const;

	/// Номер ПЧ в точке
	long GetPchNum(double coord) const;

	// ------------------------------------------------------------------
	// Скорости по приказу
	// ------------------------------------------------------------------

	/// Список скоростей по приказу на отрезке
	std::vector<VAprinfoSpeedRecord> GetSpeedList(double from, double to, bool cut_ranges_to_section = true) const;

	/// Список скоростей по приказу на пути
	std::vector<VAprinfoSpeedRecord> GetSpeedList() const;
	
	/// Список временных скоростей по приказу на отрезке
	std::vector<VAprinfoSpeedRecord> GetTmpSpeedList(double from, double to, bool cut_ranges_to_section = true) const;

	/// Список временных скоростей по приказу на пути
	std::vector<VAprinfoSpeedRecord> GetTmpSpeedList() const;

	/// Подмена списка скоростей на пути
	bool ReplaceSpeedList(const std::vector<VAprinfoSpeedRecord> & spd); 

	/// Скорости в точке
	VSpeeds GetSpeeds(double coord) const;
	
	/// Временные скорости в точке
	VSpeeds GetTmpSpeeds(double coord) const;
	
	/// Возвращает минимальные из временных и установленных скоростей
	VSpeeds GetCombinedSpeeds(double coord) const;
	
	/// Пассажирская скорость в точке
	speed_t GetPassSpeed(double coord) const;

	/// Грузовая скорость в точке
	speed_t GetTruckSpeed(double coord) const;

	/// Третья скорость в точке
	speed_t GetThirdSpeed(double coord) const;

	/// Скорость скоростного движения
	speed_t GetHighSpeed(double coord) const;

	/// Скорость скоростного движения с наклоном кузова
	speed_t GetHighSpeedBT(double coord) const;

	/// Пассажирская скорость в точке
	speed_t GetTempPassSpeed(double coord) const;

	/// Грузовая скорость в точке
	speed_t GetTempTruckSpeed(double coord) const;

	/// Третья скорость в точке
	speed_t GetTempThirdSpeed(double coord) const;

	// Минимальная пассажирская скорость на участке
	VSpeeds GetMinSpeeds(double from, double to) const;
	
	// Минимальное временное ограничение скорость на участке
	VSpeeds GetMinTmpSpeeds(double from, double to) const;

	/// Максимальная пассажирская скорость на участке
	speed_t GetMaxPassSpeed(double from, double to) const;

	/// Минимальная пассажирская скорость на участке
	speed_t GetMinPassSpeed(double from, double to) const;

	/// Максимальная грузовая скорость на участке
	speed_t GetMaxTruckSpeed(double from, double to) const;

	/// Минимальная грузовая скорость на участке
	speed_t GetMinTruckSpeed(double from, double to) const;

	/// Максимальная третья скорость на участке
	speed_t GetMaxThirdSpeed(double from, double to) const;

	/// Минимальная третья скорость на участке
	speed_t GetMinThirdSpeed(double from, double to) const;

	/// Минимальная скорость для скоростного движения
	speed_t GetMinHighSpeed(double from, double to) const;

	/// Минимальная скорость для скоростного движения с наклоном кузова
	speed_t GetMinHighSpeedBT(double from, double to) const;

	/// Максимальная скорость для скоростного движения
	speed_t GetMaxHighSpeed(double from, double to) const;

	/// Максимальная скорость для скоростного движения с наклоном кузова
	speed_t GetMaxHighSpeedBT(double from, double to) const;

	speed_t GetMinPassTmpSpeed(double from, double to) const;
	speed_t GetMinTruckTmpSpeed(double from, double to) const;
	speed_t GetMinThirdTmpSpeed(double from, double to) const;

	/// Список скоростей заполнен значениями "по умолчанию"
	bool DefaultSpeeds() const
	{
		return this->speedList.GetDefault();
	}

	// ------------------------------------------------------------------
	// Типы шпал
	// ------------------------------------------------------------------

	/// Список типов шпал на отрезке
	std::vector<VAprinfoIndexRecord> GetSpalaList(double from, double to, bool cut_ranges_to_section = true) const;

	/// Список типов шпал на пути
	std::vector<VAprinfoIndexRecord> GetSpalaList() const;

	/// Тип шпал в точке
	long GetSpala(double coord) const;

	// ------------------------------------------------------------------
	// Отверстия в шпалах
	// ------------------------------------------------------------------

	/// Диаметр отверстий на отрезке
	std::vector<VAprinfoIndexRecord> GetHoleDiameterList(double from, double to, bool cut_ranges_to_section = true) const;
	
	/// Диаметр отверстий на пути
	std::vector<VAprinfoIndexRecord> GetHoleDiameterList() const;

	/// Диаметр отверстий в точке
	long GetHoleDiameter(double coord) const;

	// ------------------------------------------------------------------
	// Категории пути
	// ------------------------------------------------------------------

	/// Категория пути в точке
	long GetWayCat(double coord) const;

	/// Категория пути на отрезке (минимум - самая строгая)
	long GetWayCat(double from, double to) const;

	/// Подмена категории пути
	bool ReplaceWayCatList(const std::vector<VAprinfoIndexRecord> & wc); 

	// ------------------------------------------------------------------
	// Стрелки
	// ------------------------------------------------------------------

	/// Список стрелок на отрезке
	std::vector<VAprinfoSwitchRecord> GetSwitchList(double from, double to) const;

	/// Список стрелок на пути
	std::vector<VAprinfoSwitchRecord> GetSwitchList() const;

	/// Нахождение на стрелке
	bool GetSwitch(double coord) const;

	/// Ориентация стрелки
	SWITCH_DIR GetSwitchDir(double coord) const;

	/// Описание ориентации стрелки
	static std::wstring GetSwitchDirDescription(SWITCH_DIR dir);

	// ------------------------------------------------------------------
	// Мосты
	// ------------------------------------------------------------------

	/// Список мостов на отрезке
	std::vector<VAprinfoIndexRecord> GetBridgeList(double from, double to, bool include_areas) const;

	/// Список мостов на всем участке
	std::vector<VAprinfoIndexRecord> GetBridgeList(bool include_areas) const;

	/// Положение на мосту (мост/охр.зона/не мост)
	long GetBridge(double coord) const;

	/// Длина моста
	double GetBridgeLength(double coord) const;

	// ------------------------------------------------------------------
	// Тоннели
	// ------------------------------------------------------------------

	/// Список тоннелей на отрезке
	std::vector<VAprinfoIndexRecord> GetTonnelList(double from, double to, bool include_areas) const;

	/// Список тоннелей на всем участке
	std::vector<VAprinfoIndexRecord> GetTonnelList(bool include_areas) const;

	/// Положение в тоннеле (мост/охр.зона/не мост)
	long GetTonnel(double coord) const;

	/// Длина тоннеля
	double GetTonnelLength(double coord) const;

	// ------------------------------------------------------------------
	// Метки профиля
	// ------------------------------------------------------------------

	/// Список паспортных меток профиля на отрезке
	std::vector<VAprinfoProfLabel> GetProfLabels(double from, double to) const;

	/// Список паспортных меток профиля на пути
	std::vector<VAprinfoProfLabel> GetProfLabels() const;

	// ------------------------------------------------------------------
	// Рихтовочные нити на прямых
	// ------------------------------------------------------------------

	/// Список рихтовочных нитей на отрезке
	std::vector<VAprinfoIndexRecord> GetStraightThreadSideList(double from, double to, bool cut_ranges_to_section = true) const;

	/// Список рихтовочных нитей на всем участке
	std::vector<VAprinfoIndexRecord> GetStraightThreadSideList() const;

	/// Рихтовочная нить в точке
	long GetStraightThreadSide(double coord) const;

	// ------------------------------------------------------------------
	//Реперные точки
	// ------------------------------------------------------------------
	double GetCoordPoint(int type, wxString id);

	// ------------------------------------------------------------------
	// Ремонты
	// ------------------------------------------------------------------

	/// Список ремонтов на отрезке
	std::vector<VAprinfoRepairRecord> GetRepairs(double from, double to, bool cut_ranges_to_section = true) const;

	/// Список ремонтов на всем участке
	std::vector<VAprinfoRepairRecord> GetRepairs() const;

	// ------------------------------------------------------------------
	// Участки обкатки
	// ------------------------------------------------------------------

	/// Список участков на отрезке
	std::vector<VAprinfoTestrunRecord> GetTestrunList(double from, double to) const;

	/// Список участков на всём участке
	std::vector<VAprinfoTestrunRecord> GetTestrunList() const;

	/// Обкатка в точке
	MACHINE_TYPE GetTestrunType(double coord) const;

	/// Скорость обкатки в точке
	VSpeeds GetTestrunSpeed(double coord) const;

	// ------------------------------------------------------------------
	// Типы рельсов и скреплений
	// ------------------------------------------------------------------

	/// Тип рельсов в точке
	long GetRailsType(double coord) const;

	/// Описание типа рельсов в точке
	std::string GetRailsTypeDescr(double coord) const;

	/// Список типов рельсов на отрезке
	std::vector<VAprinfoRailRecord> GetRailsTypeList(double from, double to, bool cut_ranges_to_section = true) const;

	/// Список типов рельсов на всём участке
	std::vector<VAprinfoRailRecord> GetRailsTypeList() const;

	/// Тип скреплений рельсов в точке
	long GetRailsBrace(double coord) const;

	/// Описание типа скреплений рельсов в точке
	std::string GetRailsBraceDescr(double coord) const;

	/// Список скреплений рельсов на отрезке
	std::vector<VAprinfoBraceRecord> GetRailsBraceList(double from, double to, bool cut_ranges_to_section = true) const;

	/// Список скреплений рельсов на всём участке
	std::vector<VAprinfoBraceRecord> GetRailsBraceList() const;

	// ------------------------------------------------------------------
	// Переезды
	// ------------------------------------------------------------------

	/// Список переездов на отрезке
	std::vector<VAprinfoCrossing> GetCrossings(double from, double to) const;

	/// Список переездов на всём участке
	std::vector<VAprinfoCrossing> GetCrossings() const;

	// ------------------------------------------------------------------
	// Уравнительные приборы
	// ------------------------------------------------------------------

	/// Наличие уравнительного прибора в точке
	bool GetChamJoint(double coord) const;

	/// Список уравнительных приборов на отрезке
	std::vector<VAprinfoObjectRecord> GetChamJoints(double from, double to) const;

	/// Список уравнительных приборов на всём участке
	std::vector<VAprinfoObjectRecord> GetChamJoints() const;
	
	// ------------------------------------------------------------------
	// Маркеры на пути
	// ------------------------------------------------------------------

    /// Путевые маркеры 
    const VWayMarkerList & GetMarkers() const
    {
        return this->markers;
    }

	// ------------------------------------------------------------------
	// Сравнение нулевых линий
	// ------------------------------------------------------------------

	/// Отчет о расхождении нулевых линий на плане по паспортным и измеренным кривым
	std::wstring CompareZL(int km) const;

	/// Суммарный отчет о расхождении нулевых линий на плане по паспортным и измеренным кривым
	/// (в лог)
	void CompareZL() const;

};

typedef boost::shared_ptr<VAprinfo> VAprinfoPtr;

template <class FILETYPE>
bool VAprinfo::Load(FILETYPE & file, uint32_t type)
{
	if (this->IsLoaded()) return true;
	this->Unload();

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
