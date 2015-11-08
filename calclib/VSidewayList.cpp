#include "VSidewayList.h"

// ����� ������ ����������� � ���������� �����
size_t VSidewayList::FindItem(size_t kadrNdx) const
{
	class PredicateFits
	{
		const size_t index;

	public:

		PredicateFits(size_t i) : index(i)
		{}

		bool operator () (const VSideway & rec) const
		{
			return rec.GetStartNdx() < this->index && 
				rec.GetEndNdx() > this->index;
		}
	};

	std::vector<VSideway>::const_iterator found_item = std::find_if(
		this->records.begin(), this->records.end(), PredicateFits(kadrNdx));

	if (found_item == this->records.end()) return BAD_INDEX;
	else return found_item - this->records.begin();
}

// �������� ������� �������
const wchar_t * VSideway::GetReasonDescr() const
{
	switch (this->GetReason())
	{
	case REASON_SIDEWAY:	return L"������� ����";
	case REASON_HALFCURVE:	return L"�������� ������";
	case REASON_UNDEFINED:	return L"������� ���� ��� ������������� ������";
	case REASON_WAY:		return L"����";
	default:				return L"������������";
	}
}

// �������� �������
bool VSideway::UpdateIndex(const VAbsCoordData & destAbs, const VAbsCoordData & srcAbs)
{
	uint32_t tmpStartNdx = this->startNdx;
	uint32_t tmpEndNdx = this->endNdx;
		
	if (destAbs.UpdateIndex(tmpStartNdx, srcAbs) && destAbs.UpdateIndex(tmpEndNdx, srcAbs))
	{
		this->startNdx = tmpStartNdx;
		this->endNdx = tmpEndNdx;
		return true;
	}
	else return false;
}

