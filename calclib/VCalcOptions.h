#pragma once

//stl
#include <strstream>
#include <memory>
#include <fstream>
#include <vector>
#include <algorithm>
#include <set>

//boost
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>

#include "VException.h"
#include "mathlib.h"
#include "strlib.h"
#include "singleton.h"

#include "Shiny.h"

/// Да/Нет
extern const std::wstring OPTION_YES;
extern const std::wstring OPTION_NO;

/// Определяем режим расшифровки ЦП-515 оригинальная или с дополнениями
extern const std::wstring OPTION_515;
extern const std::wstring OPTION_EXPERIMENT;

/// Определяем режим паспортных данных
extern const std::wstring OPTION_PASSPORT;
extern const std::wstring OPTION_FACT;

///Режимы печати
extern const std::wstring OPTION_PRINT_ALL;
extern const std::wstring OPTION_PRINT_OVERSTEPS;
extern const std::wstring OPTION_PRINT_RESTRICTIONS;

/// Закрытие движения
extern const std::wstring OPTION_CLOSE;
extern const std::wstring OPTION_RESTRICT_25;

///Масштабы ленты
extern const std::wstring OPTION_SCALE1;
extern const std::wstring OPTION_SCALE2;
extern const std::wstring OPTION_SCALE3;

/// Источники ППР
extern const std::wstring OPTION_PPR_PC1;
extern const std::wstring OPTION_PPR_PPR;

/// Источники данных для шаблона и износа
extern const std::wstring OPTION_STD;
extern const std::wstring OPTION_PPR;
extern const std::wstring OPTION_FRONT;
extern const std::wstring OPTION_BACK;

/// Переменная
class VOption
{
private:
	/// Название
	std::wstring name;
	/// Значение
	std::wstring value;
	/// Значение по умолчанию
	std::wstring default_value;
	/// Группа
	std::wstring group;
	/// Описание
	std::wstring description;	
	/// Список допустимых значение
	std::vector<std::wstring> avail_list;
	/// Хэш
	uint32_t hash;
public:
	/// Конструктор
	VOption();
	/// Конструктор
	VOption(const std::wstring& in_name, const std::wstring& in_value, const std::wstring& in_group,
		const std::wstring& in_description);
	/// Конструктор
	VOption(const std::wstring& in_name, const std::wstring& in_value, const std::wstring& in_group, 
		const std::wstring& in_description, const std::vector<std::wstring>& in_avail_list);
	/// Конструктор
	VOption(const std::wstring& in_name, const std::wstring& in_value, const std::wstring& in_group, 
		const std::wstring& in_description, const std::wstring& in_avail_list);
	/// Возвращает название
	const std::wstring& GetName() const;	
	/// Возвращает значение
	template<class T> T GetValue() const
	{
		try
		{
			return boost::lexical_cast<T>(value);
		}
		catch (std::exception&)
		{
			std::wstring msg=boost::str(boost::wformat(L"%s %s") % description % value);
			throw VSimpleException(L"Некорректное значение параметра: ", msg);
		}
	}
	/// Спецификация GetValue для std::wstring 
	template<> std::wstring GetValue<std::wstring>() const
	{
		return value;	
	};
	/// Перегруженная GetValue
	std::wstring GetValue() const
	{
		return value;
	};
	/// Возвращает группу
	const std::wstring& GetGroup() const;
	/// Возвращает описание
	const std::wstring& GetDescription() const;
	/// Устанавливает значение
	void SetValue(const std::wstring& in_value);
	/// Устанавливает значение по умолчанию		
	void ResetValue();
	/// Возвращает true, если значение соответсвет параметру
	bool Compare(const std::wstring& in_value) const;
	/// Добавляет доступное значение
	void AddAvail(const std::wstring& in_avail);
	/// Возвращает число доступных значений
	size_t GetAvailCount() const;
	/// Возвращает доступное значение
	const std::wstring& GetAvail(size_t in_index) const;
	/// Возвращает хэш
	uint32_t GetHash() const;

	/// Оператор ()
	VOption& operator[](const std::wstring& in_value)
	{
		AddAvail(in_value);
		return * this;
	}
	
	/// оператор <
	bool operator<(const VOption& in_option) const
	{
		return name.compare(in_option.name)<0;
	}
	
	/// оператор ==
	bool operator==(const VOption& in_option) const
	{
		return in_option.GetHash()==GetHash();
	}
	
	/// оператор <<
	friend std::ostream& operator<<(std::ostream& out, VOption& in_option)
	{
		out << "// " << wstring_to_string(in_option.GetGroup().c_str()) << " : " << wstring_to_string(in_option.GetDescription()).c_str();
		if (in_option.GetAvailCount()>0)
		{
			out << L" [";
			for (size_t i=0; i<in_option.GetAvailCount(); ++i)
			{
				if (i>0) out << L";";
				out << in_option.GetAvail(i);
			}
			out << L"]";
		}
		out << std::endl;
		out << wstring_to_string(in_option.GetName().c_str()) << "=" << 
			wstring_to_string(in_option.GetValue<std::wstring>()).c_str() << std::endl;
		return out;
	};
};

/// Набор опций
class VOptions
{
protected:

	// Данные

	/// Имя файла
	std::wstring filename;
	/// Список переменных
	std::set<VOption> options;
	/// Группы
	std::vector<std::wstring> groups;
	/// Поиск опции по имени
	class find_option_by_name
	{
	private:
		std::wstring name;
	public:
		find_option_by_name(const std::wstring& in_name) : name(in_name){};
		bool operator()(const VOption& in_option) const
		{		
			return in_option.GetName().compare(name)==0;
		}
	};
	/// Поиск по ключу
	class find_option_by_hash
	{
	private:
		uint32_t hash;
	public:
		find_option_by_hash(uint32_t in_hash) : hash(in_hash){};
		bool operator()(const VOption& in_option) const
		{		
			return in_option.GetHash()==hash;
		}
	};
	/// Поиск по группе и описанию
	class find_option_by_description
	{
	private:
		std::wstring group;
		std::wstring description;
	public:
		find_option_by_description(const std::wstring& in_group, const std::wstring& in_description) : 
		  group(in_group), description(in_description){};
		bool operator()(const VOption& in_option) const
		{
			return in_option.GetGroup().compare(group)==0 && 
				in_option.GetDescription().compare(description)==0;
		}
	};

public:
	/// Загрузка данных
	void Load(const std::wstring& in_filename);
	/// Запись опций
	void Save();
	/// Возвращает число переменных
	size_t GetOptionsCount() const;
	/// Возвращает переменную по индексу
	const VOption& GetOption(size_t in_index);	
	/// Возвращает true если опция присутствует в списке
	bool HasOption(const std::wstring& in_name) const;
	/// Возвращает true если опция присутствует в списке
	bool HasOption(const std::wstring& in_group, const std::wstring& in_description) const;
	/// Возвращает переменную по ее имени
	const VOption& GetOption(const std::wstring& in_name);
	/// Возвращает переменную по группе и описанию
	const VOption& GetOption(const std::wstring& in_group, const std::wstring& in_description);
	/// Возвращает переменную по ее имени
	const VOption& GetOption(const std::wstring& in_name) const;
	/// Возвращает переменную по группе и описанию
	const VOption& GetOption(const std::wstring& in_group, const std::wstring& in_description) const ;
	/// Удаляет переменную по ссылке
	void DeleteOption(const VOption & opt)
	{
		options.erase(opt);
	}
	void AddOption(const VOption & opt)
	{
		options.insert(opt);
	}
	/// Возвращает число групп
	size_t GetGroupCount() const;
	/// Возвращает группу
	const std::wstring& GetGroup(size_t in_index) const;
};


/** Класс по шаблону "одиночка", содержащий информацию настройках методов расчета*/
class VCalcOptions  : public VOptions
{
private:		

	/** Инициализация опций*/
	void InitOptions();	
		
public:	
	
	/** Конструктор*/
	VCalcOptions();
	/** Конструктор*/
	VCalcOptions(const std::wstring& in_filename);
	/** Деструктор*/
	~VCalcOptions();
		
	//Методы возвращающие константы

	/** Минимальная кривизна при поиске кривых*/
	double MIN_CURVE() const {return 5.0;};	
	/** Минимальный угол поворота при поиске кривых (40 минут)*/
	double MIN_ANGLE() const {return angle_t(0,40,0).get_rad();};	
	/** База для расчета производной при авторазметке (м)*/
	double CURVE_BASE() const {return 32.0;};
	/** Значение производной при поиске кривой*/
	double CURVE_DERIVED() const {return 0.01;};
	/** Минимальная длина переходной*/
	double MIN_LENGTH_TRANS() const {return std::max<double>(1.0, GetOption(L"MIN_LENGTH_TRANS").GetValue<double>());};
	/** Минимальная длина круговой*/
	double MIN_LENGTH_CURVE() const {return std::max<double>(1.0, GetOption(L"MIN_LENGTH_CURVE").GetValue<double>());};	
	/** Минимальное значение возвышения на кривой*/
	double MIN_LEVEL() const {return GetOption(L"MIN_LEVEL").GetValue<double>();};
	/** Граница непогашенного ускорения для высокскоростных поездов*/
	double THRESHOLD_ANP_HIGH() const {return 1.0;};
	/** Граница непогашенного ускорения для высокоскоростных поездов с наклоном кузова*/
	double THRESHOLD_ANP_HIGH_BT() const {return 1.5;};
	/** Граница непогашенного ускорения для пассажирских поездов*/
	double THRESHOLD_ANP_PASS() const {return 0.7;};
	/** Граница непогашенного ускорения для грузовых поездов*/
	double THRESHOLD_ANP_TRUCK() const {return 0.3;};
	/** Скорость по умолчанию*/
	double DEFAULT_SPEED() const {return 60;};

	pro::interval<size_t> SPEEDS_RANGE_1() const {return pro::interval<size_t>(141, 160);};
	pro::interval<size_t> SPEEDS_RANGE_2() const {return pro::interval<size_t>(161, 200);};
	pro::interval<size_t> SPEEDS_RANGE_3() const {return pro::interval<size_t>(201, 250);};

	// Методы - политики

	/** Возвращает базу усреднения для расчета непогашенного ускорения в зависимости от скорости*/
	double ANP_BASE(int in_speed) const
	{
		if (in_speed>140)
		{
			return 60;
		}
		else 
		{
			return 50;
		}
	}
	
	/** База расчета скорости нарастания непогашенного ускорения в зависимости от скорости*/
	double PSI_BASE(int in_speed) const 
	{
		if (in_speed>140)
		{
			return 40;
		}
		else
		{
			return 30;
		}
	};
	
	/** База расчета уклона отвода возвышения в зависимости от скорости*/
	double LEVEL_OTVOD_BASE(int in_speed) const 
	{
		if (in_speed>140)
		{
			return 40;
		}
		else
		{
			return 30;
		}
	};	

	/** Возвращает базу усреднения в зависимости от скорости*/
	double JAGGY_BASE(int in_speed) const
	{
		if (SPEEDS_RANGE_1().contains(in_speed))
		{
			return 100;
		}
		else if (SPEEDS_RANGE_2().contains(in_speed))
		{
			return 120;
		}
		else if (SPEEDS_RANGE_3().contains(in_speed))
		{
			return 150;
		}
		else
		{
			return 80;
		}
	}
	/** Возвращает базу расчета крутизны отвода неровностей в зависимости от скорости*/
	double JAGGY_OTVOD_BASE(int in_speed)
	{
		if (SPEEDS_RANGE_1().contains(in_speed))
		{
			return 30;
		}
		else if (SPEEDS_RANGE_2().contains(in_speed))
		{
			return 40;
		}
		else if (SPEEDS_RANGE_3().contains(in_speed))
		{
			return 50;
		}
		else
		{
			return 30;
		}
	}		
};

typedef VSingleton<VCalcOptions> CALC_OPTIONS;
#define CALC_OPTIONS_INIT(options) CALC_OPTIONS::Instance().SetObject(options)
#define OPTIONS CALC_OPTIONS::Instance().GetObject()

/// Настройки и нормативы
class VDetectOptions : public VOptions
{
private:	
	
	//Методы

	/// Инициализация опций
	void InitOptions();		

public:	
	
	/// Конструктор
	VDetectOptions();
	/// Конструктор
	VDetectOptions(const std::wstring& in_filename);
	/// Деструктор
	~VDetectOptions();	

	
	/// Возвращает true, если работатет режим расшифровки от паспорта
	bool UsePassport() const;
	/// Возвращает true, если учитаывается паспортный износ
	bool UsePassportWear() const;

	/// Установка параметров расшифровки
	void SetUsePassport(bool use);
	void SetUsePassportWear(bool use);


	/// Возвращает true если включена левая просадка
	bool UsePitLeft() const;
	/// Возвращает true если включена правая просадка
	bool UsePitRight() const;
	/// Возвращает true если включена ШК
	bool UseWidth() const;
	/// Возвращает true если включена левая рихтовка
	bool UseRichLeft() const;
	/// Возвращает true если включена правая рихтовка
	bool UseRichRight() const;
	/// Возвращает true если включен уровень
	bool UseLevel() const;
	/// Возвращает true если включен износ левый
	bool UseWearLeft() const;
	/// Возвращает true если включен износ правый
	bool UseWearRight() const;
	/// Возвращает true если включен зазор левый
	bool UseZazorLeft() const;
	/// Возвращает true если включен зазор правый
	bool UseZazorRight() const;
	/// Возвращает true если включен зазор левый
	bool UseMagZazorLeft() const;
	/// Возвращает true если включен зазор правый
	bool UseMagZazorRight() const;

	/// Возвращает true если включена коррекция по км
	bool CoordinatesCorrectionKm() const;
	/// Возвращает true если включена коррекция по кривым
	bool CoordinatesCorrectionCurve() const;
	/// Возвращает true если включена коррекция по RFID
	bool CoordinatesCorrectionRFID() const;
	
	/// Возвращает True если включена коррекция нулевой линии на переходных
	bool CorrectCurves() const;
	
	/// Возвращает порог экспресс оценки уширений
	float EXPRESS_WIDEN_THRESHOLD() const;
	/// Возвращает порог экспресс оценки сужений
	float EXPRESS_NARROW_THRESHOLD() const;
	/// Возвращает порог экспресс оценки износа
	float EXPRESS_WEAR_THRESHOLD() const;
	/// Возвращает порог экспресс оценки зазоров
	float EXPRESS_ZAZOR_THRESHOLD() const;
	
	/// Возвращает true если требуется скрыть паспортные линии()
	bool HIDE_PASSPORT_LINES() const;
	
	/// Возвращает масштаб по координате
	double GetScaleCoord() const;
	/// Возвращает масштаб просадок
	double GetScalePit() const;
	/// Возвращает масштаб ШК
	double GetScaleWidth() const;
	/// Возвращает масштаб рихтовок
	double GetScaleRich() const;
	/// Возвращает масштаб уровня
	double GetScaleLevel() const;
};
typedef VSingleton<VDetectOptions> detect_options_t;
#define DETECT_OPTIONS_INIT(file_name) detect_options_t::Instance().SetObject(VDetectOptions(file_name))
#define DETECT_OPTIONS detect_options_t::Instance().GetObject() 

	