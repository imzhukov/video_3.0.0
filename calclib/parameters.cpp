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
                msg << L"�������� " << p << L" �� ����� ��������";
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
    dump << VHtml::GetHeader("������ ��������� ����������");
    dump << "<table><thead><tr><th>���<br>���������</th><th>����������������<br>��� ���������</th>";
    dump << "<th>�������<br>���������</th><th>��� ���������<br>� �����</th></tr></thead><tbody>" << std::endl;
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
    this->Insert(PRM_NONE, 0, L"", L"����������� ��������", L"");
    this->Insert(PRM_APRINFO, DHT_APRINFO, L"APRINFO", L"��������� ����������", L"");
    this->Insert(PRM_ABS_COORD, DHT_ABS_COORD, L"ABS_COORD", L"���������� ����������", L"�");
    this->Insert(PRM_WAY_COORD, DHT_WAY_COORD, L"WAY_COORD", L"������� ����������", L"��+�");
    this->Insert(PRM_CORRUPT, DHT_CORRUPT, L"CORRUPT", L"���� ���������", L"");
    this->Insert(PRM_HAND_CORRUPT, DHT_HAND_CORRUPT, L"HAND_CORRUPT", L"������ ���������� ���������", L"");
    this->Insert(PRM_SPEED, DHT_SPEED, L"SPEED", L"�������� ������", L"��/�");
    this->Insert(PRM_COMMENT, DHT_COMMENT, L"COMMENT", L"�����������", L"");
    this->Insert(PRM_DPP, DHT_DPP, L"DPP", L"������� ��������� ���", L"");
    this->Insert(PRM_KADR_COUNT, DHT_KADR_COUNT, L"KADR_COUNT", L"������� ������", L"");
    this->Insert(PRM_OVRS2, DHT_OVRS2, L"OVRS2", L"����������� (v.2)", L"");
    this->Insert(PRM_OVRS3, DHT_OVRS3, L"OVRS3", L"����������� (v.3)", L"");
    this->Insert(PRM_OP_MARKS, DHT_OP_MARKS, L"OP_MARKS", L"����� ���������", L"");
    this->Insert(PRM_MAG_ZAZOR, DHT_MAG_ZAZOR, L"MAG_ZAZOR", L"��������� ������", L"");
    this->Insert(PRM_MAG_ZAZOR_LEFT, DHT_MAG_ZAZOR_LEFT, L"MAG_ZAZOR_LEFT", L"����� ����� (��������� ������)", L"��");
    this->Insert(PRM_MAG_ZAZOR_RIGHT, DHT_UGON_RIGHT, L"MAG_ZAZOR_RIGHT", L"����� ������ (��������� ������)", L"��");
    this->Insert(PRM_UGON_LEFT, DHT_UGON_RIGHT, L"UGON_LEFT", L"���� ������ ������", L"��");
    this->Insert(PRM_UGON_RIGHT, DHT_UGON_RIGHT, L"UGON_RIGHT", L"���� ������� ������", L"��");
    this->Insert(PRM_SPALA_MARKER, DHT_SPALA_MARKER, L"SPALA_MARKER", L"����� ����", L"");
    this->Insert(PRM_LINES, 0, L"LINES", L"������", L"");
    this->Insert(PRM_LINE_MARKS, DHT_LINE_MARKS, L"LINE_MARKS", L"�������� ������", L"");
    this->Insert(PRM_SIDEWAYS, DHT_SIDEWAYS, L"SIDEWAYS", L"������� ����", L"");
    this->Insert(PRM_HISTORY, DHT_HISTORY, L"HISTORY", L"������� ��������", L"");
    this->Insert(PRM_CURVE_MARKS, DHT_CURVE_MARKS, L"CURVE_MARKS", L"������", L"");
    this->Insert(PRM_MODEL_CURVES, DHT_MODEL_CURVES, L"MODEL_CURVES", L"��������� ������", L"");
    this->Insert(PRM_PROF_MARKS, DHT_PROF_MARKS, L"PROF_MARKS", L"����� �������", L"");
    this->Insert(PRM_KN_LEFT, DHT_KN_LEFT, L"KN_LEFT", L"�������� ���������� ������", L"");
    this->Insert(PRM_KN_RIGHT, DHT_KN_RIGHT, L"KN_RIGHT", L"�������� ���������� �����", L"");
    this->Insert(PRM_CONTROL_AREA, DHT_CONTROL_AREA, L"CONTROL_AREA", L"����������� �������", L"");
    this->Insert(PRM_KURS, DHT_KURS, L"KURS", L"�������� ����", L"���");
    this->Insert(PRM_KURS_BACKUP, DHT_KURS_BACKUP, L"KURS_BACKUP", L"��������� ����� ��������� ����", L"���");
    this->Insert(PRM_GRADIENT, DHT_GRADIENT, L"GRADIENT", L"�����", L"�");
    this->Insert(PRM_LEVEL, DHT_LEVEL, L"LEVEL", L"����������", L"��");
    this->Insert(PRM_MID_LEVEL, 0, L"MID_LEVEL", L"������� ����������", L"��");
    this->Insert(PRM_WIDTH, DHT_WIDTH, L"WIDTH", L"������ �����", L"��");
    this->Insert(PRM_MID_WIDTH, 0, L"MID_WIDTH", L"������� ������ �����", L"��");
    this->Insert(PRM_WIDTH_FRONT, DHT_WIDTH_FRONT, L"WIDTH_FRONT", L"���. �� (�������/�����)", L"��");
    this->Insert(PRM_WIDTH_BACK, DHT_WIDTH_BACK, L"WIDTH_BACK", L"���. �� (�����/������)", L"��");
    this->Insert(PRM_PIT_LEFT, DHT_PIT_LEFT, L"PIT_LEFT", L"�������� �����", L"��");
    this->Insert(PRM_PIT_RIGHT, DHT_PIT_RIGHT, L"PIT_RIGHT", L"�������� ������", L"��");
    this->Insert(PRM_CURV, DHT_CURV, L"CURV", L"��������", L"��");
    this->Insert(PRM_MID_CURV, 0, L"MID_CURV", L"������� ��������", L"��");
    this->Insert(PRM_DIST_SHORT, DHT_DIST_SHORT, L"DIST_SHORT", L"������� ��������", L"��");
    this->Insert(PRM_DIST_LONG, DHT_DIST_LONG, L"DIST_LONG", L"������� �������", L"��");
    this->Insert(PRM_RICH_LEFT, DHT_RICH_LEFT, L"RICH_LEFT", L"�������� �����", L"��");
    this->Insert(PRM_MID_RICH_LEFT, 0, L"MID_RICH_LEFT", L"������� �������� �����", L"��");
    this->Insert(PRM_RICH_RIGHT, DHT_RICH_RIGHT, L"RICH_RIGHT", L"�������� ������", L"��");
    this->Insert(PRM_MID_RICH_RIGHT, 0, L"MID_RICH_RIGHT", L"������� �������� ������", L"��");
    this->Insert(PRM_TEMP_LEFT, DHT_TEMP_LEFT, L"TEMP_LEFT", L"����������� ������ ������", L"�C");
    this->Insert(PRM_TEMP_RIGHT, DHT_TEMP_RIGHT, L"TEMP_RIGHT", L"����������� ������� ������", L"�C");
    this->Insert(PRM_ZAZOR_LEFT, DHT_ZAZOR_LEFT, L"ZAZOR_LEFT", L"����� �����", L"��");
    this->Insert(PRM_ZAZOR_RIGHT, DHT_ZAZOR_RIGHT, L"ZAZOR_RIGHT", L"����� ������", L"��");
    this->Insert(PRM_WEAR_LEFT, DHT_WEAR_LEFT, L"WEAR_LEFT", L"����� �����", L"��");
    this->Insert(PRM_WEAR_RIGHT, DHT_WEAR_RIGHT, L"WEAR_RIGHT", L"����� ������", L"��");
    this->Insert(PRM_UNI_WEAR_SENSOR, DHT_UNI_WEAR, L"UNI_WEAR_SENSOR", L"������������ ����� (����-1)", L"��");
    this->Insert(PRM_USK_VERT, DHT_USK_VERT, L"USK_VERT", L"��������� ������������", L"�/c2");
    this->Insert(PRM_USK_HOR, DHT_USK_HOR, L"USK_HOR", L"��������� ��������������", L"�/c2");
    this->Insert(PRM_LATITUDE, DHT_LATITUDE, L"LATITUDE", L"������", L"");
    this->Insert(PRM_LONGITUDE, DHT_LONGITUDE, L"LONGITUDE", L"�������", L"");
    this->Insert(PRM_SHORT_GRADIENT, DHT_SHGRADIENT, L"SHORT_GRADIENT", L"�������� �����", L"");
    this->Insert(PRM_GPS_TIME, DHT_GPS_TIME, L"GPS_TIME", L"����� GPS", L"");
    this->Insert(PRM_GPS_LATITUDE, DHT_GPS_LATITUDE, L"GPS_LATITUDE", L"������ GPS", L"");
    this->Insert(PRM_GPS_LONGITUDE, DHT_GPS_LONGITUDE, L"GPS_LONGITUDE", L"������� GPS", L"");
    this->Insert(PRM_GPS_ALTITUDE, DHT_GPS_ALTITUDE, L"GPS_ALTITUDE", L"���������� GPS", L"");
    this->Insert(PRM_TEST_4, DHT_TEST_4, L"TEST_4", L"���������� �4", L"");
    this->Insert(PRM_TEST_7, DHT_TEST_7, L"TEST_7", L"���������� �7", L"");
    this->Insert(PRM_TEST_8, DHT_TEST_8, L"TEST_8", L"���������� �8", L"");
    this->Insert(PRM_PPR_WAYPASSED, DHT_PPR_WAYPASSED, L"PPR_WAY", L"���: ���", L"");
    this->Insert(PRM_PPR_WIDTH, DHT_PPR_WIDTH, L"PPR_WIDTH", L"���: ��", L"��");
    this->Insert(PRM_PPR_NAK_LEFT, DHT_PPR_NAK_LEFT, L"PPR_NAK_LEFT", L"���: ������ �����", L"���*100");
    this->Insert(PRM_PPR_NAK_RIGHT, DHT_PPR_NAK_RIGHT, L"PPR_NAK_RIGHT", L"���: ������ ������", L"���*100");
    this->Insert(PRM_PPR_POD_LEFT, DHT_PPR_POD_LEFT, L"PPR_POD_LEFT", L"���: ���������� �����", L"���*100");
    this->Insert(PRM_PPR_POD_RIGHT, DHT_PPR_POD_RIGHT, L"PPR_POD_RIGHT", L"���: ���������� ������", L"���*100");
    this->Insert(PRM_PPR_IZN_LEFT, DHT_PPR_IZN_LEFT, L"PPR_IZN_LEFT", L"���: ����� �����", L"��");
    this->Insert(PRM_PPR_IZN_RIGHT, DHT_PPR_IZN_RIGHT, L"PPR_IZN_RIGHT", L"���: ����� ������", L"��");
    this->Insert(PRM_PPR_IZN_EXT_LEFT, DHT_PPR_IZN_EXT_LEFT, L"PPR_IZN_EXT_LEFT", L"���: ����� �������� �����", L"��");
    this->Insert(PRM_PPR_IZN_EXT_RIGHT, DHT_PPR_IZN_EXT_RIGHT, L"PPR_IZN_EXT_RIGHT", L"���: ����� �������� ������", L"��");
    this->Insert(PRM_PPR_IZN_INT_LEFT, DHT_PPR_IZN_INT_LEFT, L"PPR_IZN_INT_LEFT", L"���: ����� ���������� �����", L"��");
    this->Insert(PRM_PPR_IZN_INT_RIGHT, DHT_PPR_IZN_INT_RIGHT, L"PPR_IZN_INT_RIGHT", L"���: ����� ���������� ������", L"��");
	this->Insert(PRM_PROFILE, 0, L"PROFILE", L"�������", L"��");
    this->Insert(PRM_DIFF_PROFILE, 0, L"DIFF_PROFILE", L"���������� �� ���������� �������", L"��");
    this->Insert(PRM_APROX_PROFILE, 0, L"APROX_PROFILE", L"��������� �������", L"");
    this->Insert(PRM_JAGGY_PROFILE, 0, L"JAGGY_PROFILE", L"���������� �������", L"��");
    this->Insert(PRM_UNI_WEAR, 0, L"UNI_WEAR", L"������������ �����", L"��");
    this->Insert(PRM_STATISTICS, 0, L"STATISTICS", L"����������", L"");
    this->Insert(PRM_PLAN, 0, L"PLAN", L"����", L"");
    this->Insert(PRM_IDEAL_PLAN, 0, L"IDEAL_PLAN", L"��������� ����", L"");
    this->Insert(PRM_LEVEL_ZLINE, 0, L"LEVEL_ZLINE", L"������� ����� �� ������", L"");
    this->Insert(PRM_PLAN_OFFSET, 0, L"PLAN_OFFSET", L"���������� �� ���������� �����", L"�");
    this->Insert(PRM_PLAN_OFFSET_FICTIONAL, 0, L"PLAN_OFFSET_FICTIONAL", L"���������� �� ���������� ����� ��� ����� �������� ������", L"");
    this->Insert(PRM_MODEL_PLAN_OFFSET, 0, L"MODEL_PLAN_OFFSET", L"���������� �� ���������� ����� (����.)", L"");
    this->Insert(PRM_PROFILE_SLOPE_ELEMENT, 0, L"PROFILE_SLOPE_ELEMENT", L"����������� ����� �� ��������", L"");
    this->Insert(PRM_PROFILE_SLOPE_KM, 0, L"PROFILE_SLOPE_KM", L"����������� ����� �� ���������", L"");
    this->Insert(PRM_PROFILE_SLOPE_PK, 0, L"PROFILE_SLOPE_PK", L"����������� ����� �� ������", L"");
    this->Insert(PRM_PROFILE_SLOPE_HPK, 0, L"PROFILE_SLOPE_HPK", L"����������� ����� �� �������� ������", L"");
    this->Insert(PRM_ANP_REAL_PASS, 0, L"ANP_REAL_PASS", L"������������ ��������� (����.)", L"");
    this->Insert(PRM_ANP_REAL_TRUCK, 0, L"ANP_REAL_TRUCK", L"������������ ��������� (����.)", L"");
    this->Insert(PRM_PSI_REAL_PASS, 0, L"PSI_REAL_PASS", L"�������� ���������� ������������� ��������� (����.)", L"");
    this->Insert(PRM_PSI_REAL_TRUCK, 0, L"PSI_REAL_TRUCK", L"�������� ���������� ������������� ��������� (����.)", L"");
    this->Insert(PRM_ANP_MODEL_PSP_PASS, 0, L"ANP_MODEL_PSP_PASS", L"������������ ��������� ������ ����. (����.)", L"");
    this->Insert(PRM_ANP_MODEL_PSP_TRUCK, 0, L"ANP_MODEL_PSP_TRUCK", L"������������ ��������� ������ ����. (����.)", L"");
    this->Insert(PRM_JAGGY_PLAN, 0, L"JAGGY_PLAN", L"���������� �����", L"��");
    this->Insert(PRM_JAGGY_PLAN_ON_LINES, 0, L"JAGGY_PLAN_ON_LINES", L"���������� � ����� (������ �� ������)", L"");
    this->Insert(PRM_CURV_FROM_KURS, 0, L"CURV_FROM_KURS", L"�������� �� �����", L"");
    this->Insert(PRM_DIFF_LEVEL, 0, L"DIFF_LEVEL", L"���������� ������ �� ��������", L"");
    this->Insert(PRM_DIFF_LEVEL_MODEL, 0, L"DIFF_LEVEL_MODEL", L"���������� ������ �� �������� ������", L"");
    this->Insert(PRM_JAGGY_PROFILE_OVERSTEP, 0, L"JAGGY_PROFILE_OVERSTEP", L"���������������� ���������� �������", L"");
    this->Insert(PRM_JAGGY_PLAN_OVERSTEP, 0, L"JAGGY_PLAN_OVERSTEP", L"���������������� ���������� �����", L"");
    this->Insert(PRM_DIFF_WIDTH_FRONT, 0, L"DIFF_WIDTH_FRONT", L"�������� �� � �� �������", L"");
    this->Insert(PRM_DIFF_WIDTH_FRONT_OVERSTEP, 0, L"DIFF_WIDTH_FRONT_OVERSTEPS", L"������� (�������)", L"");
    this->Insert(PRM_DIFF_WIDTH_BACK, 0, L"DIFF_WIDTH_BACK", L"�������� �� � �� �����", L"");
    this->Insert(PRM_DIFF_WIDTH_BACK_OVERSTEP, 0, L"DIFF_WIDTH_BACK_OVERSTEPS", L"������� (�����)", L"");
    this->Insert(PRM_CANTING_LEFT_OVERSTEP, 0, L"CANTING_LEFT_OVERSTEPS", L"���������� ����� (�����������)", L"");
    this->Insert(PRM_CANTING_RIGHT_OVERSTEP, 0, L"CANTING_RIGHT_OVERSTEPS", L"���������� ������ (�����������)", L"");
    this->Insert(PRM_EXT_WEAR_LEFT_OVERSTEP, 0, L"EXT_WEAR_LEFT_OVERSTEPS", L"������� ����� ��������� ����� ����� (�����������)", L"");
    this->Insert(PRM_EXT_WEAR_RIGHT_OVERSTEP, 0, L"EXT_WEAR_RIGHT_OVERSTEPS", L"������� ����� ��������� ����� (�����������)", L"");
    this->Insert(PRM_PR_NGR_AREAS, 0, L"PR_NGR_AREAS", L"�������������� ��/��� �� �������� ����", L"");
    this->Insert(PRM_ANP_REAL_HIGH_BT, 0, L"ANP_REAL_HIGH_BT", L"������������ ��������� (����. � ����. ������)", L"");
    this->Insert(PRM_ANP_REAL_HIGH, 0, L"ANP_REAL_HIGH", L"������������ ��������� (����.)", L"");
    this->Insert(PRM_PSI_REAL_HIGH_BT, 0, L"PSI_REAL_HIGH_BT", L"�������� ���������� ������������� ��������� (����. � ����. ������)", L"");
    this->Insert(PRM_PSI_REAL_HIGH, 0, L"PSI_REAL_HIGH", L"�������� ���������� ������������� ��������� (����.)", L"");
    this->Insert(PRM_ANP_MODEL_PSP_HIGH, 0, L"ANP_MODEL_PSP_HIGH", L"������������ ��������� ������ ����. (����.)", L"");
    this->Insert(PRM_ANP_MODEL_PSP_HIGH_BT, 0, L"ANP_MODEL_PSP_HIGH_BT", L"������������ ��������� ������ ����. (����. � ����. ������)", L"");
    this->Insert(PRM_WAY_MARKERS, 0, L"WAY_MARKERS", L"����� �� ����", L"");
    this->Insert(PRM_PLAN_ELEM_LIST, 0, L"PLAN_ELEM_LIST", L"�������� �����", L"");
    this->Insert(PRM_MODEL_IDEAL_PLAN, 0, L"MODEL_IDEAL_PLAN", L"��������� ���� �� ��������� ��������", L"");
    this->Insert(PRM_MODEL_PLAN_ELEM_LIST, 0, L"MODEL_PLAN_ELEM_LIST", L"�������� ����� �� ��������� ��������", L"");
}

