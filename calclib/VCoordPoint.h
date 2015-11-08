#pragma once

#include "VDataSet.h"
#include "VParameter.h"
#include "VAprinfo.h"
#include "VOpMark.h"

enum POINT_TYPE 
{
	POINT_BRIDGE = 1,
	POINT_POLE = 4,
	POINT_SPALA_MARKER = 10,
	POINT_POLE_ACCURATE = 20
};

/*
мост - 1
радар - 2
разд. пункт - 3
км столб - 4
пикет - 5
остряк левый - 6
остряк правый - 7
крестовина левая - 8
крестовина правая - 9
метка Бредюка - 10
отмеренный километровый столб - 20
*/

struct VCoordPoint 
{
	std::wstring filename;		 // имя файла, в котором находится данный объект пути
	std::wstring id;			 // идентификатор объекта
	double abscoord;			 // измеренная координата
	double apr_abscoord;		 // априорная координата
	size_t index;				 // индекс из массива данных в файле
	POINT_TYPE type;			 // тип объекта пути
	bool isIgnore;				 // игнорировать (true) измерение
	int row;					 // номер строки в гриде
	int col;					 // номер столбца в гриде

	bool operator<(const VCoordPoint & coordpoint2)
	{
		return (abscoord < coordpoint2.abscoord);
	}

	// Участок вписывается в границы проезда, по числу кадров
	bool Fits(size_t kadrCount) const { return true; }
};

class VCoordPointList : public VIndexList<VCoordPoint>
{
public:
	/// Загрузка
	bool Load(VDataSet & ds, POINT_TYPE type, bool load_apr_abscoord = false);
	// Выгрузка
	void Unload(VDataSet & ds);
	// Получение максимального значения
	double GetMax(POINT_TYPE in_type) const;
	// Получение максимального значения
	double GetMin(POINT_TYPE in_type) const;
	// Возвращает массив индексов соответсвующих объектам выбранного типа
	// std::vector<size_t> GetPoint(POINT_TYPE in_type);
	// Поиск среднего отклонения от одноимённых меток в базовом файле в одном файле
	double CalcDelta(POINT_TYPE in_type, VCoordPointList coord_point_list);
	// Старая функция подсчёта априорного положения километровых столбов
	double GetAprAbscoord(double abscoord, VAprinfo aprinfo);
	// Удалить объект
	void DeletePoint(size_t in_index);
};

//---------------Класс для автокоррекции по различным объектам пути-----------------------------------
class VCoordPointAutoCorr : public VCoordPointList
{
	//параметры прямой по отклонениям км. столбов
	line_t line_spala_marker;
	line_t line_pole;
	std::map<POINT_TYPE, size_t> start_index; //начало блока данных с объектами пути (для ускорения доступа к данным)
public:
	// Вычисление апроксимирующей прямой
	void CalcLine(POINT_TYPE in_type);
	// Получение апроксимирующей прямой
	line_t GetLine(POINT_TYPE in_type);
	// Отбраковать объект
	void SetCorrupt(VDataSet & dataSet, POINT_TYPE type, size_t index);
	// Снять отбраковку
//	void UnsetCorrupt(VDataSet dataSet, size_t index);
	//Перегруженный метод, возвращающий значение по индексу и по типу точки
	VCoordPoint & GetPoint(POINT_TYPE in_type, size_t index);
	//Перегруженный метод, возвращающий кол-во элементов, соответствующих данному типу точки
	size_t GetCountPoint(POINT_TYPE in_type);
	//Метод возвращающий кол-во несбойных меток
	size_t GetCountNotCorrupt(POINT_TYPE in_type);
};

//---------------Класс для поиска реперных точек------------------------------------------------------
class VRepPointList : public VCoordPointList
{
	// Вектор средних значений по строкам
	std::vector<double> average;
	// Вектор СКО по строкам
	std::vector<double> sko;
public:
	// Сортировка для Grid 
	void SortForGrid(std::map<std::wstring,int> Filenames);
	// Возвращает максимальное СКО
	double GetMaxSKO();
	// Возвращает среднее по строке
	double GetAverage(int row);
	// Возвращает СКО по строке
	double GetSKO(int row);
	// Отбраковка измерений
	void ReCalc(double porog);
	// Получить значение соответсвующее данной строке и столбцу
	VCoordPoint GetPoint(int row, int col);
	// Установить значение соответсвующее данной строке и столбцу
	void SetPoint(int row, int col);
	// Восстановить исходное состояние таблицы
	void Reload();
	// Первичная обработка измеренных данных
	bool FirstCheckData();
};