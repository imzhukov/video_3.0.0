#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include "VParameter.h"
#include "VAprinfo.h"
#include "VPlanElemList.h"
#include "VCurveList.h"
#include "oversteps.h"
#include <vector>

using namespace std;

/** Определяем класс VDataSet*/
class VDataSet;

static point_t CalcNextPoint(const point_t& in_prev_point, double in_course, double in_delta_abs)
{
	return point_t(in_prev_point.x+sin(in_course)*in_delta_abs, in_prev_point.y+cos(in_course)*in_delta_abs);
}

/** Расчет положения пути в плане*/
class VPlan : public VParameterProto<point_t>
{
private:

	//Данные

	/** Массив данных*/
	vector<point_t> data;
	/** Признак того, что данные загружены и рассчитаны*/
	bool is_loaded;

public:
	/** Конструктор*/
	VPlan()
	{};

	/** Деструктор*/
	~VPlan()
	{
		this->Unload();
	};

	/** Выгрузка*/
	void Unload();

	/** Загрузка и расчет*/
	bool Load(VDataSet& ds);	

	/** Число записей*/
	size_t GetCount() const 
	{
		return data.size();
	};

	/** Вернуть констатную ссылку на i-й элемент*/
	const point_t & GetItem(size_t in_index) const 
	{
		return data.at(in_index);
	};

	/** Вернуть ссылку на i-й элемент*/
	point_t & GetItem(size_t in_index) 
	{
		return data.at(in_index);
	};	
};

/** "Идеальный" план*/
class VIdealPlan : public VParameterProto<point_t>
{
protected:

	//Данные

	/** Массив данных*/
	vector<point_t> data;

	/** Признак того, что это паспортный план*/
	bool is_model;


	//Методы

	/** Расчет плана для кривой*/
	void CalcCurvePlan(const VCurve & curve, const VParameter & in_course, 
		const VAbsCoordData & in_abscoord);
	/** Расчет плана для прямой*/
	void CalcLinePlan(const VPlanElement & plan_element, const VPlan & in_plan, const VAbsCoordData & in_abscoord);	
	/** Копирует измеренный план*/
	void CopyMeasuredPlan(const VPlanElement & plan_element, const VPlan & in_plan);
	/** Подгонка плана на элементе*/
	void CorrectPlan(const VPlanElement & in_plan_element, const VPlan & in_plan);
	/** Сглаживание углов кривыми Безье*/
	void Bezier(const VPlanElemList& in_plan_elem_list, size_t in_first, size_t in_last);

public:

	/** Конструктор*/
	VIdealPlan(bool in_is_model) : is_model(in_is_model)
	{};

	/** Деструктор*/
	~VIdealPlan() 
	{
		Unload();
	};

	/** Выгрузка*/
	void Unload();

	/** Загрузка и расчет*/
	bool Load(VDataSet& ds);

	/** Загрузка и расчет*/
	bool Load(const VParameter& in_course, const VPlan& in_plan, 
		const VAbsCoordData& in_abscoord, const VPlanElemList& in_plan_elements, 
		const VCurveList& in_curves);

	/** Обновление*/
	void Update(VDataSet& ds, size_t in_first_element_index, size_t in_last_element_index);

	/** Число записей*/
	size_t GetCount() const 
	{
		return data.size();
	};

	/** Вернуть констатную ссылку на i-й элемент*/
	const point_t & GetItem(size_t in_index) const 
	{
		return data.at(in_index);
	};

	/** Вернуть ссылку на i-й элемент*/
	point_t & GetItem(size_t in_index) 
	{
		return data.at(in_index);
	};	
};

/// План по фактической разметке
class VIdealPlanReal : public VIdealPlan
{
public:
	///Конструктор
	VIdealPlanReal() : VIdealPlan(false){};
};

/// План по паспортной разметке
class VIdealPlanModel : public VIdealPlan
{
public:
	///Конструктор
	VIdealPlanModel() : VIdealPlan(true){};
};

/// Отклонения плана от идеального
class VPlanOffset : public VParameter
{	
protected:
	///Признак источника данных кривых - фактические/паспортные
	bool is_model;
public:
	/// Конструктор
	VPlanOffset(bool in_is_model) : is_model(in_is_model){};
	/// Загрузка
	bool Load(const VPlan& in_plan, const VIdealPlan& in_ideal_plan);
	/// Загрузка
	bool Load(VDataSet& ds);
	/// Обновление
	bool Update(VDataSet& ds, size_t in_start, size_t in_end);
};

/// Отклонения плана от идельного по фактической разметке 
class VPlanOffsetReal : public VPlanOffset
{
public:
	///Конструктор
	VPlanOffsetReal() : VPlanOffset(false) {};
};

/// Отклонения плана от идельного по паспортной разметке
class VPlanOffsetModel : public VPlanOffset
{
public:
	///Конструктор
	VPlanOffsetModel() : VPlanOffset(true) {};
};

/// Непогашенное ускорение фактическое
class VAnpReal : public VParameter
{
public:
	/// Загрузка
	bool Load(VDataSet& ds, speed_type in_speed);
};

/// Скорость нарастания непогашенного ускорения фактическая
class VPsiReal : public VParameter
{
public:
	///Загрузка
	bool Load(VDataSet& ds, speed_type in_speed);
};

/// Непогашенное ускорение по модели
class VAnpModel : public VParameterProto<VAnp>
{
public:
	/// Загрузка
	bool Load(const VAbsCoordData & in_abscoord, const VCurveList & in_curves, const VAprinfo& in_aprinfo, speed_type in_speed, bool in_use_passport_speeds=true);
	///Выгрузка
	void Unload();
	/// Число записей
	virtual size_t GetCount() const;
	/// Вернуть констатную ссылку на i-й элемент
	virtual const VAnp & GetItem(size_t index) const;
	/// Вернуть ссылку на i-й элемент
	virtual VAnp & GetItem(size_t index);
	/// Возвращает минимальное Анп
	VAnp & GetMin();
	/// Возвращает максимальное Анп
	VAnp & GetMax();
private:
	///Данные
	std::vector<VAnp> data;
};


/// Неровности плана (отклонение от огибающей)
class VJaggyPlan : public VParameter
{
public:
	/// Загрузка
	bool Load(VDataSet& ds, bool lines_only);
};



/// Степень опасности
enum JAGGY_PLAN_SIGNIFICANCE {PLAN_SIGN_1=1, PLAN_SIGN_2=2};

/// Сверхнормативная неровность
class VJaggyPlanOverstep : public VBasicOverstep
{
private:
	/// Степень опасности
	JAGGY_PLAN_SIGNIFICANCE significance;
	/// Координата FixMe!!! в базовый класс
	double coord;
	/// Крутизна отвода
	double otvod;
	/// Признак моста
	bool on_bridge;
	/// Признак стрелки
	bool on_switch;
	/// Признак станции
	bool on_station;
public:
	/// Конструктор
	VJaggyPlanOverstep(double in_start, double in_end, double in_coord, double in_value, 
		double in_otvod, JAGGY_PLAN_SIGNIFICANCE in_significance, const VAprinfo& in_aprinfo);
	/// Возвращает координату
	double GetCoord() const;
	/// Возвращает максимальный отвод
	double GetOtvod() const;
	/// Вовращает описание
	std::wstring GetDescription() const;
	/// Возвращает строку с описанием степени
	std::wstring GetDegreeString() const;
};
typedef boost::shared_ptr<VJaggyPlanOverstep> VJaggyPlanOverstepPtr;


/// Сверхнормативные неровности плана
class VJaggyPlanOverstepList
{
private:
	/// Признак того что данные загружены
	bool loaded;
	/// Список отступлений
	std::deque<VJaggyPlanOverstepPtr> oversteps;

	/// Добавляет отступление в список
	void AddOverstep(double start, double end, double center, double value, double otvod, const VAprinfo& aprinfo);	
	/// Расчет
	void Calculate(VDataSet& in_ds);

public:
	/// Конструктор
	VJaggyPlanOverstepList();
	/// Загрузка
	bool Load(VDataSet& in_ds);
	/// Выгрузка
	void Unload();
	/// Возвращает число записей
	size_t GetCount() const;
	/// Возвращает запись по номеру
	const VJaggyPlanOverstepPtr& GetItem(size_t in_index) const;
};