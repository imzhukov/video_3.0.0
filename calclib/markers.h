#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include "options.h"
#include "timelib.h"
#include "dblib.h"
#include "types.h"
#include "VFileName.h"
#include "interval.h"

class VDataSet;

/// ���� ��������
enum WAY_MARKER_TYPE {
	MARKER_NONE=0, 
	MARKER_KM=1, 
	MARKER_CURVE_BOUND_START=2, 
	MARKER_CURVE_BOUND_END=3, 
	MARKER_SWITCH=4, 
	MARKER_RFID=5
};

/// ���������� true, ���� ��� �������� ��������
bool AllowedMarkerType(WAY_MARKER_TYPE in_type);

/// ������
class VWayMarker
{
private:
	/// ������ �����
	size_t index;
	/// ���������� ���������� 
	double abscoord;
	/// �������������
	long id;
	/// ���
	WAY_MARKER_TYPE type;
	/// ������, �������
	double latitude, longitude;
	/// ����
	VTime timestamp;
public:

	/// �����������
	VWayMarker(size_t in_index, double in_abscoord, long in_id, WAY_MARKER_TYPE in_type, 
		double in_lat, double in_long, const VTime & ts);

	/// ���������� ������ �����
	size_t GetIndex() const;
	/// ���������� ���������� ����������
	double GetCoord() const;
	/// ���������� �������������
	long GetID() const;
	/// ���������� ���
	WAY_MARKER_TYPE GetType() const;
	/// ���������� ��������� ��� �������, ��� ����
	std::string GetTypeString() const;
	/// ���������� ��������
	std::wstring GetDescription() const;
	/// ���������� ������
	double GetLatitude() const;
	/// ���������� �������
	double GetLongitude() const;
	/// ���������� ����� �������
	const VTime& GetDateTime() const;
	/// ��������<
	bool operator<(const VWayMarker& in_r) const;
	/// �������� ������ ������� ����� �� ����
	std::vector<VWayMarker> GetCloseMarkers(VIBDatabasePtr db, 
		VIBTransaction & trans, __int64 way_id,
		double coord_delta, double angle_delta) const;
	/// ���������� true, ���� ������� ������������		
	bool Equivalent(const VWayMarker& in_marker) const;
	/// ���������� ���������� �� �������
	double Distance(const VWayMarker& in_marker) const;
	///�������� ������ � �����
	friend std::ostream& operator<<(std::ostream& out, const VWayMarker& in_marker)
	{
		out << in_marker.GetDescription() << L" " << in_marker.GetDateTime().ToWideStringLocale();
		return out;
	}
};

/// ���������� �������
class VWayMarkerDeviation
{
private:
	/// ������������ ������
	VWayMarker detected;
	/// ��������� ������
	VWayMarker apriore;
public:
	/// �����������
	VWayMarkerDeviation(const VWayMarker& in_detected, const VWayMarker& in_apriore);
	/// ���������� ����������
	double GetCoord() const;
	/// ���������� ����������
	double GetDeviation() const;
};

/// ������ ��������
class VWayMarkerList
{
private:
	/// ������
	std::vector<VWayMarker> markers;
	/// ������� ���� ��� ������ ���������
	bool isLoaded;
public:
	/// �����������
	VWayMarkerList();
	/// �������� ������ �� �����
	bool Load(VDataSet & ds);
	/// �������� ������ �� ����
	bool Load(VIBDatabase & db, long dirCode, const std::string & wayName, const VWaySection & section);
	/// ���������� ����� ��������
	size_t GetCount() const;
	/// ���������� ������
	const VWayMarker& GetItem(size_t in_index) const;
	/// ������� ������������� ������
	size_t FindEquivalentMarker(const VWayMarker& in_marker) const;
	/// ��������� ������
	void AddItem(const VWayMarker& in_marker);
	/// ���������� ���������� �� ����������� �������
	double GetDistance(const VWayMarker& in_marker) const;
	/// ��������� ������
	void Unload();
	/// ���������� ������������ ���������� ����� ����� ��������� �� �������� ��������� ���������
	double GetMaxInterval(const pro::interval<double>& in_coordinates) const;
	/// ��������� ������ �������� � ����
	void Dump(std::ofstream& out) const;
};

/// ���������� ������������ �����
DECLARE_OPTION(double, MARKER_IDENTIFICATION_DISTANCE, 100.0, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())
/// ���� ������������� ������
DECLARE_OPTION(double, CURVE_IDENTIFICATION_ACCURACY, 1.0, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())