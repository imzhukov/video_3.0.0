#pragma once

/*
Файл datatypes.h
Допустимые типы данных в файлах c4f
*/

#include "os.h"
#include "stdint.h"

/** Флаги использования полей структуры

/** Количество записей*/
#define	PHO_REC_COUNT			0x0001		
/** Размер записи*/
#define	PHO_REC_SIZE			0x0002		
/** Делитель*/
#define	PHO_DIVIDER				0x0004		
/** Тип*/
#define	PHO_C_TYPE				0x0008		
/** Смещение*/
#define	PHO_OFFSET				0x0010		
/** Версия структуры*/
#define	PHO_REC_VER				0x0020		
/** Дата создания блока*/
#define	PHO_CREATE_TIME			0x0040		
/** Дата обновления блока*/
#define	PHO_UPDATE_TIME			0x0080		
/** Набор флагов для блока*/
#define	PHO_BLOCK_FLAGS			0x0100
/** Индекс блока данного типа*/
#define	PHO_INDEX				0x0200

/** Стандартный набор флагов*/
#define PHO_DEFAULT PHO_REC_COUNT | PHO_REC_SIZE | PHO_DIVIDER | PHO_C_TYPE | PHO_REC_VER | PHO_CREATE_TIME | PHO_UPDATE_TIME | PHO_INDEX


/* Типы C*/

/** Неизвестный тип*/
#define VCT_UNKNOWN		-1
/** float*/
#define	VCT_FLOAT		0
/** double*/
#define	VCT_DOUBLE		1
/** int*/
#define	VCT_INT			2
/** unsigned int*/
#define	VCT_UINT		3
/** long*/
#define	VCT_LONG		4
/** unsigned long*/
#define	VCT_ULONG		5
/** char*/
#define	VCT_CHAR		6
/** unsigned char*/
#define	VCT_UCHAR		7
/** short*/
#define	VCT_SHORT		8
/** unsigned short*/
#define	VCT_USHORT		9
/** user*/
#define	VCT_USER		10

/** Возвращает название C типа по его коду*/
const wchar_t * GetCTypeName(int code);

/** Массив смещений*/
#define	DHT_OFFSET				0L
/** Априорная информация*/
#define	DHT_APRINFO				10L
/** Абсолютная координата*/
#define	DHT_ABS_COORD			11L
/** Путевые координаты*/
#define	DHT_WAY_COORD			12L
/** признаки сбоя датчиков*/
#define	DHT_CORRUPT				13L
/** Кривые*/
#define	DHT_CURVE_MARKS			15L
/** Отступления*/
#define	DHT_OVRS2			    16L
/** Метки профиля*/
#define	DHT_PROF_MARKS			17L
/** Опорные отметки профиля*/
//#define	DHT_PR_MARKS		18L
/** Левый угон*/
#define	DHT_UGON_LEFT			19L
/** Правый угон*/
#define	DHT_UGON_RIGHT			20L
/** Метки оператора*/
#define	DHT_OP_MARKS			21L
/** Прямые*/
#define DHT_LINE_MARKS			22L
/** Метки прямых*/
//#define DHT_LINE_MARKS		23L
/** Скорость вагона*/
#define	DHT_SPEED				100L
/** Курс*/
#define	DHT_KURS				101L
/** Уклон*/
#define	DHT_GRADIENT			102L
/** Уpовень*/
#define	DHT_LEVEL				103L
/** Шиpина*/
#define	DHT_WIDTH				104L
/** Пpосадка пpавая*/
#define	DHT_PIT_RIGHT			105L
/** Пpосадка левая*/
#define	DHT_PIT_LEFT			106L
/** Кpивизна*/
#define	DHT_CURV				107L
/** Пеpекос короткий*/
#define	DHT_DIST_SHORT			108L
/** Пеpекос длинный*/
#define	DHT_DIST_LONG			109L
/** Рихтовка правая*/
#define	DHT_RICH_RIGHT			110L
/** Рихтовка левая*/
#define	DHT_RICH_LEFT			111L
/** Темпеpатуpа левого pельса*/
#define	DHT_TEMP_LEFT			112L
/** Стыковой зазоp левого pельса*/
#define	DHT_ZAZOR_LEFT			113L
/** Износ левого pельса*/
#define	DHT_WEAR_LEFT			114L
/** Коpоткие неpовности на левом pельсе*/
#define	DHT_KN_LEFT				115L
/** Темпеpатуpа правого pельса*/
#define	DHT_TEMP_RIGHT			116L
/** Стыковой зазоp правого pельса*/
#define	DHT_ZAZOR_RIGHT			117L
/** Износ правого pельса*/
#define	DHT_WEAR_RIGHT			118L
/** Коpоткие неpовности на пpавом pельсе*/
#define	DHT_KN_RIGHT			119L
/** Средний уровень*/
#define	DHT_MID_LEVEL			120L
/** Ср. кривизны*/
#define	DHT_MID_CURV			121L
/** Ср. pихтовка правая*/
#define	DHT_MID_RICH_RIGHT		122L
/** Ср. pихтовка левая*/
#define	DHT_MID_RICH_LEFT		123L
/** Короткий уклон*/
#define	DHT_SHGRADIENT			127L
/** Горизонтальное ускорение*/
#define	DHT_USK_HOR				128L
/** Вертикальное ускорение*/
#define	DHT_USK_VERT			129L
/** Магнитный зазор*/
#define DHT_MAG_ZAZOR			130L
/** Участки съездов на боковые пути*/
#define DHT_SIDEWAYS            131L
/** История операций произведенных над файлом*/
#define DHT_HISTORY             132L
/** Комментарий*/
#define DHT_COMMENT             133L
/** Средняя ШК*/
#define DHT_MID_WIDTH			134L
/** Дополнительная ШК (спереди/салон)*/
#define DHT_WIDTH_FRONT			135L
/** Дополнительная ШК (сзади/дизель)*/
#define DHT_WIDTH_BACK			136L
/** Объединенный износ*/
#define DHT_UNI_WEAR			137L
/** Априорная информация для станционных путей*/
#define DHT_STATION_APRINFO		138L
/** Список станционных путей (Obsolete)*/
#define DHT_STATION_WAYS		139L
/** Привязки меток на станционных путях к объектам*/
#define DHT_ATTACHMENTS			140L
/** Станционный путь*/
#define DHT_WAY					141L
/** Проезд по станции*/
#define DHT_STATION_TRAILS		142L
/** Таблица длин станционных километров (Obsolete. У обоих Михаилов могут найтись файлы с этим) */
#define DHT_STATION_KMS         143L
/** Пересчитанные абс.координаты (растянутые/сжатые) */
#define DHT_ABS_COORD_SHIFTED   144L
/** Широта */
#define DHT_LATITUDE	        145L
/** Долгота */
#define DHT_LONGITUDE	        146L
/** Время GPS */
#define DHT_GPS_TIME			147L
/** Измерения георадара */
#define DHT_GEORADAR			148L
/** Габаритомер - к-ты левого рельса */
#define DHT_GABA_LEFTRAIL		149L
/** Габаритомер - к-ты правого рельса */
#define DHT_GABA_RIGHTRAIL		150L
/** Габаритомер - к-ты контактного провода */
#define DHT_GABA_KP				151L
/** Габаритомер - к-ты точек */
#define DHT_GABA_TNL			152L
/** Габаритомер - результаты анализа */
#define DHT_GABA_RESULT			153L
/** Проектные кривые (модели кривых)*/
#define DHT_MODEL_CURVES		154L
/** Параметры габарита в тоннеле, для которых произведен анализ*/
#define DHT_GABA_TPARAMS		155L
/** Резервная копия курса */
#define DHT_KURS_BACKUP			156L
/** Контрольные участки */
#define DHT_CONTROL_AREA		157L
/** Признаки сбоя датчиков (ручная отбраковка)*/
#define DHT_HAND_CORRUPT		158L
/** Метки шпал */
#define DHT_SPALA_MARKER		159L
/** Полный профиль рельса */
#define DHT_PPR_WAYPASSED		160L
#define DHT_PPR_WIDTH			161L
#define DHT_PPR_NAK_LEFT		162L
#define DHT_PPR_NAK_RIGHT		163L
#define DHT_PPR_POD_LEFT		164L
#define DHT_PPR_POD_RIGHT		165L
#define DHT_PPR_IZN_LEFT		166L
#define DHT_PPR_IZN_RIGHT		167L
// -> - - - - - - Отладочные - - - - - - - - - - -
#define DHT_GPS_LONGITUDE		201L
#define DHT_GPS_LATITUDE		202L
#define DHT_GPS_ALTITUDE		203L
#define DHT_TEST_4				204L
#define DHT_MAG_ZAZOR_LEFT		205L
#define DHT_MAG_ZAZOR_RIGHT		206L
#define DHT_TEST_7				207L
#define DHT_TEST_8				208L

#define DHT_KADR_COUNT			209L
#define DHT_OVRS3				210L
#define DHT_DPP					211L

#define DHT_PPR_IZN_EXT_LEFT    212L
#define DHT_PPR_IZN_EXT_RIGHT   213L
#define DHT_PPR_IZN_INT_LEFT    214L
#define DHT_PPR_IZN_INT_RIGHT   215L
// <- - - - - - - Отладочные - - - - - - - - - - -

/** Номер первого параметра*/
#define	DHT_FIRST_PARAM			DHT_SPEED
/** Номер последнего параметра*/
#define	DHT_LAST_PARAM			DHT_PPR_WEAR_INT_RIGHT

