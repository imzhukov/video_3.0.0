#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

#include "types.h"
#include "oversteps.h"

struct VPkLimit
{
	long pk;
	double begin, end;
	bool hasRestriction;
	VSpeeds restriction;
	std::wstring restrict_ovr;
	std::vector<std::wstring> types;

	VPkLimit() : pk(-1), begin(-1), end(-1), 
		hasRestriction(false)
	{}

};

class VKmResult
{
	long km;
	double begin, end;

	std::map<std::wstring, std::map<long, long> > ovr_counter;

	long count, amount;
	long prk_speed;
	long restrictions;
	long cp515_restrictions;
	long excluded;
	VPenaltyCalc penalty;
	long mark;
	std::vector<VPkLimit> pks;
	long cp515_revision;
	
	public:

        VKmResult(long __km, double __beg, double __end);
        bool Apply(const VAprinfo & aprInfo, const VOverstep3 & ovr);
        void ApplyOldStylePenalty();
        
        long GetKm()        const { return this->km; }
        double GetKmBegin() const { return this->begin; }
        double GetKmEnd()   const { return this->end; }
        
        long GetPenalty()    const { return this->penalty.GetPenalty(); }
        long GetCrvPenalty() const { return this->penalty.GetCrvPenalty(); }
        long GetMark()       const { return this->mark; };

        long GetCP515Restrictions() const { return this->cp515_restrictions; }
        long GetRestrictions()      const { return this->restrictions; }

        long GetOvrAmount(const std::wstring & type, long degree = 0) const;
        long GetCP515Sum2() const;
        long GetCP515Sum3() const;

        std::wstring GetComment() const;
        
        long GetCP515Revision() const { return this->cp515_revision; }
};

class VKmResultList
{
	std::vector<VKmResult> data;

public:

	size_t GetCount() const
	{
		return this->data.size();
	}

	const VKmResult & GetItem(size_t i) const
	{
		return this->data[i];
	}

	VKmResult & GetItem(size_t i)
	{
		return this->data[i];
	}

	void CreateKmList(const VAbsCoordData & absCoord, const VAprinfo & aprInfo);

	void ApplyOverstep(const VAprinfo & aprInfo, const VOverstep3 & over);
};

