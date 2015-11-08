#pragma once

#include "vaprinfo.h"
#include "singleton.h"
#include "VCalcOptions.h"
#include "markers.h"
#include "rtparam.h"

/// Длина оптической стрелки (м)
#define OPTICAL_SWITCH_LENGTH 60.0
/// Максимальная величина коррекции координат переходной
#define MAX_TRANSITION_CORRECTION 50.0
/// Минимальная длина моста
#define MIN_BRIDGE_LENGTH 25.0
/// Минимальный интервал между маркерами
#define MIN_MARKER_INTERVAL 1500.0

/// Класс события
class VRTMessage
{
private:
	/// Координата
	double coord;
	/// Сообщение
	std::wstring message;
public:
	/// Конструктор
	VRTMessage(double in_coord, const std::wstring& in_message);
	/// Возвращает координату
	double GetCoord() const;
	/// Возвращает сообщение
	const std::wstring& GetMsg() const;
};

/// Обозначение несовпадения плана
static const std::wstring BAD_PLAN_SIGN=L"[план]";
/// Обозначение несовпадения уровня
static const std::wstring BAD_LEVEL_SIGN=L"[уров.]";
/// Обозначение несовпадения координаты
static const std::wstring BAD_COORDINATES_SIGN=L"[коорд.]";
/// Допустимый коэффициент корреляции плана
static const double PLAN_CORRELATION=0.8;
/// Допустимый коэффициент корреляции уровня
static const double LEVEL_CORRELATION=0.8;
/// Процент совпадения координат
static const double COORDINATES_MATCH_PERCENT=80.0;

/// Участок несоответствия паспорту
class VBadPassport
{
private:
	/// Границы
	pro::interval<double> section;	
	/// Коэффициент корреляции кривизны
	double plan_correlation;	
	/// Коэффициент корреляции уровня
	double level_correlation;
	/// Размах паспорта кривизны
	double plan_range;	
	/// Размах паспорта уровня
	double level_range;
	/// Процент совпадения координат кривой
	double coordinates_match_percent;
public:
	/// Конструктор
	VBadPassport(double in_start, double in_end, 
		double in_plan_correlation, double in_plan_range,		
		double in_level_correlation, double in_level_range,
		double in_coordinates_match_percent);
	/// Возвращает границы
	const pro::interval<double>& GetSection() const;
	/// Возвращает описание
	const std::wstring GetDescription() const;
	/// Возвращает true, если кривая не соответствует паспорту по левой рихтовке
	bool IsBadPlan() const;			
	/// Возвращает true, если кривая не соответствует паспорту по уровню
	bool IsBadLevel() const;
	/// Возвращает true, если кривая не соответствует паспорту по координатам
	bool IsBadCoordinates() const;
	/// Возвращает true, если кривая не соответствует паспорту хотя бы по одному из параметров
	bool IsBad() const;
};

enum PWLF_TYPE {PWLF_PASSPORT=0, PWLF_ZERO=1};

/// Объекты пути, обнаруженные в реальном времени
class VSituation
{
private:

	/// Предикат поиска участков недостоверного паспторта
	struct find_bad_passport
	{
		double coord;
		find_bad_passport(double in_coord) : coord(in_coord) {};
		bool operator()(const VBadPassport& in_bad_passport) {return in_bad_passport.GetSection().contains(coord);};
	};

	/// Исходная априорная информация
	VAprinfo aprinfo;

	/// Список километров
	std::vector<VAprinfoKmRecord> km_list;
	/// Список кривых
	std::vector<VAprinfoCurve> curve_list;
	/// Участки недостоверных паспортных данных
	std::deque<VBadPassport> bad_passport;
	/// Стрелки
	std::deque<VAprinfoSwitchRecord> switches;
	/// Априорные стрелки
	std::vector<VAprinfoSwitchRecord> apr_switches;
	/// События
	std::deque<VRTMessage> messages;

	/// Паспортная линия (уровень)
	zline level_passport;
	/// Паспортная линия (рихтовка)
	zline rich_passport;
	/// Паспортная линия (ШК)
	zline width_passport;
		
	/// Предыдущие проезды (уровень)
	zline level_previous;
	/// Предыдущие проезды (рихтовка)
	zline rich_previous;

	/// Нулевая линия (уровень)
	zline level_zero;
	/// Нулевая линия (рихтовка левая)
	zline rich_left_zero;
	/// Нулевая линия (рихтовка правая)
	zline rich_right_zero;
	/// Нулевая линия (ШК)
	zline width_zero;
	
	/// Маркеры
	VWayMarkerList markers;
	
	/// Корректирует часть нулевой линии
	void AdjustLine(zline& out_zline, size_t in_index, const line_t& in_line);
	
public:

	/// Конструктор
	VSituation();
	/// Конструктор
	VSituation(const VAprinfo& in_aprinfo);
	/// Инициализация
	void Init(const VAprinfo& in_aprinfo);
	/// Возвращает максимальную скорость на направлении
	speed_t GetMaxSpeed() const;
	/// Возвращает число маркеров на направлении
	size_t GetMarkerCount() const;
	/// Возвращает максимальную длину участка между маркерами на направлении
	double GetMaxMarkerInterval(const pro::interval<double>& in_coordinates) const;
	/// Возвращает исходную апириорную информацию
	const VAprinfo& GetAprinfo() const;
	/// Возвращает ближайший километр
	VAprinfoKmRecord GetNearestKm(double in_coord) const;

	/// Возвращает количество априорных стрелок
	size_t GetAprSwitchCount() const;
	/// Возвращает априорную стрелку
	const VAprinfoSwitchRecord& GetAprSwitch(size_t in_index) const;

	/// Добавляет участок недостоверного паспорта
	void AddBadPassport(const VBadPassport& in_bad_passport);
	/// Возвращает участок недостоверного паспорта
	const VBadPassport& GetBadPassport(size_t in_index) const;
	/// Возвращает количество участков недостоверного паспорта
	size_t GetBadPassportSize() const;
	/// Возвращает признак недостоверных паспортных данных
	bool BadPassport(double in_coord) const;	
	/// Возвращает признак недостоверных паспортных данных
	bool BadPlanPassport(double in_coord) const;	
	/// Возвращает признак недостоверных паспортных данных
	bool BadLevelPassport(double in_coord) const;
	// Возвращает признак недостоверных паспортных данных
	bool BadCoordinatePassport(double in_coord) const;
	/// Добавляет стрелку
	void AddSwitch(double in_coord, side_t in_side);
	/// Возвращает стрелку
	const VAprinfoSwitchRecord& GetSwitch(size_t in_index) const;
	/// Возвращает количество стрелок
	size_t GetSwitchCount() const;
	/// Возвращает признак стрелки
	bool OnSwitch(const pro::interval<double>& in_range) const;
	/// Добавляет сообщение
	void AddMessage(const VRTMessage& in_message);
	/// Возвращает количество сообщений
	size_t GetMessageCount() const;
	/// Возвращает сообщение
	const VRTMessage& GetMsg(size_t in_index) const;
	/// Возвращает значение на паспортной линии (рихтовка)
	double GetPassportRich(double in_coord) const;
	/// Возвращает значение на паспортной линии (уровень)
	double GetPassportLevel(double in_coord) const;
	/// Возвращает значение на нулевой линии (уровень)
	double GetLevel(double in_coord) const;
	/// Возвращает значение на нулевой линии (рихтовка левая)
	double GetRichLeft(double in_coord) const;
	/// Возвращает значение на нулевой линии (рихтовка правая)
	double GetRichRight(double in_coord) const;
	/// Возвращает значение на нулевой линии (ШК)
	double GetWidth(double in_coord) const;
	/// Корректирует нулевую линию (уровень)
	void CorrectLevelTransition(const std::deque<point_t>& in_points);
	/// Корректирует нулевую линию (рихтовка)
	void CorrectRichLeftTransition(const std::deque<point_t>& in_points);
	/// Корректирует нулевую линию (рихтовка правая)
	void CorrectRichRightTransition(const std::deque<point_t>& in_points);
	/// Корректирует нулевую линию (ШК)
	void CorrectWidthTransition(const std::deque<point_t>& in_points);
	/// Корректирует нулевую линию (уровень)
	void CorrectLevelRadius(const std::deque<point_t>& in_points);
	/// Корректирует нулевую линию (рихтовка левая)
	void CorrectRichLeftRadius(const std::deque<point_t>& in_points);
	/// Корректирует нулевую линию (рихтовка правая)
	void CorrectRichRightRadius(const std::deque<point_t>& in_points);
	/// Корректирует нулевую линию (ШК)
	void CorrectWidthRadius(const std::deque<point_t>& in_points);	
	/// Возвращает число точек нулевой линии (уровень)
	size_t GetLevelCount(PWLF_TYPE in_type);
	/// Возвращает число точек нулевой линии (рихтовка)
	size_t GetRichCount(PWLF_TYPE in_type, side_t in_side);
	/// Возвращает число точек нулевой линии (ШК)
	size_t GetWidthCount(PWLF_TYPE in_type);
	/// Возвращает точку нулевой линии (уровень)
	point_t GetLevelPoint(size_t in_index, PWLF_TYPE in_type) const;
	/// Возвращает точку нулевой линии (рихтовка)
	point_t GetRichPoint(size_t in_index, PWLF_TYPE in_type, side_t in_side) const;
	/// Возвращает точку нулевой линии (ШК)
	point_t GetWidthPoint(size_t in_index, PWLF_TYPE in_type) const;
	/// Поиск эквивалентного маркера
	size_t FindMarker(const VWayMarker& in_marker) const;
	/// Возвращает маркер
	const VWayMarker& GetMarker(size_t in_index) const;
	/// Записывает априорные маркеры в файл
	void DumpMarkers(const std::wstring& in_filename) const;
	///Возвращает путевую координату
	VWayCoord GetRealCoord(double in_abscoord) const;
	/// Возвращает скорости по приказу
	VSpeeds GetCombinedSpeeds(double in_abscoord) const;
	/// Возвращает скорости по приказу с учетом границ кривых
	VSpeeds GetCurveSpeeds(double in_abscoord) const;
	/// Возвращает скорости по приказу
	VSpeeds GetSpeeds(double in_abscoord) const;
	/// Возвращае минимальную скорость на участке
	VSpeeds GetMinSpeeds(double in_start, double in_end) const; 	
 	/// Возвращает радиус кривой
 	double GetCurveRadius(double in_abscoord) const;
 	/// Возвращает признак уравнительного прибора
	bool GetChamJoint(double in_abscoord) const;
	/// Возвраает тип шпал
	long GetSpala(double in_abscoord) const;
	/// Возвращает признак моста или охранной зоны
	bool GetBridge(double in_abscoord) const;
	/// Возвращает признак тоннеля или охранной зоны
	bool GetTonnel(double in_abscoord) const;
	/// Возвращает признак кривой
	bool IsCurvePlan(double in_abscoord) const;
	/// Возвращает оринетацию кривой
	long GetCurveOrientation(double in_abscoord) const;
	/// Возвращает признак кривой (уровень)
	bool IsCurveLevel(double in_abscoord) const;
	/// Возвращает рихтовочную нить 
	long GetSummaryThreadSide(double in_abscoord) const;
	/// Возвращает диаметр отверстий
	long GetHoleDiameter(double in_abscoord) const;
	/// Возвращает признак контрольного участка
	bool IsControlArea(double in_abscoord) const;
	/// Возвращает МО ШК на котрольном участке
	float GetControlAreaWidth(double in_abscoord) const;
	/// Возвращает СКО ШК на котрольном участке
	float GetControlAreaWidthSKO(double in_abscoord) const;
	/// Возвращает МО уровня на контрольном участке
	float GetControlAreaLevel(double in_abscoord) const;
	/// Возвращает СКО уровня на контрольном участке
	float GetControlAreaLevelSKO(double in_abscoord) const;
	/// Возвращает паспортный износ
	double GetNormaWear(double in_abscoord) const;	
	/// Возвращает тип нулевой линии
	ZLINE_TYPE GetCurveStatusPlan(double in_abscoord) const;
	/// Возвращает тип нулевой линии (уровень)
	ZLINE_TYPE GetCurveStatusLevel(double in_abscoord) const;
	/// Возвращает номер ПЧ
	long GetPchNum(double in_abscoord) const;
	/// Возвращает километр
	VAprinfoKmRecord GetKm(double in_abscoord) const;
	/// Возвращает название направления
	std::string GetDirName() const;
	/// Возвращает код направления
	long GetDirCode() const;
	/// Возвращает номер пути
	std::string GetWayCode() const;
	/// Возвращает перегон
	std::string GetPeregon(double in_abscoord) const;
	/// Возвращает список паспортных кривых
	std::vector<VAprinfoCurve> GetPassportCurveList(double in_start, double in_end) const;
	/// Возвращает пикет
	VPicket GetPk(double in_abscoord) const;
	/// Возвращает список скоростей
	std::vector<VAprinfoSpeedRecord> GetSpeedList(double in_start, double in_end) const;
	/// Возвращает список временных ограничений
	std::vector<VAprinfoSpeedRecord> GetTmpSpeedList(double in_start, double in_end) const;
	/// Возвращает список перегонов и раздельных пунктов
	std::vector<VAprinfoPntRecord> GetPntList(double in_start, double in_end) const;
	/// Возвращает список ПЧ
	std::vector<VAprinfoPchRecord> GetPchList(double in_start, double in_end) const;
	/// Возвращает список ремонтов
	std::vector<VAprinfoRepairRecord> GetRepairs(double in_start, double in_end) const;
	/// Возвращает список плетей
	std::vector<VAprinfoWobRecord> GetWobLeftList(double in_start, double in_end) const;
	/// Возвращает список плетей
	std::vector<VAprinfoWobRecord> GetWobRightList(double in_start, double in_end) const;
	/// Возвращает список участков типов рельсов
	std::vector<VAprinfoRailRecord> GetRailsTypeList(double in_start, double in_end) const;
	/// Возвращает список пикетов на километре
	std::vector<VPicket> GetPkList(long in_km) const;
	/// Возвращает список участков обкатки
	std::vector<VAprinfoTestrunRecord> GetTestrunList(double in_start, double in_end) const;
	/// Возвращает признак станции
	bool IsStation(double in_abscoord) const;
	/// Возвращает true если тип рельсов Р50
	bool P50(double in_abscoord) const;
	/// Возвращает тип участка обкатки
	MACHINE_TYPE GetTestRun(double in_abscoord) const;
	/// Возвращает признак плети
	bool GetWobLeft(double in_abscoord) const;
	/// Возвращает признак плети
	bool GetWobRight(double in_abscoord) const;
	/// Возвращает априорную длину километра
	double GetKmLength(long in_km) const ;
	
	/// Возвращает true если в точке действуют условия для ограничения ПРЖ
	bool IsPrj(double in_coord);

};
typedef VSingleton<VSituation> VCurrentSituation;
#define SITUATION_INIT(objects) VCurrentSituation::Instance().SetObject(objects)
#define SITUATION VCurrentSituation::Instance().GetObject()
#define GLOBAL_APRINFO VCurrentSituation::Instance().GetObject().GetAprinfo()

