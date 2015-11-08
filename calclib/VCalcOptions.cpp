#include "VCalcOptions.h"

/// Да/Нет
extern const std::wstring OPTION_YES(L"Да");
extern const std::wstring OPTION_NO(L"Нет");

/// Определяем режим расшифровки ЦП-515 оригинальная или с дополнениями
extern const std::wstring OPTION_515(L"ЦП-515");
extern const std::wstring OPTION_EXPERIMENT(L"эксперимент");

/// Определяем режим паспортных данных
extern const std::wstring OPTION_PASSPORT(L"паспорт");
extern const std::wstring OPTION_FACT(L"факт");

///Режимы печати
extern const std::wstring OPTION_PRINT_ALL(L"все");
extern const std::wstring OPTION_PRINT_OVERSTEPS(L"только с отступлениями");
extern const std::wstring OPTION_PRINT_RESTRICTIONS(L"только с ограничениями скорости");

/// Закрытие движения
extern const std::wstring OPTION_CLOSE(L"закрытие");
extern const std::wstring OPTION_RESTRICT_25(L"25 км/ч до натурной проверки");

///Масштабы ленты
extern const std::wstring OPTION_SCALE1(L"основной");
extern const std::wstring OPTION_SCALE2(L"дополнительный 1");
extern const std::wstring OPTION_SCALE3(L"дополнительный 2");

/// Источники ППР
extern const std::wstring OPTION_PPR_PC1(L"ПЭВМ-1");
extern const std::wstring OPTION_PPR_PPR(L"ПЭВМ-ППР");

/// Источники данных для шаблона и износа
extern const std::wstring OPTION_STD(L"стандарт");
extern const std::wstring OPTION_PPR(L"ППР");
extern const std::wstring OPTION_FRONT(L"спереди");
extern const std::wstring OPTION_BACK(L"сзади");

/// Конструктор
VOption::VOption(){};

/// Конструктор
VOption::VOption(const std::wstring& in_name, const std::wstring& in_value, const std::wstring& in_group, 
	const std::wstring& in_description) : 
	name(in_name), value(in_value), default_value(in_value), group(in_group), description(in_description) 
{
	hash=FNV32(name);
};

/// Конструктор
VOption::VOption(const std::wstring& in_name, const std::wstring& in_value, const std::wstring& in_group,
	const std::wstring& in_description, const std::vector<std::wstring>& in_avail_list) : 
	name(in_name), value(in_value), default_value(in_value), group(in_group), description(in_description), avail_list(in_avail_list) 
{
	hash=FNV32(name);
};

/// Конструктор
VOption::VOption(const std::wstring& in_name, const std::wstring& in_value, const std::wstring& in_group,
	const std::wstring& in_description, const std::wstring& in_avail_list) : 
	name(in_name), value(in_value), default_value(in_value), group(in_group), description(in_description)
{
	hash=FNV32(name);
	boost::algorithm::split(avail_list, in_avail_list, boost::is_any_of(L";"));
};

/// Возвращает название
const std::wstring& VOption::GetName() const 
{
	return name;
};

/// Возвращает группу
const std::wstring& VOption::GetGroup() const 
{
	return group;
};

/// Возвращает описание
const std::wstring& VOption::GetDescription() const 
{
	return description;
};

/// Устанавливает значение
void VOption::SetValue(const std::wstring& in_value)
{
	if (GetAvailCount()==0)
		value=in_value;
	else
	{
		std::vector<std::wstring>::iterator i=std::find(avail_list.begin(), avail_list.end(), in_value);
		if (i==avail_list.end())
			value=avail_list.front();
		else
			value=in_value;
	}
};

/// Устанавливает значение по умолчанию		
void VOption::ResetValue()
{
	value=default_value;	
};

/// Возвращает true, если значение опции соответсвует входному
bool VOption::Compare(const std::wstring& in_value) const 
{
	return value.compare(in_value)==0;
};

/// Добавляет доступное значение
void VOption::AddAvail(const std::wstring& in_avail)
{
	avail_list.push_back(in_avail);
};

/// Возвращает число доступных значений
size_t VOption::GetAvailCount() const 
{
	return avail_list.size();
};

/// Возвращает доступное значение
const std::wstring& VOption::GetAvail(size_t in_index) const 
{
	return avail_list.at(in_index);
};

/// Возвращает хэш
uint32_t VOption::GetHash() const
{
	return hash;
}

//------------------------------- Методы класса VOptions -------------------------------------------

/** Загрузка данных*/
void VOptions::Load(const std::wstring& in_filename)
{
	filename=in_filename;
	std::ifstream file(filename.c_str());
	if (file.is_open())
	{
		do
		{			
			//Описание
			std::strstreambuf _description;
			file.get(_description);							
			std::string description=std::string(_description.str(), file.gcount());
			file.get();
			//Значение
			std::strstreambuf _value;
			file.get(_value);							
			std::string value=std::string(_value.str(), file.gcount());
			file.get();
			if (value.compare("")!=0)
			{
				std::vector<std::string> parts;
				boost::algorithm::split(parts, value, boost::is_any_of("="));
				if (HasOption(string_to_wstring(parts.front())))
				{
					const VOption oldopt = GetOption(string_to_wstring(parts.front()));
					VOption newopt(oldopt);
					newopt.SetValue(string_to_wstring(parts.back()));
					options.erase(oldopt);
					options.insert(newopt);
					//GetOption(string_to_wstring(parts.front())).SetValue(string_to_wstring(parts.back()));
				}
			}
		} while (!file.eof());
	}
};

/** Запись опций*/
void VOptions::Save()
{	
	std::ofstream file(filename.c_str());
	for (std::set<VOption>::iterator i=options.begin(); i!=options.end(); ++i)
	{
		VOption _opt(*i);
		file << _opt;
	}
};

/** Возвращает число переменных*/
size_t VOptions::GetOptionsCount() const 
{
	return options.size();
};

/** Возвращает переменную по индексу*/
const VOption& VOptions::GetOption(size_t in_index)
{
	size_t j=0;
	std::set<VOption>::iterator i=options.begin();
	while (j++<in_index)
		++i;
	return *i;
};	

/** Возвращает true если опция присутствует в списке*/
bool VOptions::HasOption(const std::wstring& in_name) const
{
	uint32_t hash=FNV32(in_name);
	std::set<VOption>::const_iterator i=std::find_if(options.begin(), options.end(), find_option_by_hash(hash));
	return i!=options.end();			
};

/** Возвращает true если опция присутствует в списке*/
bool VOptions::HasOption(const std::wstring& in_group, const std::wstring& in_description) const
{
	std::set<VOption>::const_iterator i=std::find_if(options.begin(), options.end(), find_option_by_description(in_group, in_description));
	return i!=options.end();			
};

/** Возвращает переменную по ее имени*/	
const VOption& VOptions::GetOption(const std::wstring& in_name)
{			
	uint32_t hash=FNV32(in_name);
	std::set<VOption>::iterator i=std::find_if(options.begin(), options.end(), find_option_by_hash(hash));
	if (i!=options.end())
		return *i;
	else
		throw VSimpleException(L"Запрошенная переменная отсутствует в списке.", in_name);
};

/** Возвращает переменную по группе и описанию*/
const  VOption& VOptions::GetOption(const std::wstring& in_group, const std::wstring& in_description)
{
	std::set<VOption>::iterator i=std::find_if(options.begin(), options.end(), find_option_by_description(in_group, in_description));
	if (i!=options.end())
		return *i;
	else
		throw VSimpleException(L"Запрошенная переменная отсутствует в списке.", in_description);
};

/** Возвращает переменную по ее имени*/	
const VOption& VOptions::GetOption(const std::wstring& in_name) const
{	
	uint32_t hash=FNV32(in_name);
	std::set<VOption>::const_iterator i=std::find_if(options.begin(), options.end(), find_option_by_hash(hash));
	if (i!=options.end())
		return *i;
	else
		throw VSimpleException(L"Запрошенная переменная отсутствует в списке.", in_name);
};

/** Возвращает переменную по группе и описанию*/
const VOption& VOptions::GetOption(const std::wstring& in_group, const std::wstring& in_description) const
{
	std::set<VOption>::const_iterator i=std::find_if(options.begin(), options.end(), find_option_by_description(in_group, in_description));
	if (i!=options.end())
		return *i;
	else
		throw VSimpleException(L"Запрошенная переменная отсутствует в списке.", in_description);
};

/** Возвращает число групп*/
size_t VOptions::GetGroupCount() const 
{
	return groups.size();
};

/** Возвращает группу*/
const std::wstring& VOptions::GetGroup(size_t in_index) const 
{
	return groups.at(in_index);
};

//------------------------------ Методы класса VCalcOptions -----------------------------------------

/** Конструктор*/
VCalcOptions::VCalcOptions()
{
	InitOptions();
};

/** Конструктор*/
VCalcOptions::VCalcOptions(const std::wstring& in_filename)
{
	InitOptions();
	Load(in_filename);
};

/** Деструктор*/
VCalcOptions::~VCalcOptions()
{
	Save();
};

/** Инициализация опций*/
void VCalcOptions::InitOptions()
{		
	//Кривые
	groups.push_back(L"План");
	
	options.insert(VOption(L"MIN_CURVE_ANGLE", L"40", 
		L"План", L"минимальный угол поворота кривой (мин.)"));
	options.insert(VOption(L"MIN_CURVE_LENGTH", L"30", 
		L"План", L"минимальная длина кривой (м)"));
	options.insert(VOption(L"MAX_PLAN_OFFSET", L"15", 
		L"План", L"максимальное отклонение от паспорта (см)"));
	options.insert(VOption(L"MIN_LENGTH_TRANS", L"5", 
		L"План", L"минимальная длина переходной (м)"));
	options.insert(VOption(L"MIN_LENGTH_CURVE", L"5", 
		L"План", L"минимальная длина круговой (м)"));
	options.insert(VOption(L"BEZIER_ON_PLAN", OPTION_YES, 
		L"План", L"сглаживать переломы кривыми Безье")[OPTION_YES][OPTION_NO]);
	options.insert(VOption(L"MIN_LEVEL", L"5", 
		L"План", L"минимальное возвышение на кривой (мм)"));
	options.insert(VOption(L"JAGGY_LINES_ONLY", OPTION_NO, 
		L"План", L"вычислять неровности только на прямых")[OPTION_YES][OPTION_NO]);
	options.insert(VOption(L"LEVEL_QUANTUM", L"5", 
		L"План", L"шаг изменения возвышения (мм)"));
//	options.insert(VOption(L"CURVE_ANP_RANGES", OPTION_46_15,
//		L"План", L"пороги непогашенного ускорения")[OPTION_46_15][OPTION_EXPERIMENT]);
	//Профиль
	groups.push_back(L"Профиль");
	options.insert(VOption(L"MIN_PROFILE_ELEMENT_LENGTH", L"200", 
		L"Профиль", L"минимальная длина элемента (м)"));
	options.insert(VOption(L"MAX_PROFILE_OFFSET", L"150", 
		L"Профиль", L"максимальное отклонение от паспорта (мм)"));	
	options.insert(VOption(L"DROP_MARKS_ON_PROFILE", OPTION_YES, 
		L"Профиль", L"привязывать метки к профилю")[OPTION_YES][OPTION_NO]);
	options.insert(VOption(L"BEZIER_ON_PROFILE", OPTION_YES, 
		L"Профиль", L"сглаживать переломы кривыми Безье")[OPTION_YES][OPTION_NO]);
	options.insert(VOption(L"MAX_DISTANCE_BTW_MARKS", L"5000", 
		L"Профиль", L"максимальное расстояние между опорными отметками профиля (м)"));
	//Усреднение
	groups.push_back(L"Расчет средних");
	options.insert(VOption(L"LEVEL_MEAN_BASE", L"48", 
		L"Расчет средних", L"база расчета среднего возвышения (м)"));
	options.insert(VOption(L"CURV_MEAN_BASE", L"48", 
		L"Расчет средних", L"база расчета средней кривизны (м)"));
	options.insert(VOption(L"WIDTH_MEAN_BASE", L"48", 
		L"Расчет средних", L"база расчета средней ШК (м)"));
	//Отжатия
	groups.push_back(L"Отжатия");
	options.insert(VOption(L"MAX_DIFF_WIDTH", L"3", 
		L"Отжатия", L"максимальная разность ШК с нагрузкой и без (мм)"));
	//Карточки кривых
	groups.push_back(L"Карточки кривых");
	options.insert(VOption(L"CURVE_CARD_SHOW_CURV_MEAN", OPTION_YES, 
		L"Карточки кривых", L"выводить среднюю кривизну на графике")[OPTION_YES][OPTION_NO]);
	options.insert(VOption(L"CURVE_CARD_SHOW_LEVEL_MEAN", OPTION_YES, 
		L"Карточки кривых", L"выводить средний уровень на графике")[OPTION_YES][OPTION_NO]);
	options.insert(VOption(L"CURVE_CARD_SHOW_CURV", OPTION_NO, 
		L"Карточки кривых", L"выводить кривизну на графике")[OPTION_YES][OPTION_NO]);
	options.insert(VOption(L"CURVE_CARD_SHOW_LEVEL", OPTION_NO, 
		L"Карточки кривых", L"выводить уровень на графике")[OPTION_YES][OPTION_NO]);		
	//Износ
	groups.push_back(L"Износ");
	options.insert(VOption(L"USE_NTD", OPTION_YES, 
		L"Износ", L"Учитывать НТД при оценке износа")[OPTION_YES][OPTION_NO]);
	options.insert(VOption(L"EXT_WEAR_THRESHOLD", L"15", 
		L"Износ", L"Порог износа нерабочей грани рельса"));	
	options.insert(VOption(L"WEAR_SOURCE", OPTION_STD,
		L"Износ", L"Источник износов")[OPTION_STD][OPTION_PPR]);
	//Пересчёт файла			
	groups.push_back(L"Пересчёт файла");	
	options.insert(VOption(L"PLAN_MARKS_COORD_CORRECTION", OPTION_NO, 
		L"Пересчёт файла", L"корректировать разметку кривых при пересчёте координаты")[OPTION_YES][OPTION_NO]);	
	options.insert(VOption(L"PROFILE_MARKS_COORD_CORRECTION", OPTION_NO,
		L"Пересчёт файла", L"корректировать разметку профиля при пересчёте координаты")[OPTION_YES][OPTION_NO]);
	//Перешифровка
	groups.push_back(L"Перешифровка");
	options.insert(VOption(L"ZEROLINE_ON_FILE_CURVE_MARKS", OPTION_YES,
		L"Перешифровка", L"строить нулевую линию по кривым из файла")[OPTION_YES][OPTION_NO]);
	//Координаты фото-зазоров
	groups.push_back(L"Энумерация фото-зазоров");
	options.insert(VOption(L"COORD_SHIFT_VSPD_PLUS", L"0", L"Энумерация фото-зазоров",
	    L"Подвижка координаты при положительной скорости вагона (м)"));
	options.insert(VOption(L"COORD_SHIFT_VSPD_MINUS", L"0", L"Энумерация фото-зазоров",
	    L"Подвижка координаты при отрицательной скорости вагона (м)"));
}

//----------------------------------------- Методы класса VDetectOptions --------------------------------------

/** Инициализация опций*/
void VDetectOptions::InitOptions()
{				
	groups.push_back(L"Режимы расшифровки");
		options.insert(VOption(L"INSTRUCTION", OPTION_515, 
			L"Режимы расшифровки", L"Инструкция")[OPTION_515][OPTION_EXPERIMENT]);
		options.insert(VOption(L"CURVE_INSTRUCTION", OPTION_YES, 
			L"Режимы расшифровки", L"Оценка кривых")[OPTION_YES][OPTION_NO]);			
		options.insert(VOption(L"PASSPORT", OPTION_PASSPORT, 
			L"Режимы расшифровки", L"Нулевая линия")[OPTION_PASSPORT][OPTION_FACT]);
		options.insert(VOption(L"WEAR", OPTION_PASSPORT, 
			L"Режимы расшифровки", L"Износ в кривых")[OPTION_PASSPORT][OPTION_FACT]);
		options.insert(VOption(L"DISABLE_ALL_DETECTORS", OPTION_NO,
			L"Режимы расшифровки", L"Отключить оценку всех отступлений")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"ADJUST_ZERO_LINES", OPTION_YES,
			L"Режимы расшифровки", L"Уточнять нулевую линию по факту")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"DISABLE_WEAR_DETECTOR_ON_RADIUS_3000", OPTION_YES,
			L"Режимы расшифровки", L"Не оценивать износ на кривых радиусом более 3000 м")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"LINE_CLOSURE_POLICY", OPTION_CLOSE,
			L"Режимы расшифровки", L"Закрытие движения")[OPTION_CLOSE][OPTION_RESTRICT_25]);
	groups.push_back(L"Графическая диаграмма");			
		options.insert(VOption(L"HIDE_PASSPORT_LINE", OPTION_NO,
			L"Графическая диаграмма", L"Скрыть паспортную линию при поездке по факту")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"PRINT_POLICY_GEOMETRY", OPTION_PRINT_ALL,
			L"Графическая диаграмма", L"Печатать ленты ГРК")[OPTION_PRINT_ALL][OPTION_PRINT_OVERSTEPS][OPTION_PRINT_RESTRICTIONS]);
		options.insert(VOption(L"PRINT_POLICY_EXTRA", OPTION_PRINT_ALL,
			L"Графическая диаграмма", L"Печатать ленты доп. параметров")[OPTION_PRINT_ALL][OPTION_PRINT_OVERSTEPS][OPTION_PRINT_RESTRICTIONS]);
		options.insert(VOption(L"CUT_DISTANTIONS", OPTION_NO,
			L"Графическая диаграмма", L"Разрыв страницы при смене дистанций")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"ROUND_RADIUS_AND_LEVEL", OPTION_NO,
			L"Графическая диаграмма", L"Округлять радиусы и возвышения")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"SHOW_PASSPORT_WEAR", OPTION_NO,
			L"Графическая диаграмма", L"Показывать график паспортного износа")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"SHOW_FACT_WEAR", OPTION_NO,
			L"Графическая диаграмма", L"Показывать график фактического износа на ленте ГРК")[OPTION_YES][OPTION_NO]);			
		options.insert(VOption(L"SCALE", OPTION_SCALE1,
			L"Графическая диаграмма", L"Масштаб")[OPTION_SCALE1][OPTION_SCALE2][OPTION_SCALE3]);
		options.insert(VOption(L"SPLIT_DIAGRAM", OPTION_NO,
		    L"Графическая диаграмма", L"Сохранять каждый километр в отдельный файл")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"SAVE_GEOMETRY", OPTION_YES,
		    L"Графическая диаграмма", L"Сохранять в файл диаграмму ГРК")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"SAVE_EXTRA", OPTION_YES,
		    L"Графическая диаграмма", L"Сохранять в файл диаграмму доп. параметров")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"LEFT_MARGIN", L"20",
			L"Графическая диаграмма", L"Отступ слева (мм)")[L"20"][L"15"][L"10"]);		    
	groups.push_back(L"Каналы измерений");
		options.insert(VOption(L"USE_PIT_LEFT", OPTION_YES, 
			L"Каналы измерений", L"Просадка левая")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_PIT_RIGHT", OPTION_YES, 
			L"Каналы измерений", L"Просадка правая")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_WIDTH", OPTION_YES, 
			L"Каналы измерений", L"ШК")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_RICH_LEFT", OPTION_YES, 
			L"Каналы измерений", L"Рихтовка левая")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_RICH_RIGHT", OPTION_YES, 
			L"Каналы измерений", L"Рихтовка правая")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_LEVEL", OPTION_YES, 
			L"Каналы измерений", L"Уровень")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_WEAR_LEFT", OPTION_YES, 
			L"Каналы измерений", L"Износ левый")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_WEAR_RIGHT", OPTION_YES, 
			L"Каналы измерений", L"Износ правый")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_ZAZOR_LEFT", OPTION_YES, 
			L"Каналы измерений", L"Зазор левый")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_ZAZOR_RIGHT", OPTION_YES, 
			L"Каналы измерений", L"Зазор правый")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_MAG_ZAZOR_LEFT", OPTION_YES, 
			L"Каналы измерений", L"Зазор левый (магнитный)")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_MAG_ZAZOR_RIGHT", OPTION_YES, 
			L"Каналы измерений", L"Зазор правый (магнитный)")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"WIDTH_SOURCE", OPTION_STD, 
			L"Каналы измерений", L"Источник ШК")[OPTION_STD][OPTION_FRONT][OPTION_BACK][OPTION_PPR]);
		options.insert(VOption(L"PPR_SOURCE", OPTION_PPR_PC1,
			L"Каналы измерений", L"Источник ППР")[OPTION_PPR_PC1][OPTION_PPR_PPR]);
		options.insert(VOption(L"WEAR_SOURCE", OPTION_STD,
			L"Каналы измерений", L"Источник износов для расшифровки")[OPTION_STD][OPTION_PPR]);
		options.insert(VOption(L"COPY_PPR_TO_WEAR", OPTION_NO,
			L"Каналы измерений", L"Копировать износы из ППР в основные каналы")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_RFID", OPTION_YES, 
			L"Каналы измерений", L"RFID метки")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_PPR", OPTION_YES, 
			L"Каналы измерений", L"ППР")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"CALCULATE_ZERO_ZAZOR", OPTION_YES, 
			L"Каналы измерений", L"Определять нулевые зазоры")[OPTION_YES][OPTION_NO]);
	groups.push_back(L"Привязка пикетажа");
		options.insert(VOption(L"ENABLE_COORDINATES_CORRECTION", OPTION_YES, 
			L"Привязка пикетажа", L"Включить привязку пикетажа")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"COORDINATES_CORRECTION_KM", OPTION_YES, 
			L"Привязка пикетажа", L"Километры")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"COORDINATES_CORRECTION_CURVE", OPTION_YES, 
			L"Привязка пикетажа", L"Кривые")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"COORDINATES_CORRECTION_RFID", OPTION_NO, 
			L"Привязка пикетажа", L"Метки RFID")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"MAX_SWITCH_CORRECTION_DISTANCE", L"30", 
			L"Привязка пикетажа", L"Порог идентификации стрелки (м)"));
		options.insert(VOption(L"MARKER_COUNT", L"18", 
			L"Привязка пикетажа", L"Минимальное число маркеров для привязки"));		
		options.insert(VOption(L"SAVE_CORRECTED_COORDINATE", OPTION_NO, 
			L"Привязка пикетажа", L"Сохранять скорректированную координату")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"DETECT_SIDEWAYS", OPTION_NO, 
			L"Привязка пикетажа", L"Распознавание съездов на боковые пути")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"COORDINATES_CORRECTION_MARKERS", OPTION_PASSPORT, 
			L"Привязка пикетажа", L"Источник маркеров для привязки")[OPTION_FACT][OPTION_PASSPORT]);
		options.insert(VOption(L"USE_SAVED_CORRECTION_SLOPE", OPTION_YES, 
			L"Привязка пикетажа", L"Использовать поправку из предыдущего проезда")[OPTION_YES][OPTION_NO]);			
	groups.push_back(L"Экспресс оценка");					
		options.insert(VOption(L"EXPRESS_WIDEN_THRESHOLD", L"1548.0", 
			L"Экспресс оценка", L"Порог уширений"));
		options.insert(VOption(L"EXPRESS_NARROW_THRESHOLD", L"1510.0", 
			L"Экспресс оценка", L"Порог сужений"));
		options.insert(VOption(L"EXPRESS_WEAR_THRESHOLD", L"18.0", 
			L"Экспресс оценка", L"Порог износа"));
		options.insert(VOption(L"EXPRESS_ZAZOR_THRESHOLD", L"26.0", 
			L"Экспресс оценка", L"Порог зазора"));
	groups.push_back(L"Оценка доп. параметров");
		options.insert(VOption(L"DETECT_WEAR", OPTION_YES, 
			L"Оценка доп. параметров", L"Оценка износа")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"DETECT_ZAZOR", OPTION_YES, 
			L"Оценка доп. параметров", L"Оценка зазоров")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"DETECT_ZERO_ZAZOR", OPTION_YES, 
			L"Оценка доп. параметров", L"Оценка слитых зазоров")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_NTD", OPTION_YES, 
			L"Оценка доп. параметров", L"Учитывать НТД при оценке износа")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_283p", OPTION_YES, 
			L"Оценка доп. параметров", L"Учитывать распоряжение 283р при оценке износа")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"USE_P50_CORRECTION", OPTION_YES, 
			L"Оценка доп. параметров", L"Вносить поправку в величину износа на рельсах р50")[OPTION_YES][OPTION_NO]);
	groups.push_back(L"Контрольные участки");
		options.insert(VOption(L"SHOW_CONTROL_SECTION_SKO", OPTION_NO, 
			L"Контрольные участки", L"Оценивать СКО на контрольных участках")[OPTION_YES][OPTION_NO]);
	groups.push_back(L"Пересчет рихтовки");
		options.insert(VOption(L"RICH_RECALCULATION", OPTION_NO, 
			L"Пересчет рихтовки", L"Включить пересчет рихтовки")[OPTION_YES][OPTION_NO]);
		options.insert(VOption(L"FRONT_ARM_REAL", L"11.48", 
			L"Пересчет рихтовки", L"Переднее плечо хорды (вагон) (м)"));
		options.insert(VOption(L"BACK_ARM_REAL", L"4.55", 
			L"Пересчет рихтовки", L"Заднее плечо хорды (вагон) (м)"));
		options.insert(VOption(L"FRONT_ARM_VIRTUAL", L"17.4", 
			L"Пересчет рихтовки", L"Переднее плечо хорды (пересчет) (м)"));
		options.insert(VOption(L"BACK_ARM_VIRTUAL", L"4.1", 
			L"Пересчет рихтовки", L"Заднее плечо хорды (пересчет) (м)"));
		options.insert(VOption(L"RICH_RECALCULATION_PRECISION", L"2.5", 
			L"Пересчет рихтовки", L"Точность пересчета (мм)"));
		options.insert(VOption(L"RICH_RECALCULATION_BUFFER", L"100.0", 
			L"Пересчет рихтовки", L"Длина буфера (м)"));		
	groups.push_back(L"Отбраковка сбоев аппаратуры");
		options.insert(VOption(L"WIDTH_CORRUPT_DETECTION", OPTION_NO, 
			L"Отбраковка сбоев аппаратуры", L"Отбраковка сбоев ШК в зоне уравнительных приборов и стрелок")[OPTION_YES][OPTION_NO]);
	groups.push_back(L"Параметры ВПС");
    	options.insert(VOption(L"VPS", L"", L"Параметры ВПС", L"Название ВПС"));
	    options.insert(VOption(L"CHIEF", L"", L"Параметры ВПС", L"Начальник смены"));
	    options.insert(VOption(L"APPEND_VPS_TO_FILENAME", OPTION_YES, L"Параметры ВПС",
	        L"Добавлять название ВПС к имени файла")[OPTION_YES][OPTION_NO]);
}

/** Конструктор*/
VDetectOptions::VDetectOptions()
{
	InitOptions();		
}

/** Конструктор*/
VDetectOptions::VDetectOptions(const std::wstring& in_filename)
{
	InitOptions();
	Load(in_filename);
}

/** Деструктор*/
VDetectOptions::~VDetectOptions()
{		
	Save();
}

/// Возвращает true, если работатет режим расшифровки от паспорта
bool VDetectOptions::UsePassport() const
{
	return GetOption(L"PASSPORT").Compare(OPTION_PASSPORT);
}

/// Возвращает true, если учитаывается паспортный износ
bool VDetectOptions::UsePassportWear() const
{
	return GetOption(L"WEAR").Compare(OPTION_PASSPORT);
}

/// Устанавливает опцию "строить нулевую линию по паспорту"
void VDetectOptions::SetUsePassport(bool use)
{
	const VOption oldopt = GetOption(L"PASSPORT");
	VOption newopt(oldopt);
	newopt.SetValue(use ? OPTION_PASSPORT : OPTION_FACT);
	options.erase(oldopt);
	options.insert(newopt);
	//GetOption(L"PASSPORT").SetValue(use ? OPTION_PASSPORT : OPTION_FACT);	
}

void VDetectOptions::SetUsePassportWear(bool use)
{
	const VOption oldopt = GetOption(L"WEAR");
	VOption newopt(oldopt);
	newopt.SetValue(use ? OPTION_PASSPORT : OPTION_FACT);
	options.erase(oldopt);
	options.insert(newopt);
	//GetOption(L"WEAR").SetValue(use ? OPTION_PASSPORT : OPTION_FACT);	
}


/// Возвращает true если включена левая просадка
bool VDetectOptions::UsePitLeft() const
{
	return GetOption(L"USE_PIT_LEFT").Compare(OPTION_YES);
}

/// Возвращает true если включена правая просадка
bool VDetectOptions::UsePitRight() const
{
	return GetOption(L"USE_PIT_RIGHT").Compare(OPTION_YES);
}

/// Возвращает true если включена ШК
bool VDetectOptions::UseWidth() const
{
	return GetOption(L"USE_WIDTH").Compare(OPTION_YES);
}

/// Возвращает true если включена левая рихтовка
bool VDetectOptions::UseRichLeft() const
{
	return GetOption(L"USE_RICH_LEFT").Compare(OPTION_YES);
}

/// Возвращает true если включена правая рихтовка
bool VDetectOptions::UseRichRight() const
{
	return GetOption(L"USE_RICH_RIGHT").Compare(OPTION_YES);
}

/// Возвращает true если включен уровень
bool VDetectOptions::UseLevel() const
{
	return GetOption(L"USE_LEVEL").Compare(OPTION_YES);	
}

/// Возвращает true если включен износ левый
bool VDetectOptions::UseWearLeft() const
{
	return GetOption(L"USE_WEAR_LEFT").Compare(OPTION_YES);
}

/// Возвращает true если включен износ правый
bool VDetectOptions::UseWearRight() const
{
	return GetOption(L"USE_WEAR_RIGHT").Compare(OPTION_YES);
}

/// Возвращает true если включен зазор левый
bool VDetectOptions::UseZazorLeft() const
{
	return GetOption(L"USE_ZAZOR_LEFT").Compare(OPTION_YES);
}

/// Возвращает true если включен зазор правый
bool VDetectOptions::UseZazorRight() const
{
	return GetOption(L"USE_ZAZOR_RIGHT").Compare(OPTION_YES);
}

/// Возвращает true если включен зазор левый
bool VDetectOptions::UseMagZazorLeft() const
{
	return GetOption(L"USE_MAG_ZAZOR_LEFT").Compare(OPTION_YES);
}

/// Возвращает true если включен зазор правый
bool VDetectOptions::UseMagZazorRight() const
{
	return GetOption(L"USE_MAG_ZAZOR_RIGHT").Compare(OPTION_YES);
}

/// Возвращает true если включена коррекция по км
bool VDetectOptions::CoordinatesCorrectionKm() const
{
	return GetOption(L"COORDINATES_CORRECTION_KM").Compare(OPTION_YES);
}

/// Возвращает true если включена коррекция по кривым
bool VDetectOptions::CoordinatesCorrectionCurve() const
{
	return !GetOption(L"COORDINATES_CORRECTION_CURVE").Compare(OPTION_NO);
}

// Возвращает true если включена коррекция по RFID
bool VDetectOptions::CoordinatesCorrectionRFID() const
{
	return !GetOption(L"COORDINATES_CORRECTION_RFID").Compare(OPTION_NO);
}

/// Возвращает True если включена коррекция нулевой линии на переходных
bool VDetectOptions::CorrectCurves() const
{
	return GetOption(L"ADJUST_ZERO_LINES").Compare(OPTION_YES);
}

/// Возвращает порог экспресс оценки уширений
float VDetectOptions::EXPRESS_WIDEN_THRESHOLD() const
{
	return GetOption(L"EXPRESS_WIDEN_THRESHOLD").GetValue<float>();
}

/// Возвращает порог экспресс оценки сужений
float VDetectOptions::EXPRESS_NARROW_THRESHOLD() const
{
	return GetOption(L"EXPRESS_NARROW_THRESHOLD").GetValue<float>();
}

/// Возвращает порог экспресс оценки износа
float VDetectOptions::EXPRESS_WEAR_THRESHOLD() const
{
	return GetOption(L"EXPRESS_WEAR_THRESHOLD").GetValue<float>();
}

/// Возвращает порог экспресс оценки зазоров
float VDetectOptions::EXPRESS_ZAZOR_THRESHOLD() const
{
	return GetOption(L"EXPRESS_ZAZOR_THRESHOLD").GetValue<float>();
}

/// Возвращает true если требуется скрыть паспортные линии()
bool VDetectOptions::HIDE_PASSPORT_LINES() const
{
	return !UsePassport() && GetOption(L"HIDE_PASSPORT_LINE").Compare(OPTION_YES);
}

/// Возвращает масштаб по координате
double VDetectOptions::GetScaleCoord() const
{
	VOption option=GetOption(L"SCALE");
	if (option.Compare(OPTION_SCALE3))
	{
		return 40;
	}
	else if (option.Compare(OPTION_SCALE2))
	{
		return 20;
	}
	else
	{
		return 40;
	}
}

/// Возвращает масштаб просадок
double VDetectOptions::GetScalePit() const
{
	VOption option=GetOption(L"SCALE");
	if (option.Compare(OPTION_SCALE3))
	{
		return 10;
	}
	else if (option.Compare(OPTION_SCALE2))
	{
		return 10;
	}
	else
	{
		return 20;
	}
}

/// Возвращает масштаб ШК
double VDetectOptions::GetScaleWidth() const
{
	VOption option=GetOption(L"SCALE");
	if (option.Compare(OPTION_SCALE3))
	{
		return 10; 
	}
	else if (option.Compare(OPTION_SCALE2))
	{
		return 10;
	}
	else
	{
		return 20;
	}
}

/// Возвращает масштаб рихтовок
double VDetectOptions::GetScaleRich() const
{
	VOption option=GetOption(L"SCALE");
	if (option.Compare(OPTION_SCALE3))
	{
		return 20;
	}
	else if (option.Compare(OPTION_SCALE2))
	{
		return 20;
	}
	else
	{
		return 40;
	}
}

/// Возвращает масштаб уровня
double VDetectOptions::GetScaleLevel() const
{
	VOption option=GetOption(L"SCALE");
	if (option.Compare(OPTION_SCALE3))
	{
		return 40;
	}
	else if (option.Compare(OPTION_SCALE2))
	{
		return 20;
	}
	else
	{
		return 40;
	}
}