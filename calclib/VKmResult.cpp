#include "VKmResult.h"

static bool IsWhiteListed(const std::wstring & type)
{
	static const std::wstring ovr_white_list[23] = { NARROW, WIDEN, LEVEL, DISTORTION, PIT_LEFT, PIT_RIGHT, RICH, RICH_NR, 
	SHORT_DISTORTION, OTVOD_WIDTH, ANP, PSI, RICH_LEFT_GROUP, RICH_RIGHT_GROUP, DISTORTION_GROUP, PIT_LEFT_GROUP, 
	PIT_RIGHT_GROUP, RICH_PIT_COMBINATION, RICH_DISTORTION_COMBINATION, ANP_AVERAGE, ANP_HORIZONTAL, PSI_HORIZONTAL, OTVOD_LEVEL };

    return std::find(ovr_white_list, ovr_white_list + 23, type) != ovr_white_list + 23;
}

VKmResult::VKmResult(long __km, double __beg, double __end) : 
km(__km), begin(__beg), end(__end), prk_speed(NO_SPEED), 
count(0), amount(0), restrictions(0), cp515_restrictions(0),
excluded(0), penalty(), mark(5), cp515_revision(0)
{
	int i = 1;
	double cur_begin = begin;
	while (cur_begin < end)
	{
		VPkLimit pk;
		pk.pk = i++;
		pk.begin = cur_begin;
		if (end - cur_begin >= 150) 
		{
			pk.end = pk.begin + 100;
		}
		else
		{
			pk.end = end;
		}
		this->pks.push_back(pk);
		cur_begin = pk.end;
	}
}

long VKmResult::GetOvrAmount(const std::wstring & type, long degree) const 
{
	std::map<std::wstring, std::map<long, long> >::const_iterator it = this->ovr_counter.find(type);
	if (it == this->ovr_counter.end()) return 0;
    std::map<long, long>::const_iterator it2 = it->second.find(degree);
    if (it2 == it->second.end()) return 0;
    return it2->second;	
}

std::wstring VKmResult::GetComment() const
{
	std::wstringstream comment;
	for (size_t i = 0; i < this->pks.size(); ++i)
	{
		if (!pks[i].restriction.Valid()) continue;

		std::wstring loc_restrict = pks[i].restriction.format_short();
		if (!comment.str().empty()) comment << L"; ";
		comment << L"V=" << loc_restrict << L" "; 
		comment << L"пк" << pks[i].pk << L" ";
		comment << pks[i].restrict_ovr;
	}

	return comment.str();
}

bool VKmResult::Apply(const VAprinfo & aprInfo, const VOverstep3 & ovr)
{
    if (!IsWhiteListed(ovr.type)) return false;

	if (ovr.is_excluded)
	{
		this->excluded++;
		return true;
	}

    this->cp515_revision = ovr.cp515_revision;

	if (this->ovr_counter.find(ovr.type) == this->ovr_counter.end())
	{
		this->ovr_counter.insert(
			std::pair<std::wstring, std::map<long, long> >(
			ovr.type, std::map<long, long>()));
	}

	std::map<long, long> & cur_ovr_type = this->ovr_counter[ovr.type];

	if (cur_ovr_type.find(ovr.degree) == cur_ovr_type.end())
	{
		cur_ovr_type.insert(std::pair<long, long>(ovr.degree, ovr.amount));
	}
	else
	{
		cur_ovr_type[ovr.degree] += ovr.amount;
	}

	this->amount += ovr.amount;
	this->count++;

	int cur_prk_spd = ovr.GetMaxSpeed();
	if (cur_prk_spd != NO_SPEED)
	{
		if (this->prk_speed == NO_SPEED)
			this->prk_speed = cur_prk_spd;
		else					
			this->prk_speed = std::min<long>(this->prk_speed, cur_prk_spd);
	}

	if (ovr.hasRestriction)	
	{
	    this->restrictions += ovr.amount;
	    
	    if (penalty.IsCP515Restriction(ovr.type))
	    {
	        this->cp515_restrictions += ovr.amount;
	    }
	}

	this->penalty.Apply(aprInfo, ovr);

	if (this->prk_speed < 60)
	{
		if (this->penalty.GetPenalty() < 6)			this->mark = 5;
		else if (this->penalty.GetPenalty() < 16)	this->mark = 4;
		else if (this->penalty.GetPenalty() < 100)	this->mark = 3;
		else							this->mark = 2;
	}
	else
	{
		if (this->penalty.GetPenalty() < 4)			this->mark = 5;
		else if (this->penalty.GetPenalty() < 12)	this->mark = 4;
		else if (this->penalty.GetPenalty() < 100)	this->mark = 3;
		else							this->mark = 2;
	}

	if (ovr.hasRestriction)
	{
		for (size_t i = 0; i < this->pks.size(); ++i)
		{
			if (ovr.absCoord >= this->pks[i].begin && ovr.absCoord <= this->pks[i].end)
			{
				VSpeeds or(ovr.rst1, ovr.rst2, ovr.rst3, ovr.rst_hi, ovr.rst_hi_bt);

				if (or < this->pks[i].restriction)
				{
					VPkLimit & pk = this->pks[i];

					pk.hasRestriction = true;
					VSpeeds ovr_rest = ovr.GetRestriction();

					if (ovr_rest < pk.restriction)
					{
						pk.restriction.RestrictSpeed(ovr_rest);
						pk.restrict_ovr = ovr.type;

						bool found = false;
						for (size_t oi = 0; oi < pk.types.size(); ++oi)
						{
							if (pk.types[oi] == ovr.type)
							{
								found = true;
								break;
							}
						}
						if (!found) this->pks[i].types.push_back(ovr.type);
					}
				}

				break;
			}
		}
	}

	return true;
}

long VKmResult::GetCP515Sum2() const
{
    long sum = 0;
    for (size_t i = 0; i < this->penalty.GetCP515Degree2List().size(); ++i)
    {
        sum += this->GetOvrAmount(this->penalty.GetCP515Degree2List().at(i), 2);
    }
    return sum;
}

long VKmResult::GetCP515Sum3() const
{
    long sum = 0;
    for (size_t i = 0; i < this->penalty.GetCP515Degree3List().size(); ++i)
    {
        sum += this->GetOvrAmount(this->penalty.GetCP515Degree3List().at(i), 3);
    }
    return sum;
}

void VKmResult::ApplyOldStylePenalty() 
{
	this->penalty.CalcOldStylePenalty(this->prk_speed, this->GetCP515Restrictions(), this->GetCP515Sum2(), this->GetCP515Sum3());
	this->mark = (this->penalty.GetPenalty() <= 10) ? 5 : ((this->penalty.GetPenalty() <= 40) ? 
		4 : ((this->penalty.GetPenalty() <= 150) ? 3 : 2));
}

void VKmResultList::CreateKmList(const VAbsCoordData & absCoord, const VAprinfo & aprInfo)
{
	this->data.clear();

	std::vector<VAprinfoKmRecord> kmList = aprInfo.GetKmList(absCoord.GetItem(0), absCoord.GetLastItem(), true);

	for (size_t i = 0; i < kmList.size(); ++i)
	{
		std::vector<VAprinfoPchRecord> pchList;
		pchList = aprInfo.GetPchList(kmList[i].base, kmList[i].end);
		for (size_t j = 0; j < pchList.size(); ++j)
		{
			if (pchList.size() > 0)
			{
				int Z = 0;
			}

			if (pchList[j].end > pchList[j].base)
			{
				this->data.push_back(VKmResult(kmList[i].km, pchList[j].base, pchList[j].end));
			}
		}
	}
}

void VKmResultList::ApplyOverstep(const VAprinfo & aprInfo, const VOverstep3 & over)
{
    if (!IsWhiteListed(over.type)) return;
	if (over.is_excluded) return;

	std::vector<size_t> kmNdx;

	// при наличии 2 записей на километр (граница пч)
	// выбираем, куда вписывать отступление
	for (size_t i = 0; i < this->data.size(); ++i)
	{
		if (over.km == this->data[i].GetKm()) kmNdx.push_back(i);
	}

	if (kmNdx.size() == 1)
	{
		this->data[kmNdx.front()].Apply(aprInfo, over);
	}

	else if (kmNdx.size() > 1)
	{
		if (over.absCoord < this->data[kmNdx.front()].GetKmEnd())
		{
			this->data[kmNdx.front()].Apply(aprInfo, over);
		}

		else if (over.absCoord > this->data[kmNdx.back()].GetKmBegin())
		{
			this->data[kmNdx.back()].Apply(aprInfo, over);
		}

		else
		{
			for (size_t i = 1; i < kmNdx.size() - 1; ++i)
			{
				if (over.absCoord < this->data[kmNdx[i]].GetKmEnd())
				{
					this->data[kmNdx[i]].Apply(aprInfo, over);
				}
			}
		}
	}
}

