#include "VDataSet.h"
#include "VUklon.h"
#include "VParameter.h"

#include "VProgressInterface.h"

// Загрузка из файла
bool VUklonList::Load(VDataSet & ds, VProgressBox * prog)
{
	if (this->IsLoaded()) 
		return true;

	// загрузка параметров
	if (!ds.Load(PRM_ABS_COORD) || !ds.Load(PRM_GRADIENT))
	{
		this->message = ERRMSG_MISSING_PARAMETER;
		return false;
	}

	vector<double> value_uklon;
	VAbsCoordData * absCoord = &ds.GetAbsCoord();
	VAprinfo *aprinfo = &ds.GetAprinfo();
	VParameter *gradient = &ds.GetParameter(PRM_GRADIENT);
	VUklon uklon_new;
	// Количество кадров
	int iCount = 0;

	// Кадр
    long smplspd;
	double absx=0.0;
	double abs1=0.0;
    double uklon1=0.0;

	double abstemp=0.0;

	//long speeds;
    unsigned int VPZ1=0;
    int Porog=5;
	int kon=0;
	int nach=0;
	int rr=0;
	int r=0;

	    
	double absmid_ch=0.0;
	double absnach=0.0;
	double abskon=0.0;
	double absmid=0.0;
	double absmax=0.0;
    double maxuklon=-999;
    double minuklon=999;
	double abs_maxuklon=0.0;
	double abs_minuklon=0.0;
    double Maxofmax=0;
	double absnd=0;
	double abskd=0;
	double dlina=0;
   	double absplus10=0;

	vector<ParUklon> ParGrad;
	ParUklon parukl;


   // Запрашиваем количество кадров
	iCount = (int)absCoord->GetCount();

    // Запрашиваем норму (настройки)
	int pros = iCount/28;
	int step=0;

	if (prog)
	{
		prog->SetRange(0, iCount*3);
		prog->SetMessage(0, L"Загрузка максимальных изменений уклона");
	}

	// Переливаем в массив
	for (int iNdx=0; iNdx<iCount; ++iNdx)
	{
		if (prog && (iNdx % 100) == 0)
			prog->SetPosition(0, iNdx);

		parukl.fuklon = gradient->GetItem(iNdx);
		parukl.dabs   = absCoord->GetItem(iNdx);
		ParGrad.push_back(parukl);
	}

	for (int iNdx = 0; iNdx < iCount; ++iNdx)
	{
		char msg[4096] = {0};
		if (prog && (iNdx % 100) == 0)
			prog->SetPosition(0, iNdx+iCount);

		step = iNdx%pros;
		if (iNdx==13997)
			int absc=0;

		// Запрашиваем текущий кадр
		parukl = ParGrad[iNdx];
		uklon1 = parukl.fuklon;
		absx    = parukl.dabs;
	    abs1    = absx;

		nach=iNdx;
	    absplus10 = abs1+10.0;
		if (uklon1>maxuklon) 
		{
		  maxuklon=uklon1;
		  abs_maxuklon = absx;
		}
		if (uklon1<minuklon) 
		{								
			minuklon=uklon1;
			abs_minuklon = absx;
		}

		while ((absx < absplus10)&&(iNdx<(iCount-1)))
		{	
			iNdx++;
			parukl = ParGrad[iNdx];
			uklon1 = parukl.fuklon;
			absx    = parukl.dabs;
	    }


		if (iNdx>0)
		iNdx--;
		parukl = ParGrad[iNdx];
		uklon1 = parukl.fuklon;
		absx    = parukl.dabs;

	    if (uklon1>maxuklon) 
	    {
		    maxuklon=uklon1;
		    abs_maxuklon = absx;
	    }
	    if (uklon1<minuklon) 
	    {	 							
		    minuklon=uklon1;
		    abs_minuklon = absx;
	    }

		iNdx=nach;
		  
        // разница уклонов больше порога
        if (((maxuklon-minuklon)>Porog)&&(maxuklon!=0)&&(minuklon!=0))
        { 
			  if (rr==0)
			  {
				 // Запрашиваем текущий кадр
				 parukl = ParGrad[nach];
		         uklon1 = parukl.fuklon;
		         absnd    = parukl.dabs;
		         rr=1;
			  }

              if ((maxuklon-minuklon) > Maxofmax) 
	   		  {
			   	 Maxofmax = maxuklon-minuklon;  
				 absmax = absmid;
			  }

	    }
        else if (Maxofmax!=0)                                  // закончилось превышение порога 
        {
			// Запрашиваем текущий кадр
			int konkadr = nach-1;
			parukl = ParGrad[konkadr];
		    uklon1 = parukl.fuklon;
		    abskd    = parukl.dabs;
		    			 

            dlina = abskd - absnd;                            // длина в метрах, на которой длилось превышение порога
            if (dlina>=2)  
			{
			   // Добавляем результаты превышения
			       uklon_new.absCoord = abskd; 
			   	   uklon_new.length = dlina;
			       uklon_new.value = Maxofmax;
			       uklon_new.porog = Porog;
			       uklon_new.name_len = 10;
				   this->records.push_back(uklon_new);			  
			}
            Maxofmax=0;
            rr=0;
        }
        maxuklon=-999;
        minuklon=999;
		abstemp = absx;
	}
	
		absx=0.0;
		abs1=0.0;
		uklon1=0.0;
		abstemp=0.0;
	    Porog=5;
		kon=0;
		nach=0;
		rr=0;
		r=0;	
	    
		absmid_ch=0.0;
		absnach=0.0;
		abskon=0.0;
		absmid=0.0;
		absmax=0.0;
		maxuklon=-999;
		minuklon=999;
		abs_maxuklon=0.0;
		abs_minuklon=0.0;
		Maxofmax=0;
		absnd=0;
		abskd=0;
		dlina=0;
   		absplus10=0;

	for (int iNdx = 0; iNdx < iCount; ++iNdx)
	{
		if (prog && (iNdx % 100) == 0)
			prog->SetPosition(0, iNdx+2*iCount);

		step = iNdx%pros;

		// Запрашиваем текущий кадр
		parukl = ParGrad[iNdx];
		uklon1 = parukl.fuklon;
		absx    = parukl.dabs;
	    abs1    = absx;

		nach=iNdx;
	    absplus10 = abs1+25.0;

        while ((absx < absplus10)&&(iNdx<(iCount-1)))
		{	

			if (uklon1>maxuklon) 
		    {
		  	    maxuklon=uklon1;
			    abs_maxuklon = absx;
		    }
		    if (uklon1<minuklon) 
		    {								
			    minuklon=uklon1;
			    abs_minuklon = absx;
		    }
		
			iNdx++;
			parukl = ParGrad[iNdx];
			uklon1 = parukl.fuklon;
			absx    = parukl.dabs;
	    }

		if (iNdx>0)
			iNdx--;
		parukl = ParGrad[iNdx];
		uklon1 = parukl.fuklon;
		absx = parukl.dabs;

		iNdx=nach;

		// Определяем Vпз в точке середины отрезка (max - min)
        absmid = (abs_maxuklon+abs_minuklon)/2;
	    if (absmid_ch != absmid)
		{
			smplspd = aprinfo->GetMaxPassSpeed(abs_maxuklon,abs_minuklon);
		}
		absmid_ch = absmid;	

		if (smplspd>120) Porog=8;
		else             Porog=10;

        // разница уклонов больше порога и скорость не менялась  
        if (((maxuklon-minuklon)>Porog)&&(VPZ1==smplspd)&&(maxuklon!=0)&&(minuklon!=0))
        { 
			  if (rr==0)
			  {
				 // Запрашиваем текущий кадр
				 parukl = ParGrad[nach];
		         uklon1 = parukl.fuklon;
		         absnd    = parukl.dabs;
		         rr=1;
			  }

              if ((maxuklon-minuklon) > Maxofmax) 
	   		  {
			   	 Maxofmax = maxuklon-minuklon;  
				 absmax = absmid;
			  }

	    }
        else if (Maxofmax!=0)                                  // закончилось превышение порога 
        {
			// Запрашиваем текущий кадр
			int konkadr = nach-1;
			parukl = ParGrad[konkadr];
		    uklon1 = parukl.fuklon;
		    abskd    = parukl.dabs;
		    			 

            dlina = abskd - absnd;                            // длина в метрах, на которой длилось превышение порога
            if (dlina>=2)  
			{
			       // Заносим результаты превышения
			       uklon_new.absCoord = abskd; 
			   	   uklon_new.length = dlina;
			       uklon_new.value = Maxofmax;
			       uklon_new.porog = Porog;
			       uklon_new.name_len = 25;
				   this->records.push_back(uklon_new);
			}
            Maxofmax=0;
            rr=0;
        }
        maxuklon=-999;
        minuklon=999;
		VPZ1 = smplspd;

		abstemp = absx;
	}
	if(this->records.size() > 0)
		return true;
	else
		return false;
}