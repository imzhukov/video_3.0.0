#include "types.h"

#include <sstream>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

speed_t::speed_t() : spd(NO_SPEED)
{}

speed_t::speed_t(int in_spd) : spd(in_spd)
{}

speed_t::speed_t(const char * in_str) : spd(NO_SPEED)
{
	std::stringstream ss;
	ss << in_str;
	ss >> this->spd;
}

speed_t::speed_t(const std::string & in_str) : spd(NO_SPEED)
{
	std::stringstream ss;
	ss << in_str;
	ss >> this->spd;
}

speed_t::speed_t(const wchar_t * in_str) : spd(NO_SPEED)
{
	std::wstringstream ss;
	ss << in_str;
	ss >> this->spd;
}

speed_t::speed_t(const std::wstring & in_str) : spd(NO_SPEED)
{
	std::wstringstream ss;
	ss << in_str;
	ss >> this->spd;
}

speed_t & speed_t::operator = (int in_spd)
{
	this->spd = in_spd;
	return *this;
}

speed_t & speed_t::operator = (const char * in_str)
{
	this->spd = NO_SPEED;
	std::stringstream ss;
	ss << in_str;
	ss >> this->spd;
	return *this;
}

speed_t & speed_t::operator = (const std::string & in_str)
{
	(*this) = in_str.c_str();
	return *this;
}

speed_t & speed_t::operator = (const wchar_t * in_str)
{
	this->spd = NO_SPEED;
	std::wstringstream ss;
	ss << in_str;
	ss >> this->spd;
	return *this;
}

speed_t & speed_t::operator = (const std::wstring & in_str)
{
	(*this) = in_str.c_str();
	return *this;
}

speed_t::operator int() const
{
	return this->spd;
}

bool speed_t::Valid() const
{
	return this->spd != NO_SPEED;
}

bool speed_t::operator < (const speed_t & in_spd) const
{
	if (this->Valid() && in_spd.Valid())	return this->spd < in_spd.spd;
	else if (this->Valid())					return true;
	else									return false;
}

bool speed_t::operator < (int in_spd) const
{
	return (*this) < speed_t(in_spd);
}

bool speed_t::operator > (const speed_t & in_spd) const
{
	if (this->Valid() && in_spd.Valid())	return this->spd > in_spd.spd;
	else if (this->Valid())					return true;
	else									return false;
}

bool speed_t::operator > (int in_spd) const
{
	return (*this) > speed_t(in_spd);
}

bool speed_t::operator != (const speed_t & in_spd) const
{
	return this->spd != in_spd.spd;
}

bool speed_t::operator != (int in_spd) const
{
	return (*this) != speed_t(in_spd);
}

bool speed_t::Decrease(int in_step)
{
	if (!this->Valid())			return false;
	if (this->spd > in_step)	this->spd -= in_step;
	else						this->spd = 0;
	return true;
}

std::wstring speed_t::format() const
{
	std::wstringstream msg;
	if (this->Valid()) msg << this->spd;
	else msg << L"-";
	return msg.str();
}

namespace std
{
	template <> const speed_t & max<speed_t>(const speed_t & s1, const speed_t & s2)
	{
		return (s1 > s2) ? s1 : s2;
	}
};

VSpeeds::VSpeeds() 
{}

VSpeeds::VSpeeds(const speed_t & in_spd_pass, 
				 const speed_t & in_spd_truck, 
				 const speed_t & in_spd_empty) : 
spd_pass(in_spd_pass), spd_truck(in_spd_truck), spd_empty(in_spd_empty)
{}

VSpeeds::VSpeeds(const speed_t & in_spd_pass, 
				 const speed_t & in_spd_truck, 
				 const speed_t & in_spd_empty, 
				 const speed_t & in_spd_hi, 
				 const speed_t & in_spd_hi_bt) :
spd_pass(in_spd_pass), spd_truck(in_spd_truck), spd_empty(in_spd_empty),
spd_hi(in_spd_hi), spd_hi_bt(in_spd_hi_bt)
{
	int Z = 0;
}

VSpeeds::VSpeeds(const char * in_spd)
{
	std::vector<std::string> spds;
	boost::algorithm::split(spds, in_spd, boost::is_any_of("/"));
	if (spds.size() == 3)
	{
		(*this) = VSpeeds(speed_t(spds[0]), speed_t(spds[1]), speed_t(spds[2]));
	}
	else if (spds.size() == 5)
	{
		(*this) = VSpeeds(speed_t(spds[2]), speed_t(spds[3]), speed_t(spds[4]),
			speed_t(spds[1]), speed_t(spds[0]));
	}
}

VSpeeds::VSpeeds(const wchar_t * in_spd)
{
	std::vector<std::wstring> spds;
	boost::algorithm::split(spds, in_spd, boost::is_any_of(L"/"));
	if (spds.size() == 3)
	{
		(*this) = VSpeeds(speed_t(spds[0]), speed_t(spds[1]), speed_t(spds[2]));
	}
	else if (spds.size() == 5)
	{
		(*this) = VSpeeds(
			speed_t(spds[2]), speed_t(spds[3]), speed_t(spds[4]), 
			speed_t(spds[1]), speed_t(spds[0]));
	}
}

VSpeeds & VSpeeds::operator = (const char * in_spd)
{
	std::vector<std::string> spds;
	boost::algorithm::split(spds, in_spd, boost::is_any_of("/"));
	if (spds.size() == 3)
	{
		(*this) = VSpeeds(speed_t(spds[0]), speed_t(spds[1]), speed_t(spds[2]));
	}
	else if (spds.size() == 5)
	{
		(*this) = VSpeeds(
			speed_t(spds[2]), speed_t(spds[3]), speed_t(spds[4]), 
			speed_t(spds[1]), speed_t(spds[0]));
	}
	return (*this);
}

VSpeeds & VSpeeds::operator = (const std::string & in_spd)
{
	(*this) = in_spd.c_str();
	return (*this);
}

VSpeeds & VSpeeds::operator = (const wchar_t * in_spd)
{	
	std::vector<std::wstring> spds;
	boost::algorithm::split(spds, in_spd, boost::is_any_of(L"/"));
	if (spds.size() == 3)
	{
		(*this) = VSpeeds(speed_t(spds[0]), speed_t(spds[1]), speed_t(spds[2]));
	}
	else if (spds.size() == 5)
	{
		(*this) = VSpeeds(
			speed_t(spds[2]), speed_t(spds[3]), speed_t(spds[4]), 
			speed_t(spds[1]), speed_t(spds[0]));
	}
	return (*this);
}

VSpeeds & VSpeeds::operator = (const std::wstring & in_spd)
{
	(*this) = in_spd.c_str();
	return (*this);
}

// Возвращает true если скорости валидны
bool VSpeeds::Valid() const
{
	return this->spd_pass.Valid() && this->spd_truck.Valid() && this->spd_empty.Valid();
}

// Возвращает скорости
const speed_t & VSpeeds::GetPassSpeed()   const { return this->spd_pass;  }
const speed_t & VSpeeds::GetTruckSpeed()  const { return this->spd_truck; }
const speed_t & VSpeeds::GetEmptySpeed()  const { return this->spd_empty; }
const speed_t & VSpeeds::GetHighSpeed()   const { return this->spd_hi;    }
const speed_t & VSpeeds::GetHighBTSpeed() const { return this->spd_hi_bt; }

// Возвращает максимальную скорость
const speed_t & VSpeeds::GetMaxSpeed() const
{
//	speed_t spd = this->spd_hi;
//	spd = std::max<speed_t>(spd, spd_hi_bt);
//	spd = std::max<speed_t>(spd, spd_pass);
//	spd = std::max<speed_t>(spd, spd_truck);
//	spd = std::max<speed_t>(spd, spd_empty);
//	return spd;

	return
		std::max<speed_t>(this->spd_hi,
		std::max<speed_t>(this->spd_hi_bt,
		std::max<speed_t>(this->spd_pass,
		std::max<speed_t>(this->spd_truck, this->spd_empty))));
}

// Устанавливает скорости 
void VSpeeds::SetPassSpeed(const speed_t & spd)   { this->spd_pass = spd;  }
void VSpeeds::SetTruckSpeed(const speed_t & spd)  { this->spd_truck = spd; }
void VSpeeds::SetEmptySpeed(const speed_t & spd)  { this->spd_empty = spd; }
void VSpeeds::SetHighSpeed(const speed_t & spd)   { this->spd_hi = spd;    }
void VSpeeds::SetHighBTSpeed(const speed_t & spd) { this->spd_hi_bt = spd; }

// Возвращает строковое представление
std::wstring VSpeeds::format_short() const
{
	std::wstringstream msg;
	if (this->spd_hi.Valid() || this->spd_hi_bt.Valid())
	{
		msg << this->spd_hi_bt.format() << L"/";
		msg << this->spd_hi.format() << L"/";
	}
	msg << this->spd_pass.format() << L"/";
	msg << this->spd_truck.format() << L"/";
	msg << this->spd_empty.format();
	return msg.str();
}

// Ограничивает скорость
bool VSpeeds::RestrictSpeed(const VSpeeds& in_restriction)
{
	bool changed = this->spd_pass > in_restriction.spd_pass ||
		this->spd_truck > in_restriction.spd_truck ||
		this->spd_empty > in_restriction.spd_empty ||
		this->spd_hi > in_restriction.spd_hi ||
		this->spd_hi_bt > in_restriction.spd_hi_bt;

	//Ограничиваем скорость
	this->spd_pass  = std::min<speed_t>(this->spd_pass,  in_restriction.spd_pass);
	this->spd_truck = std::min<speed_t>(this->spd_truck, in_restriction.spd_truck);
	this->spd_empty = std::min<speed_t>(this->spd_empty, in_restriction.spd_empty);
	this->spd_hi    = std::min<speed_t>(this->spd_hi,	  in_restriction.spd_hi);
	this->spd_hi_bt = std::min<speed_t>(this->spd_hi_bt, in_restriction.spd_hi_bt);

	return changed;
};

/// Ограничивает скорость
bool VSpeeds::RestrictMaxSpeed(const speed_t & in_speed)
{
	//FixMe!!!
	if (GetMaxSpeed()>in_speed)
	{
		DecreaseSpeed(GetMaxSpeed()-in_speed);
		return true;
	}
	return false;
}

// Ограничивает скорость
bool VSpeeds::RestrictEmptySpeed(const speed_t & in_speed)
{
	bool changed = this->spd_empty > in_speed;
	this->spd_empty = std::min<speed_t>(this->spd_empty, in_speed);
	return changed;
}

// Ограничивает скорость
void VSpeeds::DecreaseSpeed(int in_step)
{
	speed_t cur = this->GetMaxSpeed();
	if (!cur.Valid()) return;

	cur.Decrease(in_step);

	if (this->spd_hi > cur)		this->DecreaseHighSpeed(this->spd_hi - cur);
	if (this->spd_hi_bt > cur)	this->DecreaseHighBTSpeed(this->spd_hi_bt - cur);
	if (this->spd_pass > cur)	this->DecreasePassSpeed(this->spd_pass - cur);
	if (this->spd_truck > cur)	this->DecreaseTruckSpeed(this->spd_truck - cur);
	if (this->spd_empty > cur)	this->DecreaseEmptySpeed(this->spd_empty - cur);
}

// Снижает высокую скорость
void VSpeeds::DecreaseHighSpeed(int in_step)
{
	this->spd_hi.Decrease(in_step);
}

// Снижает высокую скорость (накл.кузова)
void VSpeeds::DecreaseHighBTSpeed(int in_step)
{
	this->spd_hi_bt.Decrease(in_step);
}

// Снижает пассажирскую скорость
void VSpeeds::DecreasePassSpeed(int in_step)
{
	this->spd_pass.Decrease(in_step);

	if (this->spd_pass > 120)
	{
		this->spd_truck = std::min<speed_t>(this->spd_truck, 90);
	}
	else if (this->spd_pass > 80)
	{
		this->spd_truck = std::min<speed_t>(this->spd_truck, 80);
	}
	else
	{
		this->spd_truck = std::min<speed_t>(this->spd_truck, this->spd_pass);
	}
	this->spd_empty = std::min<speed_t>(this->spd_empty, this->spd_truck);	
}

// Снижает грузовую скорость 
void VSpeeds::DecreaseTruckSpeed(int in_step)
{
	if (!this->spd_truck.Valid()) return;
	this->spd_truck.Decrease(in_step);
	this->spd_empty = std::min<speed_t>(this->spd_empty, this->spd_truck);	
}

// Снижает скорость порожняка
void VSpeeds::DecreaseEmptySpeed(int in_step)
{
	this->spd_empty.Decrease(in_step);
}

// Оператор !=
bool VSpeeds::operator != (const VSpeeds & in_speeds) const
{
	return 
		this->spd_pass  != in_speeds.spd_pass || 
		this->spd_truck != in_speeds.spd_truck || 
		this->spd_empty != in_speeds.spd_empty ||
		this->spd_hi    != in_speeds.spd_hi ||
		this->spd_hi_bt != in_speeds.spd_hi_bt;
}

// Оператор <
bool VSpeeds::operator < (const VSpeeds & in_speeds) const
{
	return 
		this->spd_pass  < in_speeds.spd_pass ||
		this->spd_truck < in_speeds.spd_truck ||
		this->spd_empty < in_speeds.spd_empty ||
		this->spd_hi    < in_speeds.spd_hi ||
		this->spd_hi_bt < in_speeds.spd_hi_bt;
}

// Комбинирует скорости выбирая минимальную
VSpeeds VSpeeds::CombineSpeeds(const VSpeeds & in_l, const VSpeeds & in_r)
{
	return VSpeeds(
		std::min<speed_t>(in_l.spd_pass,  in_r.spd_pass),
		std::min<speed_t>(in_l.spd_truck, in_r.spd_truck),
		std::min<speed_t>(in_l.spd_empty, in_r.spd_empty),
		std::min<speed_t>(in_l.spd_hi,    in_r.spd_hi),
		std::min<speed_t>(in_l.spd_hi_bt, in_r.spd_hi_bt));
}

VSpeeds VSpeeds::CombineSpeeds(const VSpeeds & in_1, const VSpeeds & in_2, const VSpeeds & in_3)
{
	return VSpeeds::CombineSpeeds(in_1, VSpeeds::CombineSpeeds(in_2, in_3));
}

/// Возвращает только ограничение
VSpeeds VSpeeds::Restriction(const VSpeeds & in_speed, const VSpeeds & in_restriction)
{
	return VSpeeds(
		in_speed.spd_pass > in_restriction.spd_pass ? in_restriction.spd_pass : NO_SPEED,
		in_speed.spd_truck > in_restriction.spd_truck ? in_restriction.spd_truck : NO_SPEED,
		in_speed.spd_empty > in_restriction.spd_empty ? in_restriction.spd_empty : NO_SPEED,
		in_speed.spd_hi > in_restriction.spd_hi ? in_restriction.spd_hi : NO_SPEED,
		in_speed.spd_hi_bt > in_restriction.spd_hi_bt ? in_restriction.spd_hi_bt : NO_SPEED);
}

// Оператор =
VRouteSection & VRouteSection::operator = (const VRouteSection & in_source)
{
	dirCode = in_source.GetCode();
	wayName = in_source.wayName;
	section = in_source.section;
	return *this;
};	

// Оператор==
bool VRouteSection::operator == (const VRouteSection & in_right) const
{
	return dirCode == in_right.GetCode() &&
		wayName == in_right.wayName &&
		section == in_right.section;
};

