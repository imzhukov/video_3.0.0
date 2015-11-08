#include "markers.h"
#include "VDataSet.h"
#include "VCalcOptions.h"


/// ���������� true, ���� ��� �������� ��������
bool AllowedMarkerType(WAY_MARKER_TYPE in_type)
{
	switch (in_type)
	{
		case MARKER_KM:
			return DETECT_OPTIONS.CoordinatesCorrectionKm();
		case MARKER_CURVE_BOUND_START:
		case MARKER_CURVE_BOUND_END:
			return DETECT_OPTIONS.CoordinatesCorrectionCurve();
		case MARKER_RFID:
			return DETECT_OPTIONS.CoordinatesCorrectionRFID();
		default:
			return false;
	}
};

//---------------------------------------- ������ ������ VWayMarker --------------------------------------------------

/// �����������
VWayMarker::VWayMarker(size_t in_index, double in_abscoord, long in_id, WAY_MARKER_TYPE in_type, 
					   double in_lat, double in_long, const VTime & ts) : 
index(in_index), abscoord(in_abscoord), id(in_id), type(in_type), latitude(in_lat), longitude(in_long), timestamp(ts)
{};

/// ���������� ������
size_t VWayMarker::GetIndex() const
{
	return index;
}

/// ���������� ����������
double VWayMarker::GetCoord() const 
{
	return abscoord;
};

/// ���������� ������
long VWayMarker::GetID() const
{
	return id;
};

/// ���������� ���
WAY_MARKER_TYPE VWayMarker::GetType() const
{
	return type;
};

std::string VWayMarker::GetTypeString() const
{
	switch(type)
	{
	case MARKER_KM: return "��"; break;
	case MARKER_CURVE_BOUND_START: return "��"; break;
	case MARKER_CURVE_BOUND_END: return "��"; break;
	case MARKER_RFID: return "RFID"; break;
	default: return "";
	}
}

/// ���������� ��������
std::wstring VWayMarker::GetDescription() const 
{
	switch (type)
	{
		case MARKER_KM:
			return boost::str(boost::wformat(L"�� ����� %d � ����������� %0.0f �") % id % abscoord);
		case MARKER_CURVE_BOUND_START:
			return boost::str(boost::wformat(L"������ ������ %d � ����������� %0.0f �") % id % abscoord);
		case MARKER_CURVE_BOUND_END:
			return boost::str(boost::wformat(L"����� ������ %d � ����������� %0.0f �") % id % abscoord);
		case MARKER_SWITCH:
			return L"�������";
		case MARKER_RFID: 
			return	boost::str(boost::wformat(L"����� RFID 0x%x � ����������� %0.0f �") % id % abscoord);
		default:
			return L"";
	};
};

/// ��������<
bool VWayMarker::operator<(const VWayMarker& in_r) const 
{
	return abscoord < in_r.abscoord;
};

/// ���������� ������
double VWayMarker::GetLatitude() const
{
	return this->latitude;
}

/// ���������� �������
double VWayMarker::GetLongitude() const
{
	return this->longitude;
}

/// ���������� ����� �������
const VTime& VWayMarker::GetDateTime() const
{
	return this->timestamp;
}

// �������� ������ ������� ����� �� ����
std::vector<VWayMarker> VWayMarker::GetCloseMarkers(VIBDatabasePtr db, 
													VIBTransaction & trans, __int64 way_id,
													double coord_delta, double angle_delta) const
{
	VIBDataHandlerParameter handler;
	std::stringstream sql;
	std::vector<VWayMarker> markers;

	if (AllowedMarkerType(this->GetType()))
	{
		long crv_index_delta = angle_t(0, angle_delta, 0).get_rad() * 10000;
		sql << "select distinct abscoord, ident_object, latitude, longitude, rail_date from msr_way_marker ";
		sql << "inner join msr_trail on msr_way_marker.msr_trail_id = msr_trail.id ";
		sql << "inner join enu_marker_type on msr_way_marker.enu_marker_type_id = enu_marker_type.id ";
		sql << "where msr_trail.adm_way_id = " << way_id << " ";
		sql << "and abscoord > " << this->GetCoord() - coord_delta << " ";
		sql << "and abscoord < " << this->GetCoord() + coord_delta << " ";
		sql << "and enu_marker_type.s_name = '" << this->GetTypeString() << "' ";

		if (this->GetType() == MARKER_KM)
		{
			sql << "and ident_object = " << this->GetID();
		}
		else if (this->GetType() == MARKER_CURVE_BOUND_START || 
			this->GetType() == MARKER_CURVE_BOUND_END)
		{
			sql << "and abs(ident_object - (" << this->GetID() << ") < " << crv_index_delta;
		}
		else if (this->GetType() == MARKER_RFID)
		{
			sql << "and ident_object = " << this->GetID();
		}
		sql << ";";

		try	
		{
			db->ExecuteStatement(trans, sql.str().c_str(), handler);
		}
		catch(...)
		{
			std::wstring error = string_to_wstring(sql.str() + "\n\n" + db->GetErrorDescription());
			LOG_ERROR(error);
		}

		for (size_t i = 0; i < handler.GetRowCount(); ++i)
		{
			markers.push_back(VWayMarker(BAD_INDEX,
				handler.GetParameter(i, 0).GetFloat(),
				handler.GetParameter(i, 1).GetInteger(),
				this->GetType(),
				handler.GetParameter(i, 2).GetFloat(),
				handler.GetParameter(i, 3).GetFloat(),
				handler.GetParameter(i, 4).GetTimestamp()));
		}
	}

	return markers;
}

/// ���������� true, ���� ������� ������������		
bool VWayMarker::Equivalent(const VWayMarker& in_marker) const
{
	if (in_marker.type==type && Distance(in_marker) < MARKER_IDENTIFICATION_DISTANCE().Value())
	{
		switch (type)
		{
			case MARKER_KM:
				return id==in_marker.id;
			case MARKER_RFID:
				return id==in_marker.id;
			case MARKER_CURVE_BOUND_START:
			case MARKER_CURVE_BOUND_END:
				return fabs(double(in_marker.id) - double(id)) < CURVE_IDENTIFICATION_ACCURACY().Value()*10000.0*M_PI/180.0;//FixMe!!! magic number
			default:
				return false;					
		}
	}
	else
		return false;
}

/// ���������� ���������� �� �������
double VWayMarker::Distance(const VWayMarker& in_marker) const
{
	return fabs(GetCoord()-in_marker.GetCoord());
}

//--------------------------------------- ������ ������ VWayMarkerDeviation ------------------------------------------

/// �����������
VWayMarkerDeviation::VWayMarkerDeviation(const VWayMarker& in_detected, const VWayMarker& in_apriore) : detected(in_detected), apriore(in_apriore)
{}

/// ���������� ����������
double VWayMarkerDeviation::GetCoord() const
{
	return detected.GetCoord();
}

/// ���������� ����������
double VWayMarkerDeviation::GetDeviation() const
{
	return detected.GetCoord()-apriore.GetCoord();
}

//-------------------------------------- ������ ������ VWayMarkerList -------------------------------------------------

/// �����������
VWayMarkerList::VWayMarkerList() : isLoaded(false) {};

/// ���������� ����� ��������
size_t VWayMarkerList::GetCount() const
{
	return markers.size();
}

/// ���������� ������
const VWayMarker& VWayMarkerList::GetItem(size_t in_index) const
{
	return markers.at(in_index);
}

/// ������� ������������� ������
size_t VWayMarkerList::FindEquivalentMarker(const VWayMarker& in_marker) const
{
	struct pred
	{
		VWayMarker marker;
		pred(const VWayMarker& in_marker) : marker(in_marker) {};
		bool operator()(const VWayMarker& in_marker) const {return marker.Equivalent(in_marker);};
	};
	std::vector<VWayMarker>::const_iterator i = find_if(markers.begin(), markers.end(), pred(in_marker));
	if (i!=markers.end())
	{		
		return size_t(i-markers.begin());
	}
	else
	{
		return BAD_INDEX;
	}
}

/// ��������� ������
void VWayMarkerList::AddItem(const VWayMarker& in_marker)
{
	struct pred
	{
		VWayMarker marker;
		pred(const VWayMarker& in_marker) : marker(in_marker) {};
		bool operator()(const VWayMarker& in_marker) const {return marker.Equivalent(in_marker);};
	};
	if (find_if(markers.begin(), markers.end(), pred(in_marker))==markers.end())
	{
		markers.push_back(in_marker);
		std::sort(markers.begin(), markers.end());
	}
}

/// ���������� ���������� �� ����������� �������
double VWayMarkerList::GetDistance(const VWayMarker& in_marker) const
{
	size_t index=FindEquivalentMarker(in_marker);
	if (index!=BAD_INDEX)
	{
		double distance=in_marker.GetCoord() - GetItem(index).GetCoord();
		return distance;
	}
	else
	{
		return DBL_MAX;
	}
}

/// ��������� ������
void VWayMarkerList::Unload()
{
	this->markers.clear();
	this->isLoaded = false;
}

/// �������� ������ �� �����
bool VWayMarkerList::Load(VDataSet & ds)
{
	if (this->isLoaded) return true;

	if (!ds.IsLoaded(PRM_ABS_COORD)) return false;
	if (!ds.IsLoaded(PRM_CURV)) return false;
	if (!ds.IsLoaded(PRM_KURS)) return false;

	const VAbsCoordData & absCoord = ds.GetAbsCoord();
	const VParameter & curv = ds.GetParameter(PRM_CURV);
	const VParameter & kurs = ds.GetParameter(PRM_KURS);
	const VDoubleParameter & gps_lat = ds.GetDoubleParameter(PRM_GPS_LATITUDE);
	const VDoubleParameter & gps_long = ds.GetDoubleParameter(PRM_GPS_LONGITUDE);
	const VOpMarkList & opMarks = ds.GetOpMarks();
	const VAprinfo & aprInfo = ds.GetAprinfo();
	size_t kadrCount = absCoord.GetCount();
	bool have_gps = ds.IsLoaded(PRM_GPS_LATITUDE) && ds.IsLoaded(PRM_GPS_LONGITUDE);

	//������
	VFindCurve curve_detector(1);
	for (size_t i = 0; i < kadrCount; ++i)
	{
		curve_detector.Put(VPoint(absCoord.GetItem(i), curv.GetItem(i)));
		while (curve_detector.Ready())
		{
			VProtoCurve e = curve_detector.Front();
			size_t start = absCoord.FindIndex(e.GetStart(), false);
			size_t end = absCoord.FindIndex(e.GetEnd(), false);
			if (start < kadrCount && end < kadrCount)	
			{
				double angle = fabs(kurs.GetItem(end) - kurs.GetItem(start));
				double length = e.GetLength();

				long markId = angle * 10000;
				if (e.GetSide() == SD_LEFT) markId *= -1;

				if (angle > angle_t(0, OPTIONS.GetOption(L"MIN_CURVE_ANGLE").GetValue<double>(), 0).get_rad() &&				
					length > OPTIONS.GetOption(L"MIN_CURVE_LENGTH").GetValue<double>())
				{	
					double lat_start = 0, long_start = 0, lat_end = 0, long_end = 0;
					if (have_gps)
					{
						lat_start = gps_lat.GetItem(start);
						long_start = gps_long.GetItem(start);
						lat_end = gps_lat.GetItem(end);
						long_end = gps_long.GetItem(end);
					}

					this->AddItem(VWayMarker(absCoord.FindIndex(curve_detector.Front().GetStart(), false), curve_detector.Front().GetStart(), 
						markId, MARKER_CURVE_BOUND_START, lat_start, long_start, ds.GetRailDate()));

					this->AddItem(VWayMarker(absCoord.FindIndex(curve_detector.Front().GetEnd(), false), curve_detector.Front().GetEnd(), 
						markId, MARKER_CURVE_BOUND_END, lat_end, long_end, ds.GetRailDate()));
				}
			}
			curve_detector.Pop();
		}
	}

	// ������� ���������
	if (ds.IsLoaded(PRM_OP_MARKS))
	{
		for (size_t i = 0; i < opMarks.GetCount(); ++i)
		{
			if (opMarks.GetItem(i).isPole)
			{
				uint32_t index = opMarks.GetItem(i).index;
				double abs_coord = absCoord.GetItem(index);

				VWayCoord wc_0 = aprInfo.GetRealCoord(abs_coord);
				wc_0.m = 0;
				double abs_0 = aprInfo.GetAbsCoord(wc_0);

				VWayCoord wc_1 = wc_0;
				double abs_1;

				while (true)
				{
					wc_1.km++;

					if (aprInfo.GetKmLength(wc_1.km) > 0)
					{
						abs_1 = aprInfo.GetAbsCoord(VWayCoord(wc_1.km, 0));
						break;
					}
				}

				// ��� ���������������� ���������
				if (fabs(abs_0 - abs_coord) > 100 && fabs(abs_1 - abs_coord) > 100)
					continue;

				long markId;
				if (fabs(abs_0 - abs_coord) < fabs(abs_1 - abs_coord))
					markId = wc_0.km;
				else
					markId = wc_1.km;

				double lat_pnt = 0, long_pnt = 0;
				if (have_gps)
				{
					lat_pnt = gps_lat.GetItem(index);
					long_pnt = gps_long.GetItem(index);
				}

				this->AddItem(VWayMarker(
				        absCoord.FindIndex(abs_coord, false), abs_coord, markId, MARKER_KM, lat_pnt, long_pnt, ds.GetRailDate()));
			}
		}
	}
	///����� RFID
	for (size_t i=0; i<ds.GetSpalaMarker().GetCount(); ++i)
	{
		VSpalaMarker spalaMarker=ds.GetSpalaMarker().GetItem(i);		
		double abs_coord = absCoord.GetItem(spalaMarker.index);
		long markId = spalaMarker.value;
		double lat_pnt = 0, long_pnt = 0;
		if (have_gps)
		{
			lat_pnt = gps_lat.GetItem(spalaMarker.index);
			long_pnt = gps_long.GetItem(spalaMarker.index);
		}
		this->AddItem(VWayMarker(
						absCoord.FindIndex(abs_coord, false), abs_coord, markId, MARKER_RFID, lat_pnt, long_pnt, ds.GetRailDate()));
	}

	this->isLoaded = true;
	return true;
}

/// �������� ������ �� ����
bool VWayMarkerList::Load(VIBDatabase & db, long dirCode, const std::string & wayName, const VWaySection & section)
{
	VIBDataHandlerParameter handler;
	std::wstring query=boost::str(boost::wformat(
	                L"SELECT a.ABSCOORD, a.IDENT_OBJECT, m.WAY_MARKER_TYPE, a.LATITUDE, a.LONGITUDE, t.DATE_TRAIL "
					L"FROM MSR_WAY_MARKER a "
					L"inner join ENU_MARKER_TYPE m on m.ID=a.ENU_MARKER_TYPE_ID "
				    L"inner join msr_trail t on t.id=a.MSR_TRAIL_ID "
					L"inner join adm_way w on w.id=t.ADM_WAY_ID "
					L"inner join ADM_DIRECTION d on d.ID=w.ADM_DIRECTION_ID "
					L"where d.CODE='%d' and w.CODE='%s' "
					L"order by t.DATE_TRAIL descending, abscoord") 
					% dirCode % string_to_wstring(wayName));
	VIBTransaction transaction=db.StartTransaction();
	db.ExecuteStatement(transaction, wstring_to_string(query).c_str(), handler);
	db.RollbackTransaction(transaction);
	for (size_t i=0; i<handler.GetRowCount(); ++i)
	{
		VWayMarker marker(BAD_INDEX, handler.GetParameter(i, 0).GetFloat(), 
		    handler.GetParameter(i, 1).GetInteger(), 
			static_cast<WAY_MARKER_TYPE>(handler.GetParameter(i, 2).GetInteger()), 
			handler.GetParameter(i, 3).GetFloat(), handler.GetParameter(i, 4).GetFloat(),
			handler.GetParameter(i, 5).GetTimestamp());
		AddItem(marker);
	}
	return true;
}


/// ���������� ������������ ���������� ����� ����� ��������� �� �������� ��������� ���������
double VWayMarkerList::GetMaxInterval(const pro::interval<double>& in_coordinates) const
{
	double result=0;
	std::deque<double> coordinates;
	coordinates.push_back(in_coordinates.left_bound().value);
	for (size_t i=0; i<markers.size(); ++i)
	{
		coordinates.push_back(markers.at(i).GetCoord());
	}
	coordinates.push_back(in_coordinates.right_bound().value);
	while (coordinates.size()>1)
	{
		double left=coordinates.front();
		coordinates.pop_front();
		double right=coordinates.front();
		result=std::max<double>(result, fabs(right-left));
	}
	return result;
}

/// ��������� ������ �������� � ����
void VWayMarkerList::Dump(std::ofstream& out) const
{
	for (size_t i=0; i<markers.size(); ++i)
	{
		out << markers.at(i) << endl;
	}
};
