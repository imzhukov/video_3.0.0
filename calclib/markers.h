#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "options.h"
#include "timelib.h"
#include "dblib.h"
#include "types.h"
#include "VFileName.h"
#include "interval.h"

class VDataSet;

/// Типы маркеров
enum WAY_MARKER_TYPE {
	MARKER_NONE=0, 
	MARKER_KM=1, 
	MARKER_CURVE_BOUND_START=2, 
	MARKER_CURVE_BOUND_END=3, 
	MARKER_SWITCH=4, 
	MARKER_RFID=5
};

/// Возвращает true, если тип маркеров разрешен
bool AllowedMarkerType(WAY_MARKER_TYPE in_type);

/// Маркер
class VWayMarker
{
private:
	/// Индекс кадра
	size_t index;
	/// Абсолютная координата 
	double abscoord;
	/// Идентификатор
	long id;
	/// Тип
	WAY_MARKER_TYPE type;
	/// Широта, долгота
	double latitude, longitude;
	/// Дата
	VTime timestamp;
public:

	/// Конструктор
	VWayMarker(size_t in_index, double in_abscoord, long in_id, WAY_MARKER_TYPE in_type, 
		double in_lat, double in_long, const VTime & ts);

	/// Возвращает индекс кадра
	size_t GetIndex() const;
	/// Возвращает абсолютную координату
	double GetCoord() const;
	/// Возвращает идентификатор
	long GetID() const;
	/// Возвращает тип
	WAY_MARKER_TYPE GetType() const;
	/// Возвращает строковый тип маркера, для базы
	std::string GetTypeString() const;
	/// Возвращает описание
	std::wstring GetDescription() const;
	/// Возвращает широту
	double GetLatitude() const;
	/// Возвращает долготу
	double GetLongitude() const;
	/// Возвращает время маркера
	const VTime& GetDateTime() const;
	/// Оператор<
	bool operator<(const VWayMarker& in_r) const;
	/// Получить список близких меток из базы
	std::vector<VWayMarker> GetCloseMarkers(VIBDatabasePtr db, 
		VIBTransaction & trans, __int64 way_id,
		double coord_delta, double angle_delta) const;
	/// Возвращает true, если маркеры эквивалентны		
	bool Equivalent(const VWayMarker& in_marker) const;
	/// Возвращает расстояние до маркера
	double Distance(const VWayMarker& in_marker) const;
	///Оператор вывода в поток
	friend std::ostream& operator<<(std::ostream& out, const VWayMarker& in_marker)
	{
		out << in_marker.GetDescription() << L" " << in_marker.GetDateTime().ToWideStringLocale();
		return out;
	}
};

/// Отклонение маркера
class VWayMarkerDeviation
{
private:
	/// Обнаруженный маркер
	VWayMarker detected;
	/// Априорный маркер
	VWayMarker apriore;
public:
	/// Конструктор
	VWayMarkerDeviation(const VWayMarker& in_detected, const VWayMarker& in_apriore);
	/// Возвращает координату
	double GetCoord() const;
	/// Возвращает отклонение
	double GetDeviation() const;
};

/// Список маркеров
class VWayMarkerList
{
private:
	/// Данные
	std::vector<VWayMarker> markers;
	/// Признак того что данные загружены
	bool isLoaded;
public:
	/// Конструктор
	VWayMarkerList();
	/// Загрузка данных из файла
	bool Load(VDataSet & ds);
	/// Загрузка данных из базы
	bool Load(VIBDatabase & db, long dirCode, const std::string & wayName, const VWaySection & section);
	/// Возвращает число маркеров
	size_t GetCount() const;
	/// Возвращает маркер
	const VWayMarker& GetItem(size_t in_index) const;
	/// Находит эквивалентный маркер
	size_t FindEquivalentMarker(const VWayMarker& in_marker) const;
	/// Добавляет маркер
	void AddItem(const VWayMarker& in_marker);
	/// Возвращает расстояние до идентичного маркера
	double GetDistance(const VWayMarker& in_marker) const;
	/// Выгружает данные
	void Unload();
	/// Возвращает максимальное расстояние между двумя маркерами на заданном интревале координат
	double GetMaxInterval(const pro::interval<double>& in_coordinates) const;
	/// Сохраняет список маркеров в файл
	void Dump(std::ofstream& out) const;
};

/// Расстояние идентфикации метки
DECLARE_OPTION(double, MARKER_IDENTIFICATION_DISTANCE, 100.0, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())
/// Угол идентификации кривой
DECLARE_OPTION(double, CURVE_IDENTIFICATION_ACCURACY, 1.0, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())