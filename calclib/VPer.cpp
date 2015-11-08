#include "VDataSet.h"
#include "VPer.h"
#include "VParameter.h"

#include "VProgressInterface.h"

bool VPerKuzList::Load(VDataSet & ds, VProgressBox * prog)
{	
	if (this->IsLoaded()) 
		return true;

	// загрузка параметров
	if (!ds.Load(PRM_ABS_COORD) || !ds.Load(PRM_DIST_LONG))
	{
		this->message = ds.GetErrorMessage();
		return false;
	}

	VAbsCoordData * absCoord = &ds.GetAbsCoord();
	VAprinfo *aprinfo = &ds.GetAprinfo();
	VParameter *kuzov = &ds.GetParameter(PRM_DIST_LONG);
	VPer per_new;

	// Количество кадров
	int iCount = 0;
	// Текущее превышение

	// Кадр
    long smplspd;
	double absc = 0.0;
	double abstmp = 0.0;
	double PerK = 0.0;

	// Порог, определяется из DopUklon[] по соотв. скорости Vпз из Vpr[]
	double Porog = 0.0;
	double PerPor = 0.0;
	long SpeedPrk = 0;
	long Vpz1 = 255;
	double Per_kuz = 0.0;
	double Max = 0.0;
	int Kmn = 0;
	float Mn = 0.0;
	double abscoord = 0.0;
	double dlina = 0.0;
	int r=0;
	float Per_Kuzov = (float)1.2;

    // Запрашиваем количество кадров
	iCount = kuzov->GetCount();

	int pros = iCount/28;
	int step=0;

	if (prog)
	{
		char msg[4096] = {0};
		_snprintf(msg, 4095, "Загрузка перекосов на базе вагона");
		prog->SetMessage(0, msg);
		prog->SetRange(0, iCount);
	}

	for (int iNdx = 0; iNdx < iCount; ++iNdx)
	{
		if (prog && (iNdx % 100) == 0)
			prog->SetPosition(0, iNdx);

		step = iNdx%pros;

		// Запрашиваем текущий кадр
		PerK = kuzov->GetItem(iNdx);
		absc    = absCoord->GetItem(iNdx);
		smplspd = aprinfo->GetPassSpeed(absc);
		if(smplspd!=Vpz1)
		{
			Porog = this->GetPorog(smplspd);       // Определяем порог по Vpz1
			Porog = 19.5*Porog;
		}
		Vpz1 = smplspd;
		Per_kuz = fabs(PerK) - Per_Kuzov*Porog;
        while ((Per_kuz>0)&&(iNdx < iCount))
        {     
			if (r!=1)
			{
				dlina = absc;
				r=1;
			}

            if (Per_kuz > Max)
			{
				Max = Per_kuz;
				abscoord = absc;
				SpeedPrk = Vpz1;
				PerPor = Per_Kuzov*Porog;
			}
            ++iNdx;
			step = iNdx%pros;

			if (iNdx < iCount)
			{
				PerK = kuzov->GetItem(iNdx);
				absc    = absCoord->GetItem(iNdx);
				smplspd = aprinfo->GetPassSpeed(absc);
			}

			if (Vpz1 != smplspd)
		    {
			   Porog = this->GetPorog(smplspd);       // Определяем порог по Vpz1
		       Porog = 19.5*Porog;
		    }
		    Vpz1 = smplspd;

            Per_kuz = fabs(PerK) - Per_Kuzov*Porog;

			abstmp = absc;
        }
        if (Max!=0)
        {    
			dlina = absc - dlina;
			if (dlina >= 1)
			{
			   // Добавляем результаты превышения
			   if (Max>=0.5)
			   {
			       // Заносим результаты превышения
			       per_new.value = Max;
			       per_new.porog = PerPor;
			       per_new.speed = SpeedPrk;
			       per_new.absPer = abscoord;
			       this->records.push_back(per_new);
			   }
			}

			Max=0;
			r=0;
        }

		abstmp = absc;
       
	}
	return true;
}

float VPerKuzList::GetPorog(unsigned int Vpz)
{
   float DopUklon[17]={0.5f,0.67f,0.7f,1.0f,1.2f,1.4f,1.6f,1.7f,1.9f,2.1f,2.3f,2.5f,2.7f,2.9f,3.0f,3.1f,3.2f};
   float Spd_prikaz[17]={200.0f,160.0f,140.0f,120.0f,110.0f,100.0f,90.0f,85.0f,80.0f,75.0f,70.0f,65.0f,60.0f,55.0f,50.0f,40.0f,25.0f};
   // Определение порогового значения, выше которого - бум
   float x=0;
   for (int ii=0;ii<16;ii++)
   {
    if ( (Vpz<Spd_prikaz[ii])&&(Vpz>=Spd_prikaz[ii+1]) )
       x = DopUklon[ii+1];
   }
   if ( Vpz < Spd_prikaz[16] )
      x = DopUklon[16];
   if ( Vpz >= Spd_prikaz[0] )
      x = DopUklon[0];

   return x;
}

bool VPerTelList::Load(VDataSet & ds, VProgressBox * prog)
{	
	if (this->IsLoaded()) 
		return true;

	// загрузка параметров
	if (!ds.Load(PRM_ABS_COORD) || !ds.Load(PRM_DIST_SHORT))
	{
		this->message = ds.GetErrorMessage();
		return false;
	}

	VAbsCoordData * absCoord = &ds.GetAbsCoord();
	VAprinfo *aprinfo = &ds.GetAprinfo();
	VParameter *tel = &ds.GetParameter(PRM_DIST_SHORT);
	VPer per_new;

	// Количество кадров
	size_t iCount = 0;
	long SpeedPrk=0;
	long Vpz2=0;
	double Per_tel = 0.0;
	double Max = 0.0;
	int Kmn = 0;
	float Mn = 0.0;
	double abscoord = 0.0;
	double abstmp = 0.0;
	
	double dlina = 0.0;
	int r=0;
	int Porog = 12;

	// Кадр
    long smplspd;
	double absc = 0.0;
	float PerT = 0.0;

	// Запрашиваем количество кадров
	iCount = tel->GetCount();
	int pros = iCount/28;
	int step=0;

	if (prog)
	{
		char msg[4096] = {0};
		_snprintf(msg, 4095, "Загрузка перекосов на базе тележки");
		prog->SetMessage(0, msg);
		prog->SetRange(0, iCount);
	}

	for(unsigned int iNdx = 0; iNdx < iCount; ++iNdx)
	{	 
		if (prog && (iNdx % 100) == 0)
			prog->SetPosition(0, iNdx);

		step = iNdx%pros;
		// Запрашиваем текущий кадр
		PerT = tel->GetItem(iNdx);
		absc    = absCoord->GetItem(iNdx);
		smplspd = aprinfo->GetTruckSpeed(absc);		
		
		Per_tel = fabs(PerT);
        Vpz2 = smplspd;
	    Vpz2 > 60 ? Porog=12 : Porog=16;
		while ((Per_tel>Porog)&&(Vpz2==smplspd)&&(iNdx < iCount))
        {     
			if (r!=1)
			{
				dlina = absc;
				r=1;
			}
            if (Per_tel > Max)
			{
				Max = Per_tel;
				abscoord = absc;
				SpeedPrk = smplspd;
			}
			
            ++iNdx;
			step = iNdx%pros;
			if (iNdx < iCount)
			{
				PerT = tel->GetItem(iNdx);
				absc    = absCoord->GetItem(iNdx);
				smplspd = aprinfo->GetTruckSpeed(absc);		
			}

			Per_tel = fabs(PerT);
			Vpz2 = smplspd;
			Vpz2 > 60 ? Porog=12 : Porog=16;
        }
        if (Max!=0)
        {    
			dlina = absc - dlina;
			if (dlina >= 1)
			{
			       // Заносим результаты превышения
			       per_new.value = Max;
			       per_new.porog = Porog;
			       per_new.speed = SpeedPrk;
			       per_new.absPer = abscoord;
			       this->records.push_back(per_new);
			}
			
			Max=0;
			r=0;
		}
		abstmp = absc;
	}
	return true;
}