#include "VDataSet.h"
#include "VPerLine.h"
#include "VParameter.h"

// �������� �� �����
bool VPerLineList::Load(VDataSet & ds)
{
	if (this->IsLoaded()) 
		return true;

	// �������� ����������
	if (!ds.Load(PRM_ABS_COORD) || !ds.Load(PRM_MID_LEVEL) || !ds.Load(PRM_KURS))
	{
		this->message = ERRMSG_MISSING_PARAMETER;
		return false;
	}
	VAbsCoordData * absCoord = &ds.GetAbsCoord();
	VAprinfo *aprinfo = &ds.GetAprinfo();
	VParameter *midlevel_prm = &ds.GetParameter(PRM_MID_LEVEL);
	VParameter *kurs_prm = &ds.GetParameter(PRM_KURS);
	VCurveList *curves = &ds.GetCurves();
	VPerLine new_perline;


    // ���������� ������
	size_t iCount = 0;

	// ����
    long smplspd;
	double absc = 0.0;
	float PerLine = 0.0;
	float Kurs    = 0.0;

	// ����� (�� ���������)
	int Norma = 20;
	
	short Vpz1=0;
	short Vmax=-999;
	short Vmin=999;
	short Sign=0;
    
	double MidLevel=0;
    double MaxLevel=0;

	double kurs1 = 0;
	double kursmax = 0;
	double delta = 0;      // ������� �������� ����� � ����� ��������� ���������� � � �����
	                       // ������ ���������� (���� delta>0.1�������, ��, ������ �����, ����� ����� �������������� ������)

	double abs_nach=0;
	double abs_prev = 0;
	int sch=0;

	size_t Nach=0;
	size_t Kon =0;

	int colMetPr = 0;

	double coordmet1ur=0;
	double coordmetNur=0;

	int count_Hole=0;

	double abstmp = 0.0;

	int Norma1=11;
	int Norma2=11;
	int Norma3=15;

	double maxabscoord=0;

	// ����������� ���������� ������
	iCount = absCoord->GetCount();

	// ���-�� ������
	int iCurve = curves->GetCount();
	
	for (int colCrv=0;colCrv<=iCurve;colCrv++)
	{	
		 // ���������� ����� �� ���������� ������
		 if (colCrv!=0)
			 colMetPr = curves->GetItem(colCrv-1).GetMarkCount();
		 else
		 colMetPr=0;
		
		 if (colCrv!=iCurve)
		 {
		    // �����. 1-� ����� �� ������
			 coordmet1ur = absCoord->GetItem(curves->GetItem(colCrv).GetLevelMark(0).index);
		    // ������ ������ ������
			 Kon = absCoord->FindIndex(coordmet1ur,true);

			if (Kon>=iCount)
			 continue;
		 }
		 else
			 Kon = iCount;
		 
		 if (colCrv!=0)
		 {
		    // �����. ��������� ����� �� ������ ���������� ������  
			 coordmetNur = absCoord->GetItem(curves->GetItem(colCrv-1).GetLevelMark(colMetPr-1).index);
	 	    // ������ ����� ���������� ������
			Nach = absCoord->FindIndex(coordmetNur,true);
			if (Nach>=iCount)
			 continue;
		 }
		 else
			 Nach = 0;

   
		 // �������� ����� ���������
		 MidLevel=0;
         MaxLevel=0;
         abs_nach=0;
		 abstmp=0;
         sch=0;
	     Vmax=-999;
	     Vmin=999;
	
 	     for (size_t iNdx = Nach; iNdx < Kon; ++iNdx)
	     {  		             
		     // ����������� ������� ����
		     absc    = absCoord->GetItem(iNdx);
		     smplspd = aprinfo->GetPassSpeed(absc);
			 PerLine = midlevel_prm->GetItem(iNdx);
			 Kurs    = kurs_prm->GetItem(iNdx);

 		     Vpz1 = smplspd;
		     if (Vpz1>120)
			     Norma=Norma1;
		     else if ((Vpz1>60)&&(Vpz1<=120))
			     Norma=Norma2;
		     else
			     Norma=Norma3;

		
		     if ((fabs(PerLine)>Norma)&&(iNdx!=Kon-1))
             {			
                 if (PerLine<0) Sign = -1;
                 else                  Sign = 1;
     
			     kurs1 = Kurs;
     
                 if (abs_nach!=0)
                 {
			        if (fabs(PerLine) > MaxLevel)
			        {
			           MaxLevel = fabs(PerLine);
				       maxabscoord = absc;
				       kursmax = Kurs;
			        }
                    MidLevel = MidLevel + fabs(PerLine);
                 }
                 else
                 {
			        abs_nach = absc;
                    MaxLevel = fabs(PerLine);
			        maxabscoord = absc;
					kursmax = Kurs;
                    MidLevel = MidLevel + fabs(PerLine);
                 }
                 sch++;

			     if (Vpz1>Vmax) Vmax = Vpz1;
			     if (Vpz1<Vmin) Vmin = Vpz1;
		
             }
             else
             {
			     if (abs_nach!=0)
			     {
                      // ����������� ����������� ����
					 absc    = absCoord->GetItem(iNdx-1);
		         }
     
			     abs_prev = absc - abs_nach;
                 if (sch!=0)
                 MidLevel = MidLevel/sch;

				// � ����� ������� ����
          		 absc    = absCoord->GetItem(iNdx);	

                 if ((abs_nach!=0)&&(abs_prev>=40)) //&&(delta<=0.1))
                 {      
				     // ��������� ���������� ����������
				        new_perline.abscoord = absc;
				        new_perline.dLength = abs_prev;
				        new_perline.dMaxValue = (float)(Sign*MaxLevel);
					    new_perline.dMidValue = (float)(Sign*fabs(MidLevel));
                        new_perline.dNorma = Norma;
				        new_perline.speed1 = Vmin;
				        new_perline.speed2 = Vmax;
						this->records.push_back(new_perline);
			     }
                 MidLevel=0;
                 MaxLevel=0;
                 abs_nach=0;
                 sch=0;
			     Vmax=-999;
	             Vmin=999;
		     }

			 abstmp = absc;
		           
	     }
	}

	absc = 0.0;
	PerLine = 0.0;
	Kurs    = 0.0;

	// ����� (�� ���������)
	Norma = 20;
	
	Vpz1=0;
	Vmax=-999;
	Vmin=999;
	Sign=0;
    
	MidLevel=0;
    MaxLevel=0;

	kurs1 = 0;
	kursmax = 0;
	delta = 0;      // ������� �������� ����� � ����� ��������� ���������� � � �����
	                       // ������ ���������� (���� delta>0.1�������, ��, ������ �����, ����� ����� �������������� ������)
	abs_nach=0;
	abs_prev = 0;
	sch=0;

	Nach=0;
	Kon =0;

	colMetPr = 0;

	coordmet1ur=0;
	coordmetNur=0;

	count_Hole=0;

	abstmp = 0.0;

	Norma1=16;
	Norma2=20;
	Norma3=25;

	maxabscoord=0;

	for (int colCrv=0;colCrv<=iCurve;colCrv++)
	{	
		 // ���������� ����� �� ���������� ������
		 if (colCrv!=0)
			 colMetPr = curves->GetItem(colCrv-1).GetMarkCount();
		 else
		 colMetPr=0;
		
		 if (colCrv!=iCurve)
		 {
		    // �����. 1-� ����� �� ������
			 coordmet1ur = absCoord->GetItem(curves->GetItem(colCrv).GetLevelMark(0).index);
		    // ������ ������ ������
			 Kon = absCoord->FindIndex(coordmet1ur,true);

			if (Kon>=iCount)
			 continue;
		 }
		 else
			 Kon = iCount;
		 
		 if (colCrv!=0)
		 {
		    // �����. ��������� ����� �� ������ ���������� ������  
			 coordmetNur = absCoord->GetItem(curves->GetItem(colCrv-1).GetLevelMark(colMetPr-1).index);
	 	    // ������ ����� ���������� ������
			Nach = absCoord->FindIndex(coordmetNur,true);
			if (Nach>=iCount)
			 continue;
		 }
		 else
			 Nach = 0;

   
		 // �������� ����� ���������
		 MidLevel=0;
         MaxLevel=0;
         abs_nach=0;
		 abstmp=0;
         sch=0;
	     Vmax=-999;
	     Vmin=999;
	
 	     for (size_t iNdx = Nach; iNdx < Kon; ++iNdx)
	     {  		             
		     // ����������� ������� ����
		     absc    = absCoord->GetItem(iNdx);
		     smplspd = aprinfo->GetPassSpeed(absc);
			 PerLine = midlevel_prm->GetItem(iNdx);
			 Kurs    = kurs_prm->GetItem(iNdx);

 		     Vpz1 = smplspd;
		     if (Vpz1>120)
			     Norma=Norma1;
		     else if ((Vpz1>60)&&(Vpz1<=120))
			     Norma=Norma2;
		     else
			     Norma=Norma3;

		
		     if ((fabs(PerLine)>Norma)&&(iNdx!=Kon-1))
             {			
                 if (PerLine<0) Sign = -1;
                 else                  Sign = 1;
     
			     kurs1 = Kurs;
     
                 if (abs_nach!=0)
                 {
			        if (fabs(PerLine) > MaxLevel)
			        {
			           MaxLevel = fabs(PerLine);
				       maxabscoord = absc;
				       kursmax = Kurs;
			        }
                    MidLevel = MidLevel + fabs(PerLine);
                 }
                 else
                 {
			        abs_nach = absc;
                    MaxLevel = fabs(PerLine);
			        maxabscoord = absc;
					kursmax = Kurs;
                    MidLevel = MidLevel + fabs(PerLine);
                 }
                 sch++;

			     if (Vpz1>Vmax) Vmax = Vpz1;
			     if (Vpz1<Vmin) Vmin = Vpz1;
		
             }
             else
             {
			     if (abs_nach!=0)
			     {
                      // ����������� ����������� ����
					 absc    = absCoord->GetItem(iNdx-1);
		         }
     
			     abs_prev = absc - abs_nach;
                 if (sch!=0)
                 MidLevel = MidLevel/sch;

				// � ����� ������� ����
          		 absc    = absCoord->GetItem(iNdx);	

                 if ((abs_nach!=0)&&(abs_prev>=40)) //&&(delta<=0.1))
                 {      
				     // ��������� ���������� ����������
				        new_perline.abscoord = absc;
				        new_perline.dLength = abs_prev;
				        new_perline.dMaxValue = (float)(Sign*MaxLevel);
					    new_perline.dMidValue = (float)(Sign*fabs(MidLevel));
                        new_perline.dNorma = Norma;
				        new_perline.speed1 = Vmin;
				        new_perline.speed2 = Vmax;
						this->records.push_back(new_perline);
			     }
                 MidLevel=0;
                 MaxLevel=0;
                 abs_nach=0;
                 sch=0;
			     Vmax=-999;
	             Vmin=999;
		     }

			 abstmp = absc;
		           
	     }
	}

	return true;
  }
