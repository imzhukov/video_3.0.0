
#include "situation.h"


//------------------------------------ ������ ������ VRTMessage -------------------------------------------------

/// �����������
VRTMessage::VRTMessage(double in_coord, const std::wstring& in_message) : coord(in_coord), message(in_message) {}

/// ���������� ����������
double VRTMessage::GetCoord() const
{
	return coord;
}

/// ���������� ���������
const std::wstring& VRTMessage::GetMsg() const
{
	return message;
}

//---------------------------------- ������ ������ VBadPassport ----------------------------------------

/// �����������
VBadPassport::VBadPassport(double in_start, double in_end, 
	double in_plan_correlation, double in_plan_range,		
	double in_level_correlation, double in_level_range,
	double in_coordinates_match_percent) :
	section(std::min<double>(in_start,in_end), std::max<double>(in_start,in_end)), 
	plan_correlation(in_plan_correlation), plan_range(in_plan_range),		
	level_correlation(in_level_correlation), level_range(in_level_range),
	coordinates_match_percent(in_coordinates_match_percent)
{};
/// ���������� �������
const pro::interval<double>& VBadPassport::GetSection() const 
{
	return section;
};
/// ���������� ��������
const std::wstring VBadPassport::GetDescription() const 
{
	std::wstringstream description;
	description << L"����. � ����. ";
	if (IsBadPlan())
		description << BAD_PLAN_SIGN;
	if (IsBadLevel())
		description << BAD_LEVEL_SIGN;
	if (IsBadCoordinates())
		description << BAD_COORDINATES_SIGN;			
	return description.str();
};
/// ���������� true, ���� ������ �� ������������� �������� �� ����� ��������
bool VBadPassport::IsBadPlan() const
{
	return plan_range >= 20 && plan_correlation < PLAN_CORRELATION;
};			
/// ���������� true, ���� ������ �� ������������� �������� �� ������
bool VBadPassport::IsBadLevel() const
{
	return level_range >= 20.0 && level_correlation < LEVEL_CORRELATION;
};
/// ���������� true, ���� ������ �� ������������� �������� �� �����������
bool VBadPassport::IsBadCoordinates() const
{
	return coordinates_match_percent < COORDINATES_MATCH_PERCENT;
};
/// ���������� true, ���� ������ �� ������������� �������� ���� �� �� ������ �� ����������
bool VBadPassport::IsBad() const
{
	return IsBadPlan() || IsBadLevel() || IsBadCoordinates();
};

//---------------------------------- ������ ������ VSituation ------------------------------------------

/// ������������ ����� ������� �����
void VSituation::AdjustLine(zline& out_zline, size_t in_index, const line_t& in_line)
{
	if (in_index==0)
	{
	}
	else if (in_index==out_zline.GetLineCount()-1)
	{
	}
	else
	{
		line_t left_line=out_zline.GetLine(in_index-1);
		line_t right_line=out_zline.GetLine(in_index+1);
		pro::interval<double> left_range=out_zline.GetRange(in_index-1);
		pro::interval<double> right_range=out_zline.GetRange(in_index+1);
		pro::interval<double> allowed_range(left_range.left_bound().value, right_range.right_bound().value);
		double old_length=out_zline.GetRange(in_index).length();
		if (!parallel(left_line, in_line) &&
		    !parallel(right_line, in_line))
		{
			point_t new_left_point=calc_cross_point(left_line, in_line);
			point_t new_right_point=calc_cross_point(right_line, in_line);
			double new_length=fabs(new_right_point.x-new_left_point.x);
			if (allowed_range.contains(new_left_point.x) && 
				allowed_range.contains(new_right_point.x) && 
				new_left_point.x < new_right_point.x &&
				pro::interval<double>(0.5,2.0).contains(old_length/new_length))
			{
				out_zline.GetLine(in_index)=line_t(new_left_point, new_right_point);	
			}
		}
	}
}

/// �����������
VSituation::VSituation(){};

/// �����������
VSituation::VSituation(const VAprinfo& in_aprinfo)
{
	Init(in_aprinfo);
}

/// �������������
void VSituation::Init(const VAprinfo& in_aprinfo)
{
	level_passport = zline(pro::interval<double>(in_aprinfo.GetAbsCoordBase(),in_aprinfo.GetAbsCoordEnd()));
	rich_passport = zline(pro::interval<double>(in_aprinfo.GetAbsCoordBase(),in_aprinfo.GetAbsCoordEnd()));
	width_passport = zline(pro::interval<double>(in_aprinfo.GetAbsCoordBase(),in_aprinfo.GetAbsCoordEnd()));
	
	level_previous = zline(pro::interval<double>(in_aprinfo.GetAbsCoordBase(),in_aprinfo.GetAbsCoordEnd()));
	rich_previous = zline(pro::interval<double>(in_aprinfo.GetAbsCoordBase(),in_aprinfo.GetAbsCoordEnd()));

	aprinfo=in_aprinfo;
	
	//���������
	km_list=aprinfo.GetKmList();	
	//������
	curve_list=aprinfo.GetPassportCurveList();
		
		
	/// �������
	if (DETECT_OPTIONS.GetOption(L"COORDINATES_CORRECTION_MARKERS").Compare(OPTION_FACT))
	{
		markers = aprinfo.GetMarkers();
	}
	else
	{		
		for (size_t i=0;i<km_list.size();++i)
		{
			markers.AddItem(VWayMarker(BAD_INDEX, km_list.at(i).base, km_list.at(i).km, MARKER_KM, 0, 0, VTime()));
		}
		for (size_t i=0;i<curve_list.size();++i)
		{
			markers.AddItem(VWayMarker(BAD_INDEX, curve_list.at(i).base, M_PI*10000*curve_list.at(i).angle/180, MARKER_CURVE_BOUND_START, 0, 0, VTime()));
			markers.AddItem(VWayMarker(BAD_INDEX, curve_list.at(i).end, M_PI*10000*curve_list.at(i).angle/180, MARKER_CURVE_BOUND_END, 0, 0, VTime()));
		}
	}
	// �������� ��������
	double max_interval=GetMaxMarkerInterval(pro::interval<double>(aprinfo.GetAbsCoordBase(), aprinfo.GetAbsCoordEnd()));
	if (max_interval>MIN_MARKER_INTERVAL)
	{
		LOG_WARNING(boost::str(boost::wformat(L"������������ �������� ��� �������� ��������� (%d). ������������ ����� ������� ��� �������� %0.0f ������.") % GetMarkerCount() % max_interval));
	}
	
	// �������� ���������
	if (GetMaxSpeed()>140 && DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
	{
		LOG_WARNING(boost::str(boost::wformat(L"������������ �������� �� ������� ������ 140 ��/� - ������ �� ���������� ��-515 ����� ���� ������������.")));
	}
	
		
	//�������
	apr_switches=aprinfo.GetSwitchList();

	//�������	
	std::vector<VAprinfoZLRecord> zl_list_level=aprinfo.GetPassportCurveLevelZLList();

	for (size_t i=0; i<zl_list_level.size(); ++i)
	{
		point_t start(zl_list_level.at(i).base, zl_list_level.at(i).value_base);
		point_t end(zl_list_level.at(i).end, zl_list_level.at(i).value_end);
		
		if (start!=end)
			level_passport.AddLine(line_t(start,end));
	}	
	//��������
	std::vector<VAprinfoZLRecord> zl_list_rich=aprinfo.GetPassportCurveRichZLList();

	for (size_t i=0; i<zl_list_rich.size(); ++i)
	{
		point_t start(zl_list_rich.at(i).base, zl_list_rich.at(i).value_base);
		point_t end(zl_list_rich.at(i).end, zl_list_rich.at(i).value_end);
		if (start!=end)
			rich_passport.AddLine(line_t(start,end));
	}

	//��	
	std::vector<VAprinfoWidthRecord> zl_list_width=aprinfo.GetPassportCurveWidthList();
	for (size_t i=0; i<zl_list_width.size(); ++i)
	{
		point_t start(zl_list_width.at(i).base, zl_list_width.at(i).width_begin);
		point_t end(zl_list_width.at(i).end, zl_list_width.at(i).width_end);
		if (start!=end)
			width_passport.AddLine(line_t(start,end));
	}

	//������� �� ���������� ��������
	zl_list_level=aprinfo.GetFactCurveLevelZLList();
	for (size_t i=0; i<zl_list_level.size(); ++i)
	{
		point_t start(zl_list_level.at(i).base, zl_list_level.at(i).value_base);
		point_t end(zl_list_level.at(i).end, zl_list_level.at(i).value_end);
		
		if (start!=end)
			level_previous.AddLine(line_t(start,end));
	}	
	
	//�������� �� ���������� ��������
	zl_list_rich=aprinfo.GetFactCurveRichZLList();
	for (size_t i=0; i<zl_list_rich.size(); ++i)
	{
		point_t start(zl_list_rich.at(i).base, zl_list_rich.at(i).value_base);
		point_t end(zl_list_rich.at(i).end, zl_list_rich.at(i).value_end);
		if (start!=end)
			rich_previous.AddLine(line_t(start,end));
	}
	if (DETECT_OPTIONS.UsePassport())
	{
		level_zero=level_passport;
		rich_left_zero=rich_passport;
		rich_right_zero=rich_passport;
	}
	else
	{
		level_zero=level_previous;
		rich_left_zero=rich_previous;
		rich_right_zero=rich_previous;
	}
	width_zero=width_passport;	
}

/// ���������� ������������ �������� �� �����������
speed_t VSituation::GetMaxSpeed() const
{
	std::vector<VAprinfoSpeedRecord> speed_list = aprinfo.GetSpeedList();
	speed_t result=speed_list.front().spd.GetMaxSpeed();
	for (size_t i=0;i<speed_list.size();++i)
	{
		result=std::max<speed_t>(result, speed_list.at(i).spd.GetMaxSpeed());
	}
	return result;
}

/// ���������� ����� �������� �� �����������
size_t VSituation::GetMarkerCount() const
{
	return markers.GetCount();
}

/// ���������� ������������ ����� ������� ����� ��������� �� �����������
double VSituation::GetMaxMarkerInterval(const pro::interval<double>& in_coordinates) const
{
	return markers.GetMaxInterval(in_coordinates);
}

/// ���������� �������� ���������� ����������
const VAprinfo& VSituation::GetAprinfo() const
{
	return aprinfo;
}

/// ���������� ���������� ���������� ���������
VAprinfoKmRecord VSituation::GetNearestKm(double in_coord) const
{
	struct comp
	{
		double coord;
		comp(double in_coord) : coord(in_coord){};
		bool operator()(const VAprinfoKmRecord& l, const VAprinfoKmRecord& r)
		{
			return fabs(l.base-coord)<fabs(r.base-coord);
		}
	};			
	return *(std::min_element(km_list.begin(), km_list.end(), comp(in_coord)));
}

/// ���������� ���������� ��������� �������
size_t VSituation::GetAprSwitchCount() const
{
	return apr_switches.size();
}

/// ���������� ��������� �������
const VAprinfoSwitchRecord& VSituation::GetAprSwitch(size_t in_index) const
{
	return apr_switches.at(in_index);
}

/// ��������� ������� �������������� ��������
void VSituation::AddBadPassport(const VBadPassport& in_bad_passport)
{	
	bad_passport.push_back(in_bad_passport);
}

/// ���������� ������� �������������� ��������
const VBadPassport& VSituation::GetBadPassport(size_t in_index) const
{
	return bad_passport.at(in_index);
}

/// ���������� ���������� �������� �������������� ��������
size_t VSituation::GetBadPassportSize() const
{
	return bad_passport.size();
}

/// ���������� ������� ������������� ���������� ������
bool VSituation::BadPassport(double in_coord) const
{
	std::deque<VBadPassport>::const_iterator i=std::find_if(bad_passport.begin(), bad_passport.end(), find_bad_passport(in_coord));
	return  i != bad_passport.end() && i->IsBad();
};

/// ���������� ������� ������������� ���������� ������
bool VSituation::BadPlanPassport(double in_coord) const
{
	std::deque<VBadPassport>::const_iterator i=std::find_if(bad_passport.begin(), bad_passport.end(), find_bad_passport(in_coord));
	return  i != bad_passport.end() && i->IsBadPlan();
};

/// ���������� ������� ������������� ���������� ������
bool VSituation::BadLevelPassport(double in_coord) const
{
	std::deque<VBadPassport>::const_iterator i=std::find_if(bad_passport.begin(), bad_passport.end(), find_bad_passport(in_coord));
	return  i != bad_passport.end() && i->IsBadLevel();
};

/// ���������� ������� ������������� ���������� ������
bool VSituation::BadCoordinatePassport(double in_coord) const
{
	std::deque<VBadPassport>::const_iterator i=std::find_if(bad_passport.begin(), bad_passport.end(), find_bad_passport(in_coord));
	return  i != bad_passport.end() && i->IsBadCoordinates();
};

/// ��������� �������
void VSituation::AddSwitch(double in_coord, side_t in_side)
{
	// ���� � Aprinfo ��������� ������� FixMe!!! ����� ������ ��������� ������� � �������
	struct nearest
	{
		double coord;
		side_t side;
		nearest(double in_coord, side_t in_side) : coord(in_coord), side(in_side){};
		bool operator()(const VAprinfoSwitchRecord& l, const VAprinfoSwitchRecord& r)
		{
			return l.side==side && fabs(l.GetCoordO()-coord)<fabs(r.GetCoordO()-coord);
		}
	};
	
	std::vector<VAprinfoSwitchRecord>::iterator apr_sw=std::min_element(apr_switches.begin(), apr_switches.end(), nearest(in_coord, in_side));
	
	// ���� ���������� ���������� �������:
	if (apr_sw!=apr_switches.end() && fabs((*apr_sw).GetCoordO()-in_coord)<
		DETECT_OPTIONS.GetOption(L"MAX_SWITCH_CORRECTION_DISTANCE").GetValue<double>())
	{
		VAprinfoSwitchRecord sw=*apr_sw;
		double correction=in_coord-sw.GetCoordO();
		sw.CorrectCoord(correction);
		switches.push_back(sw);
		apr_switches.erase(apr_sw);				
		AddMessage(VRTMessage(sw.base, boost::str(boost::wformat(L"��������� ������� %s (%0.0f �)") % string_to_wstring(sw.num)  % correction)));
	}
	else
	{
		VAprinfoSwitchRecord sw;		
		sw.o=in_coord;
		sw.x=in_coord;
		sw.base=in_coord-OPTICAL_SWITCH_LENGTH/2;
		sw.end=in_coord+OPTICAL_SWITCH_LENGTH/2;
		sw.dir=SDIR_UNDEF;
		sw.side=in_side;
		switches.push_back(sw);		
	}
}

/// ���������� �������
const VAprinfoSwitchRecord& VSituation::GetSwitch(size_t in_index) const
{
	return switches.at(in_index);
}

/// ���������� ���������� �������
size_t VSituation::GetSwitchCount() const
{
	return switches.size();
}

/// ���������� ������� �������
bool VSituation::OnSwitch(const pro::interval<double>& in_range) const
{	
	struct pred1
	{
		pro::interval<double> range;
		pred1(const pro::interval<double>& in_range) : range(in_range) {};
		bool operator()(const VAprinfoSwitchRecord& in_switch) {return in_switch.GetRange().intersect(range);};
	};
	return
		(std::find_if(apr_switches.begin(), apr_switches.end(), pred1(in_range)) != apr_switches.end()) ||
		(std::find_if(switches.begin(), switches.end(), pred1(in_range)) != switches.end());
};

/// ��������� ���������
void VSituation::AddMessage(const VRTMessage& in_message)
{
	messages.push_back(in_message);
}	

/// ���������� ���������� ���������
size_t VSituation::GetMessageCount() const
{
	return messages.size();
}

/// ���������� ���������
const VRTMessage& VSituation::GetMsg(size_t in_index) const
{
	return messages.at(in_index);
}

/// ���������� �������� �� ���������� ����� (��������)
double VSituation::GetPassportRich(double in_coord) const
{
	return rich_passport.GetY(in_coord);
}

/// ���������� �������� �� ���������� ����� (�������)
double VSituation::GetPassportLevel(double in_coord) const
{
	return level_passport.GetY(in_coord);
}

/// ���������� �������� �� ������� ����� (�������)
double VSituation::GetLevel(double in_coord) const
{
	return level_zero.GetY(in_coord);
}

/// ���������� �������� �� ������� ����� (�������� �����)
double VSituation::GetRichLeft(double in_coord) const
{
	return rich_left_zero.GetY(in_coord);
}

/// ���������� �������� �� ������� ����� (�������� ������)
double VSituation::GetRichRight(double in_coord) const
{
	return rich_right_zero.GetY(in_coord);
}

/// ���������� �������� �� ������� ����� (��)
double VSituation::GetWidth(double in_coord) const
{
	return width_zero.GetY(in_coord);
}

/// ������������ ������� ����� (�������)
void VSituation::CorrectLevelTransition(const std::deque<point_t>& in_points)
{
	size_t index=level_zero.FindLine((in_points.front().x+in_points.back().x)/2);
	if (level_zero.GetLine(index).k()!=0 && fabs(in_points.front().x-in_points.back().x)>20.0)
	{
		lsq_calculator lsq;
		for (size_t i=0;i<in_points.size();++i)
		{
			lsq.put(in_points.at(i));
		}
		AdjustLine(level_zero, index, lsq.get());		
	}
}

/// ������������ ������� ����� (��������)
void VSituation::CorrectRichLeftTransition(const std::deque<point_t>& in_points)
{
	size_t index=rich_left_zero.FindLine((in_points.front().x+in_points.back().x)/2);
	if (rich_left_zero.GetLine(index).k()!=0 && fabs(in_points.front().x-in_points.back().x)>20.0)
	{
		lsq_calculator lsq;
		for (size_t i=0;i<in_points.size();++i)
		{
			lsq.put(in_points.at(i));
		}
		AdjustLine(rich_left_zero, index, lsq.get());
	}
}

/// ������������ ������� ����� (�������� ������)
void VSituation::CorrectRichRightTransition(const std::deque<point_t>& in_points)
{
	size_t index=rich_right_zero.FindLine((in_points.front().x+in_points.back().x)/2);
	if (rich_right_zero.GetLine(index).k()!=0 && fabs(in_points.front().x-in_points.back().x)>20.0)
	{
		lsq_calculator lsq;
		for (size_t i=0;i<in_points.size();++i)
		{
			lsq.put(in_points.at(i));
		}
		AdjustLine(rich_right_zero, index, lsq.get());
	}
}

/// ������������ ������� ����� (��)
void VSituation::CorrectWidthTransition(const std::deque<point_t>& in_points)
{
	//�� �������������� ������
}

/// ������������ ������� ����� (�������)
void VSituation::CorrectLevelRadius(const std::deque<point_t>& in_points)
{
	size_t index=level_zero.FindLine((in_points.front().x+in_points.back().x)/2);
	if (level_zero.GetLine(index).b()!=0 && fabs(in_points.front().x-in_points.back().x)>20.0)
	{
		mean_calculator mean;
		for (size_t i=0;i<in_points.size();++i)
		{
			mean.put(in_points.at(i).y);
		}
		if (fabs(mean.get())>10)//FixMe!!! magic number
			AdjustLine(level_zero, index, line_t(0,1,-mean.get()));
	}
}

/// ������������ ������� ����� (��������)
void VSituation::CorrectRichLeftRadius(const std::deque<point_t>& in_points)
{
	size_t index=rich_left_zero.FindLine((in_points.front().x+in_points.back().x)/2);
	if (rich_left_zero.GetLine(index).b()!=0 && fabs(in_points.front().x-in_points.back().x)>20.0)
	{
		mean_calculator mean;
		for (size_t i=0;i<in_points.size();++i)
		{
			mean.put(in_points.at(i).y);
		}
		AdjustLine(rich_left_zero, index, line_t(0,1,-mean.get()));
	}
}


/// ������������ ������� ����� (�������� ������)
void VSituation::CorrectRichRightRadius(const std::deque<point_t>& in_points)
{
	size_t index=rich_right_zero.FindLine((in_points.front().x+in_points.back().x)/2);
	if (rich_right_zero.GetLine(index).b()!=0 && fabs(in_points.front().x-in_points.back().x)>20.0)
	{
		mean_calculator mean;
		for (size_t i=0;i<in_points.size();++i)
		{
			mean.put(in_points.at(i).y);
		}
		AdjustLine(rich_right_zero, index, line_t(0,1,-mean.get()));
	}
}


/// ������������ ������� ����� (��)
void VSituation::CorrectWidthRadius(const std::deque<point_t>& in_points)
{
	//�� �� �������� �������������� ������
}

/// ���������� ����� ����� ������� ����� (�������)
size_t VSituation::GetLevelCount(PWLF_TYPE in_type)
{
	switch (in_type)
	{
		default:
		case PWLF_PASSPORT:
			return level_passport.GetPointCount();
		case PWLF_ZERO:
			return level_zero.GetPointCount();
	}
}

/// ���������� ����� ����� ������� ����� (��������)
size_t VSituation::GetRichCount(PWLF_TYPE in_type, side_t in_side)
{
	switch (in_type)
	{
		default:
		case PWLF_PASSPORT:
			return rich_passport.GetPointCount();
		case PWLF_ZERO:
			return in_side==SD_LEFT ? rich_left_zero.GetPointCount() : rich_right_zero.GetPointCount();
	}
}

/// ���������� ����� ����� ������� ����� (��)
size_t VSituation::GetWidthCount(PWLF_TYPE in_type)
{
	switch (in_type)
	{
		default:
		case PWLF_PASSPORT:
			return width_passport.GetPointCount();
		case PWLF_ZERO:
			return width_zero.GetPointCount();
	}
}

/// ���������� ����� ������� ����� (�������)
point_t VSituation::GetLevelPoint(size_t in_index, PWLF_TYPE in_type) const
{	
	switch (in_type)
	{
		default:
		case PWLF_PASSPORT:
			return level_passport.GetPoint(in_index);
		case PWLF_ZERO:
			return level_zero.GetPoint(in_index);		
	}
}

/// ���������� ����� ������� ����� (��������)
point_t VSituation::GetRichPoint(size_t in_index, PWLF_TYPE in_type, side_t in_side) const
{
	switch (in_type)
	{
		default:
		case PWLF_PASSPORT:
			return rich_passport.GetPoint(in_index);
		case PWLF_ZERO:
			return in_side==SD_LEFT ? rich_left_zero.GetPoint(in_index) : rich_right_zero.GetPoint(in_index);		
	}
}

/// ���������� ����� ������� ����� (��)
point_t VSituation::GetWidthPoint(size_t in_index, PWLF_TYPE in_type) const
{
	switch (in_type)
	{
		default:
		case PWLF_PASSPORT:
			return width_passport.GetPoint(in_index);
		case PWLF_ZERO:
			return width_zero.GetPoint(in_index);
	}
}

/// ����� �������������� �������
size_t VSituation::FindMarker(const VWayMarker& in_marker) const
{
	return markers.FindEquivalentMarker(in_marker);
}

/// ���������� ������
const VWayMarker& VSituation::GetMarker(size_t in_index) const
{
	return markers.GetItem(in_index);
}

/// ���������� ��������� ������� � ����
void VSituation::DumpMarkers(const std::wstring& in_filename) const
{
	std::ofstream out(in_filename.c_str());
	markers.Dump(out);
}

///���������� ������� ����������
VWayCoord VSituation::GetRealCoord(double in_abscoord) const 
{
	return aprinfo.GetRealCoord(in_abscoord);
};
/// ���������� �������� �� �������
VSpeeds VSituation::GetCombinedSpeeds(double in_abscoord) const 
{
	return aprinfo.GetCombinedSpeeds(in_abscoord);
};
/// ���������� �������� �� ������� � ������ ������ ������
VSpeeds VSituation::GetCurveSpeeds(double in_abscoord) const 
{
	return aprinfo.GetPassportCurveSpeeds(in_abscoord);
};
/// ���������� �������� �� �������
VSpeeds VSituation::GetSpeeds(double in_abscoord) const 
{
	return aprinfo.GetSpeeds(in_abscoord);
};
/// ��������� ����������� �������� �� �������
VSpeeds VSituation::GetMinSpeeds(double in_start, double in_end) const 
{
	return aprinfo.GetMinSpeeds(in_start, in_end);
}; 	
/// ���������� ������ ������
double VSituation::GetCurveRadius(double in_abscoord) const 
{
	return aprinfo.GetPassportCurveRadius(in_abscoord);
};
/// ���������� ������� �������������� �������
bool VSituation::GetChamJoint(double in_abscoord) const 
{
	return aprinfo.GetChamJoint(in_abscoord);
};
/// ��������� ��� ����
long VSituation::GetSpala(double in_abscoord) const 
{
	return aprinfo.GetSpala(in_abscoord);
};
/// ���������� ������� ����� ��� �������� ����
bool VSituation::GetBridge(double in_abscoord) const 
{
	return aprinfo.GetBridge(in_abscoord)!=SZO_NONE && aprinfo.GetBridgeLength(in_abscoord)>=MIN_BRIDGE_LENGTH;
};
/// ���������� ������� ������� ��� �������� ����
bool VSituation::GetTonnel(double in_abscoord) const 
{
	return aprinfo.GetTonnel(in_abscoord)!=SZO_NONE && aprinfo.GetTonnelLength(in_abscoord)>=MIN_BRIDGE_LENGTH;
};
/// ���������� ������� ������
bool VSituation::IsCurvePlan(double in_abscoord) const 
{
	if (DETECT_OPTIONS.UsePassport())
		return aprinfo.IsPassportCurvePlan(in_abscoord);
	else
		return aprinfo.IsFactCurvePlan(in_abscoord);
};
/// ���������� ���������� ������
long VSituation::GetCurveOrientation(double in_abscoord) const 
{
	if (DETECT_OPTIONS.UsePassport())
		return aprinfo.GetPassportCurveOrientation(in_abscoord);
	else
		return aprinfo.GetFactCurveOrientation(in_abscoord);
};
/// ���������� ������� ������ (�������) FixMe!!! obsolete
bool VSituation::IsCurveLevel(double in_abscoord) const 
{
	return aprinfo.IsPassportCurveLevel(in_abscoord);
};
/// ���������� ����������� ���� 
long VSituation::GetSummaryThreadSide(double in_abscoord) const 
{
	if (DETECT_OPTIONS.UsePassport())
		return aprinfo.GetPassportCurveSummaryThreadSide(in_abscoord);
	else
		return aprinfo.GetFactCurveSummaryThreadSide(in_abscoord);
};
/// ���������� ������� ���������
long VSituation::GetHoleDiameter(double in_abscoord) const 
{
	return aprinfo.GetHoleDiameter(in_abscoord);
};
/// ���������� ������� ������������ �������
bool VSituation::IsControlArea(double in_abscoord) const 
{
	return aprinfo.IsControlArea(in_abscoord);
};
/// ���������� �� �� �� ���������� �������
float VSituation::GetControlAreaWidth(double in_abscoord) const 
{
	return aprinfo.GetControlAreaWidth(in_abscoord);
};
/// ���������� ��� �� �� ���������� �������
float VSituation::GetControlAreaWidthSKO(double in_abscoord) const 
{
	return aprinfo.GetControlAreaWidthSKO(in_abscoord);
};
/// ���������� �� ������ �� ����������� �������
float VSituation::GetControlAreaLevel(double in_abscoord) const 
{
	return aprinfo.GetControlAreaLevel(in_abscoord);
};
/// ���������� ��� ������ �� ����������� �������
float VSituation::GetControlAreaLevelSKO(double in_abscoord) const 
{
	return aprinfo.GetControlAreaLevelSKO(in_abscoord);
};
/// ���������� ���������� �����
double VSituation::GetNormaWear(double in_abscoord) const 
{
	return aprinfo.GetPassportCurveNormaWear(in_abscoord);
};
/// ���������� ��� ������� �����
ZLINE_TYPE VSituation::GetCurveStatusPlan(double in_abscoord) const 
{
	if (DETECT_OPTIONS.UsePassport())
		return aprinfo.GetPassportCurveStatusPlan(in_abscoord);
	else
		return aprinfo.GetFactCurveStatusPlan(in_abscoord);
};
/// ���������� ��� ������� ����� (�������)
ZLINE_TYPE VSituation::GetCurveStatusLevel(double in_abscoord) const 
{
	if (DETECT_OPTIONS.UsePassport())
		return aprinfo.GetPassportCurveStatusLevel(in_abscoord);
	else
		return aprinfo.GetFactCurveStatusLevel(in_abscoord);
};
/// ���������� ����� ��
long VSituation::GetPchNum(double in_abscoord) const 
{
	return aprinfo.GetPchNum(in_abscoord);
};
/// ���������� ��������
VAprinfoKmRecord VSituation::GetKm(double in_abscoord) const 
{
	return aprinfo.GetKm(in_abscoord);
};
/// ���������� �������� �����������
std::string VSituation::GetDirName() const 
{
	return aprinfo.GetDirName();
};
/// ���������� ��� �����������
long VSituation::GetDirCode() const 
{
	return aprinfo.GetDirCode();
};
/// ���������� ����� ����
std::string VSituation::GetWayCode() const 
{
	return aprinfo.GetWayCode();
};
/// ���������� �������
std::string VSituation::GetPeregon(double in_abscoord) const 
{
	return aprinfo.GetPeregon(in_abscoord);
};
/// ���������� ������ ���������� ������
std::vector<VAprinfoCurve> VSituation::GetPassportCurveList(double in_start, double in_end) const 
{
	return aprinfo.GetPassportCurveList(in_start, in_end);
};
/// ���������� �����
VPicket VSituation::GetPk(double in_abscoord) const 
{
	return aprinfo.GetPk(in_abscoord);
};
/// ���������� ������ ���������
std::vector<VAprinfoSpeedRecord> VSituation::GetSpeedList(double in_start, double in_end) const 
{
	return aprinfo.GetSpeedList(in_start, in_end, false);
};
/// ���������� ������ ��������� �����������
std::vector<VAprinfoSpeedRecord> VSituation::GetTmpSpeedList(double in_start, double in_end) const 
{
	return aprinfo.GetTmpSpeedList(in_start, in_end, false);
};
/// ���������� ������ ��������� � ���������� �������
std::vector<VAprinfoPntRecord> VSituation::GetPntList(double in_start, double in_end) const 
{
	return aprinfo.GetPntList(in_start, in_end);
};
/// ���������� ������ ��
std::vector<VAprinfoPchRecord> VSituation::GetPchList(double in_start, double in_end) const 
{
	return aprinfo.GetPchList(in_start, in_end, false);
};
/// ���������� ������ ��������
std::vector<VAprinfoRepairRecord> VSituation::GetRepairs(double in_start, double in_end) const 
{
	return aprinfo.GetRepairs(in_start, in_end);
};
/// ���������� ������ ������
std::vector<VAprinfoWobRecord> VSituation::GetWobLeftList(double in_start, double in_end) const 
{
	return aprinfo.GetWobLeftList(in_start, in_end);
};
/// ���������� ������ ������
std::vector<VAprinfoWobRecord> VSituation::GetWobRightList(double in_start, double in_end) const 
{
	return aprinfo.GetWobRightList(in_start, in_end);
};
/// ���������� ������ �������� ����� �������
std::vector<VAprinfoRailRecord> VSituation::GetRailsTypeList(double in_start, double in_end) const 
{
	return aprinfo.GetRailsTypeList(in_start, in_end);
};
/// ���������� ������ ������� �� ���������
std::vector<VPicket> VSituation::GetPkList(long in_km) const 
{
	return aprinfo.GetPkList(in_km);
};
/// ���������� ������ �������� �������
std::vector<VAprinfoTestrunRecord> VSituation::GetTestrunList(double in_start, double in_end) const 
{
	return aprinfo.GetTestrunList(in_start, in_end);
};
/// ���������� ������� �������
bool VSituation::IsStation(double in_abscoord) const 
{
	return aprinfo.GetStationCode(in_abscoord)!=0;
};
/// ���������� true ���� ��� ������� �50
bool VSituation::P50(double in_abscoord) const 
{
	return aprinfo.GetRailsType(in_abscoord)==RAIL_R50;
};
/// ���������� ��� ������� �������
MACHINE_TYPE VSituation::GetTestRun(double in_abscoord) const 
{
	return aprinfo.GetTestrunType(in_abscoord);
};
/// ���������� ������� �����
bool VSituation::GetWobLeft(double in_abscoord) const 
{
	return aprinfo.GetWobLeft(in_abscoord);
};
/// ���������� ������� �����
bool VSituation::GetWobRight(double in_abscoord) const 
{
	return aprinfo.GetWobRight(in_abscoord);
};
/// ���������� ��������� ����� ���������
double VSituation::GetKmLength(long in_km) const 
{
	return aprinfo.GetKmLength(in_km);
};

/// ���������� true ���� � ����� ��������� ������� ��� ����������� ���
bool VSituation::IsPrj(double in_coord)
{
	if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_515))
	{
		double radius=GetCurveRadius(in_coord);
		int spala=GetSpala(in_coord);
		VSpeeds speeds=GetCombinedSpeeds(in_coord);
		return (speeds.GetEmptySpeed()>60) &&
			   (radius < 850) &&
		       (spala == SPALA_WOOD) ||
		       (radius < 650);
	}
	else if (DETECT_OPTIONS.GetOption(L"INSTRUCTION").Compare(OPTION_EXPERIMENT))
	{
		double radius=GetCurveRadius(in_coord);
		VSpeeds speeds=GetCombinedSpeeds(in_coord);
		return (speeds.GetEmptySpeed()>60) &&
			   (radius < 850);
	}
	else
		return false;
};