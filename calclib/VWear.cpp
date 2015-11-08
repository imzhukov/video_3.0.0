#include "VWear.h"
#include "VDataSet.h"
#include "options.h"
#include "oversteps.h"

/// занулять все отрицательные значения
DECLARE_OPTION(bool, WEAR_FORCE_OVER_ZERO, true, VFileName(VFileName::GetHomeDir(), L"Analysis3\\config\\.debug").GetPath())

// Загрузка
bool VWear::Load(VDataSet & ds, bool vertical_correction)
{
	if (this->IsLoaded()) return true;

	// загрузка параметров
	if (!ds.Load(PRM_ABS_COORD) || !ds.Load(PRM_CURVE_MARKS) || !ds.Load(PRM_CORRUPT))
	{
		this->message = ERRMSG_MISSING_PARAMETER;
		return false;
	}

	VAbsCoordData * absCoord = &ds.GetAbsCoord();
	VCurveList * curves = &ds.GetCurves();

    bool usePPR = false;
    if (OPTIONS.GetOption(L"WEAR_SOURCE").Compare(OPTION_STD))
    {
        if (!ds.Load(PRM_WEAR_LEFT) || !ds.Load(PRM_WEAR_RIGHT)) 
        {
    		this->message = ERRMSG_MISSING_PARAMETER;
	    	return false;
        }
	    usePPR = false;
    }
    else if (OPTIONS.GetOption(L"WEAR_SOURCE").Compare(OPTION_PPR))
    {
        if (!ds.Load(PRM_PPR_IZN_LEFT) || !ds.Load(PRM_PPR_IZN_RIGHT)) 
        {
            if (!this->already_tried)
            {
                LOG_ERROR(L"Ошибка при загрузке износа: в файле отсутствуют измерения ППР");
                this->already_tried = true;
            }
    		this->message = ERRMSG_MISSING_PARAMETER;
	    	return false;
        }
	    usePPR = true;
    }
    else
    {
        this->message = ERRMSG_WRONG_OPTION;
        return false;
    }

	this->Create(0xFFFFFFFF, absCoord->GetCount(), 0, 0.0f);
	for (size_t crvNdx = 0; crvNdx < curves->GetCount(); ++crvNdx)
	{
		VCurve * crv = &curves->GetItem(crvNdx);		
        PARAMETER_TYPE paramType;		
    
        if (usePPR)
        {
    		paramType = (crv->GetOrientation(ds) == SD_LEFT) ? PRM_PPR_IZN_RIGHT : PRM_PPR_IZN_LEFT;
        }
        else
        {
    		paramType = (crv->GetOrientation(ds) == SD_LEFT) ? PRM_WEAR_RIGHT : PRM_WEAR_LEFT;
        }
		
		VParameter * wear = &ds.GetParameter(paramType);

		// предыдущая прямая
		uint32_t prevStartNdx = 0;
		if (crvNdx > 0)
		{
			prevStartNdx = absCoord->FindIndex(std::max<double>(
				curves->GetItem(crvNdx-1).GetRangeCoord(ds, true, false, false), 
				curves->GetItem(crvNdx-1).GetRangeCoord(ds, false, false, false)), true);
		}

		uint32_t prevEndNdx = absCoord->FindIndex(std::min<uint32_t>(
			crv->GetRangeCoord(ds, true, false, true), 
			crv->GetRangeCoord(ds, false, false, true)), true);

		// следующая кривая
		uint32_t nextStartNdx = absCoord->FindIndex(std::max<uint32_t>(
			crv->GetRangeCoord(ds, true, false, false), 
			crv->GetRangeCoord(ds, false, false, false)), true);

		uint32_t nextEndNdx = wear->GetCount();
		if (crvNdx < curves->GetCount() - 1)
		{
			nextEndNdx = absCoord->FindIndex(std::min<uint32_t>(
			curves->GetItem(crvNdx+1).GetRangeCoord(ds, true, false, true), 
			curves->GetItem(crvNdx+1).GetRangeCoord(ds, false, false, true)), true);
		}

		// коррекция относительно соседних прямых?
		float value = 0.0f;
		if (vertical_correction)
		{
			size_t count = 0;
			for (uint32_t kadrNdx = prevStartNdx; kadrNdx < prevEndNdx; ++kadrNdx)
			{
				if (!wear->IsCorrupt(kadrNdx))
				{
					value += wear->GetItem(kadrNdx);
					++count;
				}
			}

			for (uint32_t kadrNdx = nextStartNdx; kadrNdx < nextEndNdx; ++kadrNdx)
			{
				if (!wear->IsCorrupt(kadrNdx))
				{
					value += wear->GetItem(kadrNdx);
					++count;
				}
			}

			if (count > 0) 
				value /= count;
		}

		// заполняем массив износов и отбраковок 
		for (uint32_t kadrNdx = prevEndNdx; kadrNdx < nextStartNdx; ++kadrNdx)
		{
			// значение параметра
			float prm_value = wear->GetItem(kadrNdx) - value;
			if (WEAR_FORCE_OVER_ZERO().Value() && prm_value < 0) prm_value = 0;
			this->GetItem(kadrNdx) = prm_value;
			
			//Учитываем поправку для рельсов типа р50
			if (ds.GetAprinfo().GetRailsType(ds.GetAbsCoord().GetItem(kadrNdx))==RAIL_R50 && DETECT_OPTIONS.GetOption(L"USE_P50_CORRECTION").Compare(OPTION_YES))
				this->GetItem(kadrNdx)-=P50_CORRECTION_VALUE;

			// отбраковка параметра
			bool prm_corrupt = wear->IsCorrupt(kadrNdx);

			if (prm_corrupt)
			{
				this->SetCorrupt(kadrNdx, prm_corrupt);
			}
		}
	}

	this->SetLoaded(true);
	this->message = ERRMSG_OK;
	return true;
}

