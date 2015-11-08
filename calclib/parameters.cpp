#include "parameters.h"
#include "VException.h"

#include <string>
#include <sstream>
#include <ostream>
#include <fstream>
#include <set>
#include <algorithm>

#include "VHtml.h"

struct ParamDescription
{
    PARAMETER_TYPE type;
    unsigned long datatype;
    std::wstring name, hr_name, meas;
    
    ParamDescription(PARAMETER_TYPE t, unsigned long dt, const std::wstring & n, const std::wstring & h, const std::wstring & m) : 
    type(t), datatype(dt), name(n), hr_name(h), meas(m)
    {}
    
    bool operator < (const ParamDescription & other) const
    {
        return this->type < other.type;
    }
};

class ParamEnumerator
{
private:

    std::set<ParamDescription> params;

    void Insert(PARAMETER_TYPE p, unsigned long dt, const std::wstring & n, const std::wstring & hrn, const std::wstring & m)
    {
        this->params.insert(ParamDescription(p, dt, n, hrn, m));
    }

public:

	ParamEnumerator();
	
	template <class FINDER> const ParamDescription & Find(const FINDER & finder)
	{
        std::set<ParamDescription>::const_iterator it = std::find_if(this->params.begin(), this->params.end(), finder);
        if (it == this->params.end())   
            return *(this->params.begin());
        else
            return *(it);
	}
	
    const ParamDescription & FindByParamType(PARAMETER_TYPE t)
    {
        struct Finder
        {
            PARAMETER_TYPE type;
            Finder(PARAMETER_TYPE t) : type(t) {}
            bool operator () (const ParamDescription & pt) { return this->type == pt.type; }
        };
        return this->Find(Finder(t));
    }

    const ParamDescription & FindByDataType(unsigned long dt)
    {
        struct Finder
        {
            unsigned long datatype;
            Finder(unsigned long dt) : datatype(dt) {}
            bool operator () (const ParamDescription & pt) { return this->datatype == pt.datatype; }
        };
        return this->Find(Finder(dt));
    }
    
    const ParamDescription & FindByParamName(const std::wstring & n)
    {
        struct Finder
        {
            std::wstring name;
            Finder(const std::wstring & n) : name(n) {}
            bool operator () (const ParamDescription & pt) { return this->name == pt.name; }
        };
        return this->Find(Finder(n));
    }

    const ParamDescription & FindByParamHrName(const std::wstring & h)
    {
        struct Finder
        {
            std::wstring hr_name;
            Finder(const std::wstring & h) : hr_name(h) {}
            bool operator () (const ParamDescription & pt) { return this->hr_name == pt.hr_name; }
        };
        return this->Find(Finder(h));
    }

	void VerifyParamNames()
	{
        for (int p = PRM_APRINFO; p != PRM_EVERYTHING; ++p)
        {
            PARAMETER_TYPE param = (PARAMETER_TYPE) p;
            if (this->FindByParamType(param).type != param)
            {
                std::wstringstream msg;
                msg << L"Параметр " << p << L" не имеет описания";
                throw VSimpleException(msg.str(), L"");
            }
        }
	}
};

static ParamEnumerator paramEnum;

const wchar_t * GetParamName(PARAMETER_TYPE param)
{
    return paramEnum.FindByParamType(param).name.c_str();
}

const wchar_t * GetUserParamName(PARAMETER_TYPE param)
{
    return paramEnum.FindByParamType(param).hr_name.c_str();
}

const wchar_t * GetParamMeasurements(PARAMETER_TYPE param)
{
    return paramEnum.FindByParamType(param).meas.c_str();
}

unsigned long GetFileCodeByParam(PARAMETER_TYPE param)
{
    return paramEnum.FindByParamType(param).datatype;
}

PARAMETER_TYPE GetParamCode(const wchar_t * name)
{
    return paramEnum.FindByParamName(name).type;
}

PARAMETER_TYPE GetParamByUserName(const wchar_t * hr_name)
{
    return paramEnum.FindByParamHrName(hr_name).type;
}

PARAMETER_TYPE GetParamByFileCode(unsigned long fc)
{
    return paramEnum.FindByDataType(fc).type;
}

void VerifyParamNames()
{
    return paramEnum.VerifyParamNames();
}

bool DumpParamList(const VFileName & path)
{
    std::ofstream dump(wstring_to_string(path.GetPath()).c_str());
    if (!dump.is_open()) return false;
    dump << VHtml::GetHeader("Список известных параметров");
    dump << "<table><thead><tr><th>Имя<br>параметра</th><th>Пользовательское<br>имя параметра</th>";
    dump << "<th>Единицы<br>измерения</th><th>Код параметра<br>в файле</th></tr></thead><tbody>" << std::endl;
    for (int p = PRM_APRINFO; p != PRM_EVERYTHING; ++p)
    {
        PARAMETER_TYPE param = (PARAMETER_TYPE) p;
        dump << "<tr><th>" << wstring_to_string(GetParamName(param)) << "</th>";
        dump << "<td>" << wstring_to_string(GetUserParamName(param)) << "</td>";
        dump << "<td>" <<  wstring_to_string(GetParamMeasurements(param)) << "</td>";
        dump << "<td>" << GetFileCodeByParam(param) << "</td></tr>";
    }
    dump << "</tbody></table>" << std::endl;
    dump << VHtml::GetFooter();
    dump.close();
    return true;
}

ParamEnumerator::ParamEnumerator()
{
    this->Insert(PRM_NONE, 0, L"", L"Неизвестный параметр", L"");
    this->Insert(PRM_APRINFO, DHT_APRINFO, L"APRINFO", L"Априорная информация", L"");
    this->Insert(PRM_ABS_COORD, DHT_ABS_COORD, L"ABS_COORD", L"Абсолютная координата", L"м");
    this->Insert(PRM_WAY_COORD, DHT_WAY_COORD, L"WAY_COORD", L"Путевая координата", L"км+м");
    this->Insert(PRM_CORRUPT, DHT_CORRUPT, L"CORRUPT", L"Сбои измерений", L"");
    this->Insert(PRM_HAND_CORRUPT, DHT_HAND_CORRUPT, L"HAND_CORRUPT", L"Ручная отбраковка измерений", L"");
    this->Insert(PRM_SPEED, DHT_SPEED, L"SPEED", L"Скорость вагона", L"км/ч");
    this->Insert(PRM_COMMENT, DHT_COMMENT, L"COMMENT", L"Комментарии", L"");
    this->Insert(PRM_DPP, DHT_DPP, L"DPP", L"Счетчик импульсов ДПП", L"");
    this->Insert(PRM_KADR_COUNT, DHT_KADR_COUNT, L"KADR_COUNT", L"Счетчик кадров", L"");
    this->Insert(PRM_OVRS2, DHT_OVRS2, L"OVRS2", L"Отступления (v.2)", L"");
    this->Insert(PRM_OVRS3, DHT_OVRS3, L"OVRS3", L"Отступления (v.3)", L"");
    this->Insert(PRM_OP_MARKS, DHT_OP_MARKS, L"OP_MARKS", L"Метки оператора", L"");
    this->Insert(PRM_MAG_ZAZOR, DHT_MAG_ZAZOR, L"MAG_ZAZOR", L"Магнитные зазоры", L"");
    this->Insert(PRM_MAG_ZAZOR_LEFT, DHT_MAG_ZAZOR_LEFT, L"MAG_ZAZOR_LEFT", L"Зазор левый (Магнитный датчик)", L"мм");
    this->Insert(PRM_MAG_ZAZOR_RIGHT, DHT_UGON_RIGHT, L"MAG_ZAZOR_RIGHT", L"Зазор правый (Магнитный датчик)", L"мм");
    this->Insert(PRM_UGON_LEFT, DHT_UGON_RIGHT, L"UGON_LEFT", L"Угон левого рельса", L"мм");
    this->Insert(PRM_UGON_RIGHT, DHT_UGON_RIGHT, L"UGON_RIGHT", L"Угон правого рельса", L"мм");
    this->Insert(PRM_SPALA_MARKER, DHT_SPALA_MARKER, L"SPALA_MARKER", L"Метки шпал", L"");
    this->Insert(PRM_LINES, 0, L"LINES", L"Прямые", L"");
    this->Insert(PRM_LINE_MARKS, DHT_LINE_MARKS, L"LINE_MARKS", L"Разметка прямых", L"");
    this->Insert(PRM_SIDEWAYS, DHT_SIDEWAYS, L"SIDEWAYS", L"Боковые пути", L"");
    this->Insert(PRM_HISTORY, DHT_HISTORY, L"HISTORY", L"История операций", L"");
    this->Insert(PRM_CURVE_MARKS, DHT_CURVE_MARKS, L"CURVE_MARKS", L"Кривые", L"");
    this->Insert(PRM_MODEL_CURVES, DHT_MODEL_CURVES, L"MODEL_CURVES", L"Модельные кривые", L"");
    this->Insert(PRM_PROF_MARKS, DHT_PROF_MARKS, L"PROF_MARKS", L"Метки профиля", L"");
    this->Insert(PRM_KN_LEFT, DHT_KN_LEFT, L"KN_LEFT", L"Короткие неровности правые", L"");
    this->Insert(PRM_KN_RIGHT, DHT_KN_RIGHT, L"KN_RIGHT", L"Короткие неровности левые", L"");
    this->Insert(PRM_CONTROL_AREA, DHT_CONTROL_AREA, L"CONTROL_AREA", L"Контрольные участки", L"");
    this->Insert(PRM_KURS, DHT_KURS, L"KURS", L"Курсовой угол", L"рад");
    this->Insert(PRM_KURS_BACKUP, DHT_KURS_BACKUP, L"KURS_BACKUP", L"Резервная копия курсового угла", L"рад");
    this->Insert(PRM_GRADIENT, DHT_GRADIENT, L"GRADIENT", L"Уклон", L"‰");
    this->Insert(PRM_LEVEL, DHT_LEVEL, L"LEVEL", L"Возвышение", L"мм");
    this->Insert(PRM_MID_LEVEL, 0, L"MID_LEVEL", L"Среднее возвышение", L"мм");
    this->Insert(PRM_WIDTH, DHT_WIDTH, L"WIDTH", L"Ширина колеи", L"мм");
    this->Insert(PRM_MID_WIDTH, 0, L"MID_WIDTH", L"Средняя ширина колеи", L"мм");
    this->Insert(PRM_WIDTH_FRONT, DHT_WIDTH_FRONT, L"WIDTH_FRONT", L"Доп. ШК (спереди/салон)", L"мм");
    this->Insert(PRM_WIDTH_BACK, DHT_WIDTH_BACK, L"WIDTH_BACK", L"Доп. ШК (сзади/дизель)", L"мм");
    this->Insert(PRM_PIT_LEFT, DHT_PIT_LEFT, L"PIT_LEFT", L"Просадка левая", L"мм");
    this->Insert(PRM_PIT_RIGHT, DHT_PIT_RIGHT, L"PIT_RIGHT", L"Просадка правая", L"мм");
    this->Insert(PRM_CURV, DHT_CURV, L"CURV", L"Кривизна", L"мм");
    this->Insert(PRM_MID_CURV, 0, L"MID_CURV", L"Средняя кривизна", L"мм");
    this->Insert(PRM_DIST_SHORT, DHT_DIST_SHORT, L"DIST_SHORT", L"Перекос короткий", L"мм");
    this->Insert(PRM_DIST_LONG, DHT_DIST_LONG, L"DIST_LONG", L"Перекос длинный", L"мм");
    this->Insert(PRM_RICH_LEFT, DHT_RICH_LEFT, L"RICH_LEFT", L"Рихтовка левая", L"мм");
    this->Insert(PRM_MID_RICH_LEFT, 0, L"MID_RICH_LEFT", L"Средняя рихтовка левая", L"мм");
    this->Insert(PRM_RICH_RIGHT, DHT_RICH_RIGHT, L"RICH_RIGHT", L"Рихтовка правая", L"мм");
    this->Insert(PRM_MID_RICH_RIGHT, 0, L"MID_RICH_RIGHT", L"Средняя рихтовка правая", L"мм");
    this->Insert(PRM_TEMP_LEFT, DHT_TEMP_LEFT, L"TEMP_LEFT", L"Температура левого рельса", L"°C");
    this->Insert(PRM_TEMP_RIGHT, DHT_TEMP_RIGHT, L"TEMP_RIGHT", L"Температура правого рельса", L"°C");
    this->Insert(PRM_ZAZOR_LEFT, DHT_ZAZOR_LEFT, L"ZAZOR_LEFT", L"Зазор левый", L"мм");
    this->Insert(PRM_ZAZOR_RIGHT, DHT_ZAZOR_RIGHT, L"ZAZOR_RIGHT", L"Зазор правый", L"мм");
    this->Insert(PRM_WEAR_LEFT, DHT_WEAR_LEFT, L"WEAR_LEFT", L"Износ левый", L"мм");
    this->Insert(PRM_WEAR_RIGHT, DHT_WEAR_RIGHT, L"WEAR_RIGHT", L"Износ правый", L"мм");
    this->Insert(PRM_UNI_WEAR_SENSOR, DHT_UNI_WEAR, L"UNI_WEAR_SENSOR", L"Объединенный износ (ПЭВМ-1)", L"мм");
    this->Insert(PRM_USK_VERT, DHT_USK_VERT, L"USK_VERT", L"Ускорение вертикальное", L"м/c2");
    this->Insert(PRM_USK_HOR, DHT_USK_HOR, L"USK_HOR", L"Ускорение горизонтальное", L"м/c2");
    this->Insert(PRM_LATITUDE, DHT_LATITUDE, L"LATITUDE", L"Широта", L"");
    this->Insert(PRM_LONGITUDE, DHT_LONGITUDE, L"LONGITUDE", L"Долгота", L"");
    this->Insert(PRM_SHORT_GRADIENT, DHT_SHGRADIENT, L"SHORT_GRADIENT", L"Короткий уклон", L"");
    this->Insert(PRM_GPS_TIME, DHT_GPS_TIME, L"GPS_TIME", L"Время GPS", L"");
    this->Insert(PRM_GPS_LATITUDE, DHT_GPS_LATITUDE, L"GPS_LATITUDE", L"Широта GPS", L"");
    this->Insert(PRM_GPS_LONGITUDE, DHT_GPS_LONGITUDE, L"GPS_LONGITUDE", L"Долгота GPS", L"");
    this->Insert(PRM_GPS_ALTITUDE, DHT_GPS_ALTITUDE, L"GPS_ALTITUDE", L"Возвышение GPS", L"");
    this->Insert(PRM_TEST_4, DHT_TEST_4, L"TEST_4", L"Отладочный №4", L"");
    this->Insert(PRM_TEST_7, DHT_TEST_7, L"TEST_7", L"Отладочный №7", L"");
    this->Insert(PRM_TEST_8, DHT_TEST_8, L"TEST_8", L"Отладочный №8", L"");
    this->Insert(PRM_PPR_WAYPASSED, DHT_PPR_WAYPASSED, L"PPR_WAY", L"ППР: ДПП", L"");
    this->Insert(PRM_PPR_WIDTH, DHT_PPR_WIDTH, L"PPR_WIDTH", L"ППР: ШК", L"мм");
    this->Insert(PRM_PPR_NAK_LEFT, DHT_PPR_NAK_LEFT, L"PPR_NAK_LEFT", L"ППР: Наклон левый", L"рад*100");
    this->Insert(PRM_PPR_NAK_RIGHT, DHT_PPR_NAK_RIGHT, L"PPR_NAK_RIGHT", L"ППР: Наклон правый", L"рад*100");
    this->Insert(PRM_PPR_POD_LEFT, DHT_PPR_POD_LEFT, L"PPR_POD_LEFT", L"ППР: Подуклонка левая", L"рад*100");
    this->Insert(PRM_PPR_POD_RIGHT, DHT_PPR_POD_RIGHT, L"PPR_POD_RIGHT", L"ППР: Полуклонка правая", L"рад*100");
    this->Insert(PRM_PPR_IZN_LEFT, DHT_PPR_IZN_LEFT, L"PPR_IZN_LEFT", L"ППР: Износ левый", L"мм");
    this->Insert(PRM_PPR_IZN_RIGHT, DHT_PPR_IZN_RIGHT, L"PPR_IZN_RIGHT", L"ППР: Износ правый", L"мм");
    this->Insert(PRM_PPR_IZN_EXT_LEFT, DHT_PPR_IZN_EXT_LEFT, L"PPR_IZN_EXT_LEFT", L"ППР: Износ наружний левый", L"мм");
    this->Insert(PRM_PPR_IZN_EXT_RIGHT, DHT_PPR_IZN_EXT_RIGHT, L"PPR_IZN_EXT_RIGHT", L"ППР: Износ наружний правый", L"мм");
    this->Insert(PRM_PPR_IZN_INT_LEFT, DHT_PPR_IZN_INT_LEFT, L"PPR_IZN_INT_LEFT", L"ППР: Износ внутренний левый", L"мм");
    this->Insert(PRM_PPR_IZN_INT_RIGHT, DHT_PPR_IZN_INT_RIGHT, L"PPR_IZN_INT_RIGHT", L"ППР: Износ внутренний правый", L"мм");
	this->Insert(PRM_PROFILE, 0, L"PROFILE", L"Профиль", L"см");
    this->Insert(PRM_DIFF_PROFILE, 0, L"DIFF_PROFILE", L"Отклонения от идеального профиля", L"см");
    this->Insert(PRM_APROX_PROFILE, 0, L"APROX_PROFILE", L"Идеальный профиль", L"");
    this->Insert(PRM_JAGGY_PROFILE, 0, L"JAGGY_PROFILE", L"Неровности профиля", L"см");
    this->Insert(PRM_UNI_WEAR, 0, L"UNI_WEAR", L"Объединенный износ", L"мм");
    this->Insert(PRM_STATISTICS, 0, L"STATISTICS", L"Статистика", L"");
    this->Insert(PRM_PLAN, 0, L"PLAN", L"План", L"");
    this->Insert(PRM_IDEAL_PLAN, 0, L"IDEAL_PLAN", L"Идеальный план", L"");
    this->Insert(PRM_LEVEL_ZLINE, 0, L"LEVEL_ZLINE", L"Нулевая линия по уровню", L"");
    this->Insert(PRM_PLAN_OFFSET, 0, L"PLAN_OFFSET", L"Отклонения от идеального плана", L"м");
    this->Insert(PRM_PLAN_OFFSET_FICTIONAL, 0, L"PLAN_OFFSET_FICTIONAL", L"Отклонения от идеального плана без учета разметки прямых", L"");
    this->Insert(PRM_MODEL_PLAN_OFFSET, 0, L"MODEL_PLAN_OFFSET", L"Отклонения от идеального плана (пасп.)", L"");
    this->Insert(PRM_PROFILE_SLOPE_ELEMENT, 0, L"PROFILE_SLOPE_ELEMENT", L"Спрямленный уклон на элементе", L"");
    this->Insert(PRM_PROFILE_SLOPE_KM, 0, L"PROFILE_SLOPE_KM", L"Спрямленный уклон на километре", L"");
    this->Insert(PRM_PROFILE_SLOPE_PK, 0, L"PROFILE_SLOPE_PK", L"Спрямленный уклон на пикете", L"");
    this->Insert(PRM_PROFILE_SLOPE_HPK, 0, L"PROFILE_SLOPE_HPK", L"Спрямленный уклон на половине пикета", L"");
    this->Insert(PRM_ANP_REAL_PASS, 0, L"ANP_REAL_PASS", L"Непогашенное ускорение (пасс.)", L"");
    this->Insert(PRM_ANP_REAL_TRUCK, 0, L"ANP_REAL_TRUCK", L"Непогашенное ускорение (груз.)", L"");
    this->Insert(PRM_PSI_REAL_PASS, 0, L"PSI_REAL_PASS", L"Скорость нарастания непогашенного ускорения (пасс.)", L"");
    this->Insert(PRM_PSI_REAL_TRUCK, 0, L"PSI_REAL_TRUCK", L"Скорость нарастания непогашенного ускорения (груз.)", L"");
    this->Insert(PRM_ANP_MODEL_PSP_PASS, 0, L"ANP_MODEL_PSP_PASS", L"Непогашенное ускорение модель пасп. (пасс.)", L"");
    this->Insert(PRM_ANP_MODEL_PSP_TRUCK, 0, L"ANP_MODEL_PSP_TRUCK", L"Непогашенное ускорение модель пасп. (груз.)", L"");
    this->Insert(PRM_JAGGY_PLAN, 0, L"JAGGY_PLAN", L"Неровности плана", L"мм");
    this->Insert(PRM_JAGGY_PLAN_ON_LINES, 0, L"JAGGY_PLAN_ON_LINES", L"Неровности в плане (только на прямых)", L"");
    this->Insert(PRM_CURV_FROM_KURS, 0, L"CURV_FROM_KURS", L"Кривизна из курса", L"");
    this->Insert(PRM_DIFF_LEVEL, 0, L"DIFF_LEVEL", L"Отклонения уровня от разметки", L"");
    this->Insert(PRM_DIFF_LEVEL_MODEL, 0, L"DIFF_LEVEL_MODEL", L"Отклонения уровня от разметки модели", L"");
    this->Insert(PRM_JAGGY_PROFILE_OVERSTEP, 0, L"JAGGY_PROFILE_OVERSTEP", L"Сверхнормативные неровности профиля", L"");
    this->Insert(PRM_JAGGY_PLAN_OVERSTEP, 0, L"JAGGY_PLAN_OVERSTEP", L"Сверхнормативные неровности плана", L"");
    this->Insert(PRM_DIFF_WIDTH_FRONT, 0, L"DIFF_WIDTH_FRONT", L"Разность ШК и ШК спереди", L"");
    this->Insert(PRM_DIFF_WIDTH_FRONT_OVERSTEP, 0, L"DIFF_WIDTH_FRONT_OVERSTEPS", L"Отжатия (спереди)", L"");
    this->Insert(PRM_DIFF_WIDTH_BACK, 0, L"DIFF_WIDTH_BACK", L"Разность ШК и ШК сзади", L"");
    this->Insert(PRM_DIFF_WIDTH_BACK_OVERSTEP, 0, L"DIFF_WIDTH_BACK_OVERSTEPS", L"Отжатия (сзади)", L"");
    this->Insert(PRM_CANTING_LEFT_OVERSTEP, 0, L"CANTING_LEFT_OVERSTEPS", L"Подуклонка левая (отступления)", L"");
    this->Insert(PRM_CANTING_RIGHT_OVERSTEP, 0, L"CANTING_RIGHT_OVERSTEPS", L"Подуклонка правая (отступления)", L"");
    this->Insert(PRM_EXT_WEAR_LEFT_OVERSTEP, 0, L"EXT_WEAR_LEFT_OVERSTEPS", L"Боковой износ нерабочей грани левая (отступления)", L"");
    this->Insert(PRM_EXT_WEAR_RIGHT_OVERSTEP, 0, L"EXT_WEAR_RIGHT_OVERSTEPS", L"Боковой износ нерабочей грани (отступления)", L"");
    this->Insert(PRM_PR_NGR_AREAS, 0, L"PR_NGR_AREAS", L"Характеристики ПР/НГР по участкам пути", L"");
    this->Insert(PRM_ANP_REAL_HIGH_BT, 0, L"ANP_REAL_HIGH_BT", L"Непогашенное ускорение (скор. с накл. кузова)", L"");
    this->Insert(PRM_ANP_REAL_HIGH, 0, L"ANP_REAL_HIGH", L"Непогашенное ускорение (скор.)", L"");
    this->Insert(PRM_PSI_REAL_HIGH_BT, 0, L"PSI_REAL_HIGH_BT", L"Скорость нарастания непогашенного ускорения (скор. с накл. кузова)", L"");
    this->Insert(PRM_PSI_REAL_HIGH, 0, L"PSI_REAL_HIGH", L"Скорость нарастания непогашенного ускорения (скор.)", L"");
    this->Insert(PRM_ANP_MODEL_PSP_HIGH, 0, L"ANP_MODEL_PSP_HIGH", L"Непогашенное ускорение модель пасп. (скор.)", L"");
    this->Insert(PRM_ANP_MODEL_PSP_HIGH_BT, 0, L"ANP_MODEL_PSP_HIGH_BT", L"Непогашенное ускорение модель пасп. (скор. с накл. кузова)", L"");
    this->Insert(PRM_WAY_MARKERS, 0, L"WAY_MARKERS", L"Метки на пути", L"");
    this->Insert(PRM_PLAN_ELEM_LIST, 0, L"PLAN_ELEM_LIST", L"Элементы плана", L"");
    this->Insert(PRM_MODEL_IDEAL_PLAN, 0, L"MODEL_IDEAL_PLAN", L"Идеальный план по модельной разметке", L"");
    this->Insert(PRM_MODEL_PLAN_ELEM_LIST, 0, L"MODEL_PLAN_ELEM_LIST", L"Элементы плана по модельной разметке", L"");
}

