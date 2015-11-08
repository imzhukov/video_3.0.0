#pragma once

#include "datatypes.h"
#include "VFileName.h"

// ---------------------------------------------------------------------------------
/// загружаемые параметры
// ---------------------------------------------------------------------------------

enum PARAMETER_TYPE
{
	PRM_NONE, // нет параметра (0)
	PRM_APRINFO, // априорная информация из файла 
	PRM_ABS_COORD, // абсолютная координата 
	PRM_WAY_COORD, // путевая координтата (устарело) 
	PRM_CORRUPT, // сбои измерений 
	PRM_HAND_CORRUPT, // ручная отбраковка измерений 
	PRM_SPEED, // скорость вагона 
	PRM_COMMENT, // комментарии
	PRM_DPP, // счетчик тиков ДПП
	PRM_KADR_COUNT, // счетчик числа кадров (устарело)
	PRM_OVRS2, // отступления - расшифровка версии 2
	PRM_OVRS3, // отступления - расшифровка версии 3
	PRM_OP_MARKS, // метки оператора
	PRM_MAG_ZAZOR, // датчик магнитных зазоров - канал без измерений
	PRM_UGON_LEFT, // угон плетей, левый
	PRM_UGON_RIGHT, // угон плетей, правый
	PRM_SPALA_MARKER, // RF метки шпал
	PRM_LINE_MARKS, // разметка прямых
	PRM_SIDEWAYS, // боковые пути
	PRM_HISTORY, // история операций
	PRM_CURVE_MARKS, // разметка кривых
	PRM_MODEL_CURVES, // разметка моделей кривых
	PRM_PROF_MARKS, // разметка профиля
	PRM_CONTROL_AREA, // контрольные участки
	PRM_KURS, // курсовой угол
	PRM_KURS_BACKUP, // резервная копия курса
	PRM_GRADIENT, // уклон
	PRM_LEVEL, // уровень
	PRM_CURV, // кривизна
	PRM_WIDTH, // ширина колеи
	PRM_WIDTH_FRONT, // ширина колеи спереди вагона
	PRM_WIDTH_BACK, // ширина колеи сзади вагона
	PRM_PIT_LEFT, // провадка правая
	PRM_PIT_RIGHT, // просадка левая
	PRM_DIST_SHORT, // перекос на базе тележки
	PRM_DIST_LONG, // перекос на базе кузова
	PRM_RICH_LEFT, // рихтовка левая 
	PRM_RICH_RIGHT, //  рихтовка правая
	PRM_TEMP_LEFT, // температура левая
	PRM_TEMP_RIGHT,  // температура правая
	PRM_ZAZOR_LEFT, // зазор левый
	PRM_ZAZOR_RIGHT, // зазор правый
	PRM_MAG_ZAZOR_LEFT, // магнитный зазор левый - измерения
	PRM_MAG_ZAZOR_RIGHT, // магнитный зазор правый - измерений
	PRM_WEAR_LEFT, // износ левой 
	PRM_WEAR_RIGHT, // износ правый
	PRM_UNI_WEAR_SENSOR, // объединенный износ от ПЭВМ-1
	PRM_USK_VERT, // ускорение вертикальное
	PRM_USK_HOR, // ускорение горизонтальное
	PRM_LATITUDE, // широта гироскопная
	PRM_LONGITUDE, // долгота гироскопная
	PRM_SHORT_GRADIENT, // короткий уклон
	PRM_KN_LEFT, // короткие неровности
	PRM_KN_RIGHT, // короткие неровности
	PRM_GPS_TIME, // метка времени GPS
	PRM_GPS_LATITUDE, // широта
	PRM_GPS_LONGITUDE,  // долгота
	PRM_GPS_ALTITUDE, // возвышение
	PRM_TEST_4, 
	PRM_TEST_7,
	PRM_TEST_8, 
	PRM_PPR_WAYPASSED, // отметка пройденного пути 
	PRM_PPR_WIDTH, // ШК 
	PRM_PPR_NAK_LEFT, // наклон левый
	PRM_PPR_NAK_RIGHT, // наклон правый
	PRM_PPR_POD_LEFT, // подуклонка левая
	PRM_PPR_POD_RIGHT, // подуклонка правая
	PRM_PPR_IZN_LEFT, // износ объединенный левый
	PRM_PPR_IZN_RIGHT, // износ объединенный правый
	PRM_PPR_IZN_EXT_LEFT, // износ внешний левый
	PRM_PPR_IZN_EXT_RIGHT, // износ внешний правый
	PRM_PPR_IZN_INT_LEFT, // износ внутренний левый
	PRM_PPR_IZN_INT_RIGHT, // износ внутренний правый
	PRM_MID_LEVEL, // средний уровень
	PRM_MID_CURV, // средняя кривизна
	PRM_MID_WIDTH, // средняя ширина колеи
	PRM_MID_RICH_LEFT, // средняя рихтовка левая
	PRM_MID_RICH_RIGHT, // средняя рихтовка правая
	PRM_PROFILE, // профиль
	PRM_UNI_WEAR,  // объединенный износ
	PRM_STATISTICS, // статитика
	PRM_PLAN, // план
	PRM_IDEAL_PLAN, // идеальный план
	PRM_PLAN_ELEM_LIST, // список элементов плана
	PRM_DIFF_PROFILE, // отклонения от идеального профиля
	PRM_MODEL_IDEAL_PLAN, // идеальный план по модельной разметке
	PRM_PLAN_OFFSET,	// отклонение от идеального плана
	PRM_PLAN_OFFSET_FICTIONAL, // отклонения от идеального плана без учета разметки прямых
	PRM_MODEL_PLAN_ELEM_LIST, // список элементов плана по модельной разметке
	PRM_ANP_REAL_PASS, 
	PRM_ANP_REAL_TRUCK,
	PRM_ANP_MODEL_PSP_PASS,
	PRM_ANP_MODEL_PSP_TRUCK,
	PRM_JAGGY_PROFILE, // неровности профиля
	PRM_LEVEL_ZLINE,   // нулевая линия на уровне
	PRM_PROFILE_SLOPE_ELEMENT, // поэлементный уклон профиля
	PRM_PROFILE_SLOPE_KM,  // уклон профиля на километре
	PRM_PROFILE_SLOPE_PK, // уклон профиля на пикете
	PRM_PROFILE_SLOPE_HPK, // уклон профиля на половине пикета
	PRM_PSI_REAL_PASS, //скорость нарастания непогашенного ускорения (факт. пасс.)
	PRM_PSI_REAL_TRUCK, //скорость нарастания непогашенного ускорения (факт. пасс.)
	PRM_MODEL_PLAN_OFFSET,	// отклонение от идеального плана (пасп.)
	PRM_LINES, // прямые (из кривых и боковых путей)
	PRM_JAGGY_PLAN, // неровности в плане	
	PRM_APROX_PROFILE, //Идеальный профиль
	PRM_CURV_FROM_KURS, //кривизна, расчитанная из курсового угла	
	PRM_DIFF_LEVEL, // отклонения уровня от разметки
	PRM_DIFF_LEVEL_MODEL, // отклонения уровня от разметки модели
	PRM_JAGGY_PROFILE_OVERSTEP, // сверхнормативные неровности профиля
	PRM_JAGGY_PLAN_OVERSTEP, // сверхнормативные неровности плана
	PRM_DIFF_WIDTH_FRONT, // разность ШК с нагрузкой и без
	PRM_DIFF_WIDTH_FRONT_OVERSTEP, //отжатия
	PRM_DIFF_WIDTH_BACK, // разность ШК с нагрузкой и без
	PRM_DIFF_WIDTH_BACK_OVERSTEP, //отжатия
	PRM_CANTING_LEFT_OVERSTEP, //подуклонка левая
	PRM_CANTING_RIGHT_OVERSTEP, //подуклонка правая
	PRM_PR_NGR_AREAS, // х-ки пр-нгр по участкам пути
	PRM_ANP_REAL_HIGH_BT, // 
	PRM_ANP_REAL_HIGH, //
	PRM_PSI_REAL_HIGH_BT, //скорость нарастания непогашенного ускорения (факт. скор. с НК)
	PRM_PSI_REAL_HIGH, //скорость нарастания непогашенного ускорения (факт. скор.)
	PRM_ANP_MODEL_PSP_HIGH, //Анп модельное для скоростных поездов
	PRM_ANP_MODEL_PSP_HIGH_BT, //Анп модельное для скоростных поездов с наклоном кузова
	PRM_WAY_MARKERS, // Метки пути
	PRM_JAGGY_PLAN_ON_LINES, // Неровности в плане на прямых
	PRM_EXT_WEAR_LEFT_OVERSTEP, //подуклонка левая
	PRM_EXT_WEAR_RIGHT_OVERSTEP, //подуклонка правая
	PRM_EVERYTHING	// все параметры
};

/// Верификация прописанности имен параметров
void VerifyParamNames();

// Описание параметра
const wchar_t * GetParamName(PARAMETER_TYPE param); /// Внутреннее имя параметра
const wchar_t * GetUserParamName(PARAMETER_TYPE param); /// Human-readable имя параметра
const wchar_t * GetParamMeasurements(PARAMETER_TYPE param); /// Human-readable имя параметра
unsigned long GetFileCodeByParam(PARAMETER_TYPE param); /// Код параметра в файле

// Поиск параметра
PARAMETER_TYPE GetParamCode(const wchar_t * param); /// Код параметра по внутреннему имени
PARAMETER_TYPE GetParamByUserName(const wchar_t * param); /// Код параметра по human-readable имени
PARAMETER_TYPE GetParamByFileCode(unsigned long fc); /// Код параметра по коду параметра в файле

/// Отчет по полному списку известных параметров
bool DumpParamList(const VFileName & path);

