#include "VParamCorrections.h"
#include <wx/wx.h>

#ifdef _OPENMP
	#include "omp.h"
#endif

/*
	Disabl-ить кнопки "Коррекция" можно в методе VVerifyParameter::Verify после проведения расчёта
*/
//----------------------------------Контейнер для всех классов оценивающих верификацию файла-------------------------------
void VVerifyFileList::LoadList(VDataSetPtr _dataSet, VIBDatabasePtr db, VProgressBox & prog)
{
	prog.SetMessage(0, L"Загрузка данных");
	prog.Update();

	dataSet = _dataSet;
	_dataSet->Load(PRM_SPEED);
	if(_dataSet->GetParameter(PRM_SPEED).GetItem(0) > 0)
		orientation = "к.в.";
	else
		orientation = "к.н.";
	if(_dataSet->GetMoveDirection()>0)
		direction = "вперёд";
	else
		direction = "назад";
	date_trail = _dataSet->GetRailDate().DateToStringISO();

	_dataSet->Load(PRM_WIDTH);
	_dataSet->Load(PRM_WIDTH_FRONT);
	_dataSet->Load(PRM_WIDTH_BACK);
	_dataSet->Load(PRM_PPR_WIDTH);
	_dataSet->Load(PRM_CURV);
	_dataSet->Load(PRM_KURS);
	_dataSet->Load(PRM_RICH_LEFT);
	_dataSet->Load(PRM_RICH_RIGHT);
	_dataSet->Load(PRM_HISTORY);

	prog.SetMessage(0, L"Оценка сфазированности ШК спереди относительно ШК");
	prog.Update();
	this->records.push_back(VVerifyParameterPtr(new VVerifyParameter(PRM_WIDTH_FRONT, PRM_WIDTH, _dataSet, 0)));

	prog.SetMessage(0, L"Оценка сфазированности ШК сзади относительно ШК");
	prog.Update();
	this->records.push_back(VVerifyParameterPtr(new VVerifyParameter(PRM_WIDTH_BACK, PRM_WIDTH, _dataSet, 0)));

	prog.SetMessage(0, L"Оценка сфазированности рихтовки левой относительно кривизны");
	prog.Update();
	this->records.push_back(VVerifyParameterPtr(new VVerifyParameter(PRM_RICH_LEFT, PRM_CURV, _dataSet, 0)));

	prog.SetMessage(0, L"Оценка сфазированности рихтовки правой относительно кривизны");
	prog.Update();
	this->records.push_back(VVerifyParameterPtr(new VVerifyParameter(PRM_RICH_RIGHT, PRM_CURV, _dataSet, 0)));

	prog.SetMessage(0, L"Оценка сфазированности ШК ППР относительно ШК");
	prog.Update();
	this->records.push_back(VVerifyPPRPtr(new VVerifyPPR(PRM_PPR_WIDTH, PRM_WIDTH, _dataSet, 0)));

	prog.SetMessage(0, L"Оценка сфазированности курса относительно кривизны");
	prog.Update();
	this->records.push_back(VVerifyKursPtr(new VVerifyKurs(_dataSet, 0)));

	prog.SetMessage(0, L"Оценка ШК и уровня по контрольным участкам");
	prog.Update();
	this->records.push_back(VVerifyWidLevPtr(new VVerifyWidLev(_dataSet, db)));

	prog.SetMessage(0, L"Оценка качества отбоя КМС");
	prog.Update();
	this->records.push_back(VVerifyPolePtr(new VVerifyPole(_dataSet, db)));

#define __FOR_TESTING_PURPOSE_ONLY__

#ifdef __FOR_TESTING_PURPOSE_ONLY__

	prog.SetMessage(0, L"Оценка сфазированности кривизны относительно уровеня");
	prog.Update();
	this->records.push_back(VVerifyParameterPtr(new VVerifyParameter(PRM_CURV, PRM_LEVEL, _dataSet, 0)));

	prog.SetMessage(0, L"Оценка сфазированности лев.износа ППР спереди относительно лев.износа");
	prog.Update();
	this->records.push_back(VVerifyPPRPtr(new VVerifyPPR(PRM_PPR_IZN_LEFT, PRM_WEAR_LEFT, _dataSet, 0)));

	prog.SetMessage(0, L"Оценка сфазированности прав.износа ППР спереди относительно прав.износа");
	prog.Update();
	this->records.push_back(VVerifyPPRPtr(new VVerifyPPR(PRM_PPR_IZN_RIGHT, PRM_WEAR_RIGHT, _dataSet, 0)));

	prog.SetMessage(0, L"Оценка положения фактических кривых относительно паспорта");
	prog.Update();
	this->records.push_back(VVerifyCurvesPtr(new VVerifyCurves(_dataSet, db)));

	prog.SetMessage(0, L"Оценка корректности координаты по меткам RFID");
	prog.Update();
	this->records.push_back(VVerifyAbscoordPtr(new VVerifyAbscoord(_dataSet, db)));

#endif

	prog.SetMessage(0, L"Завершение оценки");
	prog.Update();
}

void VVerifyFileList::Verify(VProgressBox & prog, bool calc_offset)
{
	for(size_t i = 0; i < this->GetCount(); i++)
		if(this->GetItem(i)->allowVerify)
		{
			if(prog.QueryCancel())
				continue;
			this->GetItem(i)->Verify(prog, calc_offset);
		}
}

void VVerifyFileList::Execute(VProgressBox & prog)
{
	for(size_t i = 0; i < this->GetCount(); i++)
		if(this->GetItem(i)->allowExecute)
			this->GetItem(i)->Execute(prog);
}

void VVerifyFileList::GetCSVResults(const std::string in_path, FILE_TYPE in_type)
{
	VExportImportTextFile expTextFile(in_path, in_type, "Оценка качества");
	std::vector<string> in_legend;
	in_legend.push_back("Название теста");
	in_legend.push_back("Результат");
	in_legend.push_back("Комментарий");
	expTextFile.SetLegend(in_legend);
	for(size_t i = 0; i < GetCount(); i++)
	{
		std::vector<string> new_row;
		new_row.push_back(GetItem(i)->name_test);
		if(GetItem(i)->GetVerifyMark().verifyLevel == VERIFYING_NOT_LOAD)
		{
			new_row.push_back("Данные не загружены");
			new_row.push_back("");
		}
		else if (GetItem(i)->GetVerifyMark().verifyLevel == VERIFYING_NOT_CALC)
		{
			new_row.push_back("Расчёт сдвигов не проводился");
			new_row.push_back("");
		}
		else
		{
			if(GetItem(i)->GetVerifyMark().verifyLevel == VERIFYING_ALL_RIGHT)
				new_row.push_back("Тест успешно пройден");
			if(GetItem(i)->GetVerifyMark().verifyLevel == VERIFYING_WARNING)
				new_row.push_back("Предупреждение");
			if(GetItem(i)->GetVerifyMark().verifyLevel == VERIFYING_ERROR)
				new_row.push_back("Ошибка");
			new_row.push_back(GetItem(i)->GetString());
			expTextFile.InsertRow(new_row);
		}
	}
	expTextFile.Export();
	/*
	output<<"Ориентация:"<<divider<<this->orientation<<std::endl;
	output<<"Направление:"<<divider<<this->direction<<std::endl;
	output<<"Дата поездки:"<<divider<<this->date_trail;
	*/
}

void VVerifyFileList::Save()
{
	dataSet->Save();
	dataSet->CloseFile();
}

void VVerifyFileList::Update()
{
	for(size_t i = 0; i < GetCount(); i++)
		if(GetItem(i)->IsChanged())
			GetItem(i)->allowVerify = true;
}

//---------------------------------------------- Класс фазировки параметров -----------------------------------------------
VVerifyParameter::VVerifyParameter(PARAMETER_TYPE _parameterPhased, PARAMETER_TYPE _parameterEthalon, VDataSetPtr _dataSet, short int _method_index)
{
	dataSet = _dataSet;
	parameterPhased = _parameterPhased;
	parameterEthalon = _parameterEthalon;
	method_index = _method_index;
	needAnotherFile = false;
	allowExecute = false;
	allowVerify = true;
	isExecute = false;
	valueExecute = 0;
	coefVerifyOld = 0;
	coefVerifyNew = 0;
	abscoordExecute = 0;
	this->name_test = "Фазировка параметра '" + wstring_to_string(GetUserParamName(parameterPhased)) + "' относительно '" + wstring_to_string(GetUserParamName(parameterEthalon)) + "'";
	this->verifyMark.message.push_back("Коэффициент корреляции не посчитан");
	this->verifyMark.message.push_back("Рекомендуемый сдвиг: не посчитан");
	this->verifyMark.message.push_back("Потенциальный коэффициент корреляции: не посчитан");
	this->methodsDescription.push_back("Фазировка параметра '" + wstring_to_string(GetUserParamName(parameterPhased)) + "' относительно '" + wstring_to_string(GetUserParamName(parameterEthalon)) + "'");
	this->methodsDescription.push_back("Фазировка параметра '" + wstring_to_string(GetUserParamName(parameterEthalon)) + "' относительно '" + wstring_to_string(GetUserParamName(parameterPhased)) + "'");
	Load();
}

bool VVerifyParameter::Load()
{
	if (!dataSet->Load(parameterEthalon) || 
		!dataSet->Load(parameterPhased) || 
		!dataSet->Load(PRM_ABS_COORD) || 
		!dataSet->Load(PRM_CORRUPT) /*|| !dataSet->Load(PRM_HISTORY, true)*/ )
	{
		verifyMark.verifyLevel=VERIFYING_NOT_LOAD;
		allowVerify = false;
		return false;
	}
	else
		return true;
}

void VVerifyParameter::Verify(VProgressBox & prog, bool calc_offset)
{
	//Инициализируем первоначальное состояние
	//this->verifyMark.message.clear();
	//this->verifyMark.message.push_back("Коэффициент корреляции не посчитан";
	//this->verifyMark.message.push_back("Рекомендуемый сдвиг: не посчитан";

	char _message [256];
	int maxOffset = 250;
	if(this->verifyMark.verifyLevel != VERIFYING_NOT_LOAD && allowVerify)
	{
		if(calc_offset)
		{
			if(dataSet->GetParameter(parameterPhased).GetCount() > 2 * maxOffset)
			{
				prog.SetMessage(0, string_to_wstring(this->name_test).c_str());
				
				this->coefVerifyNew = -1.0;				

#pragma omp parallel for
				for (int offset = 0; offset <= 2*maxOffset; ++offset)
				{
					double corr = CorrParams(dataSet->GetParameter(parameterPhased), dataSet->GetParameter(parameterEthalon), 
								parameterPhased, parameterEthalon, offset, maxOffset);

					if (corr > this->coefVerifyNew)
					{
						this->coefVerifyNew = corr;
						this->valueExecute = (int) offset - (int) maxOffset;
					}

					if((maxOffset - offset) == 0)
						this->coefVerifyOld = corr;
				}

				const double step = dataSet->GetAbsCoord().GetLengthStep();
				this->abscoordExecute = this->valueExecute*step;
				if(abs(this->valueExecute) <= 1)
				{
					this->verifyMark.message[1] = "Фазировка не требуется";
					this->verifyMark.message[2] = "Потенциальный коэффициент корреляции макс.";
					this->verifyMark.verifyLevel = VERIFYING_ALL_RIGHT;
					allowVerify = false;
				}
				else if (abs(this->valueExecute) == maxOffset)
				{
					sprintf(_message, "Ошибка в измеренных данных");
					this->verifyMark.message[1] = (const char *) _message;
					this->verifyMark.verifyLevel = VERIFYING_ERROR;
					allowVerify = false;
				}
				else
				{
					sprintf(_message, "Рекомендуемый сдвиг: %i (%.0f м) кадров", 
						this->valueExecute, this->abscoordExecute);
					this->verifyMark.message[1] = (const char *) _message;
					sprintf(_message, "Потенциальный коэффициент корреляции = %.4f", this->coefVerifyNew);
					this->verifyMark.message[2] = (const char *) _message;
					this->verifyMark.verifyLevel = VERIFYING_WARNING;
					if(parameterPhased == PRM_RICH_LEFT || parameterPhased == PRM_RICH_RIGHT ||
						parameterPhased == PRM_CURV)
						allowExecute = false;
					else
						allowExecute = true;
					allowVerify = false;
				}
				prog.SetMessage(0, "Завершение расчета");
			}
			else
			{
				sprintf(_message, "Слишком короткий файл");
				this->verifyMark.message[1] = (const char *) _message;
				this->verifyMark.verifyLevel = VERIFYING_ERROR;
				allowVerify = false;
			}
		}
		else
		{
			double _coefVerifyOld = this->coefVerifyOld;
			this->coefVerifyOld = CorrParams(dataSet->GetParameter(parameterPhased), dataSet->GetParameter(parameterEthalon), 
								parameterPhased, parameterEthalon, maxOffset, maxOffset);

			if(this->coefVerifyOld == 1.0)
			{
				this->verifyMark.message[0] = "Ошибка в измеренных данных";
				//sprintf(_message, "Ошибка в измеренных данных";
				this->verifyMark.verifyLevel = VERIFYING_ERROR;
				allowVerify = false;
			}
			else
			{
				if(!_coefVerifyOld) //Первоначальная оценка
				{
					sprintf(_message, "Первоначальный коэффициент корреляции = %.4f", this->coefVerifyOld);
					this->verifyMark.message[0] = (const char *) _message;
				}
				else if (_coefVerifyOld != this->coefVerifyOld) //произошло изменение коэффициента корреляции после сдвига какого-то параметра
				{
					sprintf(_message, "Новый коэффициент корреляции = %.4f", this->coefVerifyOld);
					if(this->verifyMark.message.size() == 4)
						this->verifyMark.message[3] = (const char *) _message;
					else
						this->verifyMark.message.push_back(_message);
				}
				this->verifyMark.verifyLevel = VERIFYING_NOT_CALC;
			}
			//this->verifyMark.message[0] = (const char *) _message; 
			//УКАЗыВАЕМ НА ТО, ЧТО РАСЧЁТ СДВИГА НЕ ПРОВЕДЁН
			//здесь нужно указать пороги для коэффициентов верификации с предупреждениями
		}
	}
	else
	{
		allowVerify = false;
	}
}

void VVerifyParameter::Execute(VProgressBox & prog)
{
	if(allowExecute)
	{
		double _abscoordExecute;
		int _valueExecute;
		PARAMETER_TYPE _parameterPhased;
		if(method_index)
		{
			_abscoordExecute = -this->abscoordExecute;
			_valueExecute = -this->valueExecute;
			_parameterPhased = this->parameterEthalon;
		}
		else
		{
			_abscoordExecute = this->abscoordExecute;
			_valueExecute = this->valueExecute;
			_parameterPhased = this->parameterPhased;
		}
		prog.SetMessage(0, string_to_wstring(this->methodsDescription[method_index]).c_str());
		dataSet->GetParameter(_parameterPhased).ShiftByKadr(_valueExecute);
		dataSet->SetChanged(_parameterPhased);
		dataSet->GetParameter(_parameterPhased).ShiftCorrupt(_valueExecute);
		dataSet->SetChanged(PRM_CORRUPT);
		this->verifyMark.message[1] = "Проведена фазировка";
		this->verifyMark.message[2] = "Потенциальный коэффициент корреляции макс.";
		this->verifyMark.verifyLevel = VERIFYING_ALL_RIGHT;
		//Записываем информацию в историю операций
		wchar_t histMessage[256] = {0};
		swprintf(histMessage, 255, L"%s: %i кадров (%.1f метров)", string_to_wstring(this->methodsDescription[method_index]).c_str(), _valueExecute, _abscoordExecute);
		VOperationRecord record;
		record.SetTextDescription(wstring_to_string(histMessage).c_str(),256);
		dataSet->GetHistory().Add(record);
		dataSet->SetChanged(PRM_HISTORY);
		//Расставляем флаги
		allowVerify = true;
		allowExecute = false;
		isExecute = true;
	}
}

void VVerifyParameter::GetCSVString(std::ofstream & output, const std::string divider)
{
	output<<"стар. корр.:"<<this->coefVerifyOld<<"/"<<"нов. корр.:"<<this->coefVerifyNew;
	output<<"("<<this->GetVerifyMark().message[1]<<"";
}

std::string VVerifyParameter::GetString()
{
	char msg [256];
	sprintf(msg, "стар. корр.:%.2f/нов. корр.:%.2f(%s)", this->coefVerifyOld, this->coefVerifyNew, this->GetVerifyMark().message[1].c_str());
	return (const char *) msg;
}

PARAMETER_TYPE VVerifyParameter::GetEthalonParameter()
{
	return parameterEthalon;
}

PARAMETER_TYPE VVerifyParameter::GetPhasedParameter()
{
	return parameterPhased;
}

bool VVerifyParameter::IsChanged()
{
	return (dataSet->IsChanged(parameterPhased) || dataSet->IsChanged(parameterEthalon));
}

//-----------------------------------------------Класс фазировки 'Курса'-----------------------------------------------------
VVerifyKurs::VVerifyKurs(VDataSetPtr _dataSet, short int _method_index) : VVerifyParameter(PRM_CURV_FROM_KURS, PRM_CURV, _dataSet, _method_index)
{
	this->name_test = "Фазировка параметра 'Курс' относительно 'Кривизна'";
	this->methodsDescription[0] = "Фазировка параметра 'Курс' относительно 'Кривизна'";
	this->methodsDescription[1] = "Фазировка параметра 'Кривизна' относительно 'Курс'";
}

void VVerifyKurs::Execute(VProgressBox & prog)
{
	if(allowExecute)
	{
		double _abscoordExecute;
		int _valueExecute;
		if(method_index)
		{
			_abscoordExecute = -this->abscoordExecute;
			_valueExecute = -this->valueExecute;
			dataSet->GetParameter(PRM_CURV).ShiftByKadr(_valueExecute);
			dataSet->SetChanged(PRM_CURV);			
		}
		else
		{
			if (dataSet->Load(PRM_KURS_BACKUP))
				dataSet->Unload(PRM_KURS_BACKUP);
			else
			{
				dataSet->GetParameter(PRM_KURS_BACKUP).Create(DHT_KURS_BACKUP, dataSet->GetParameter(PRM_KURS).GetCount(), 1, 0.0f);
				for (size_t i = 0; i < dataSet->GetParameter(PRM_KURS).GetCount(); ++i)
				{
					dataSet->GetParameter(PRM_KURS_BACKUP).GetItem(i) = 
					dataSet->GetParameter(PRM_KURS).GetItem(i);
				}
				dataSet->SetChanged(PRM_KURS_BACKUP);
			}

			// пишем в файл исправленный курс
			_valueExecute = this->valueExecute;
			_abscoordExecute = this->abscoordExecute;
			dataSet->Load(PRM_KURS);
			dataSet->GetParameter(PRM_KURS).ShiftByKadr(_valueExecute);
			dataSet->Load(PRM_CURV_FROM_KURS);
			dataSet->SetChanged(PRM_KURS);
		}
		//Сохранить историю операций
		wchar_t histMessage[256] = {0};
		swprintf(histMessage, 255, L"%s: %i кадров (%.1f метров)", string_to_wstring(this->methodsDescription[method_index]).c_str(), _valueExecute, _abscoordExecute);
		VOperationRecord record;
		record.SetTextDescription(wstring_to_string(histMessage).c_str(),256);
		dataSet->GetHistory().Add(record);
		dataSet->SetChanged(PRM_HISTORY);
		//Расставляем флаги
		this->isExecute = true;
		this->allowVerify = true;
		this->allowExecute = false;
		char _message [100];
		sprintf(_message, "Проведена фазировка");
		this->verifyMark.message[1] = (const char *) _message;
		this->verifyMark.message[2] = "Потенциальный коэффициент корреляции макс.";
		this->verifyMark.verifyLevel = VERIFYING_ALL_RIGHT;
	}
}

bool VVerifyKurs::IsChanged()
{
	return (dataSet->IsChanged(PRM_KURS) || dataSet->IsChanged(PRM_CURV));
}

//-----------------------------------------------Класс фазировки ППР -----------------------------------------------------
VVerifyPPR::VVerifyPPR(PARAMETER_TYPE _parameterPhased, PARAMETER_TYPE _parameterEthalon, VDataSetPtr _dataSet, short int _method_index) : VVerifyParameter(_parameterPhased, _parameterEthalon, _dataSet, _method_index)
{
	this->methodsDescription[0] = "Фазировка 'ППР' относительно '" + wstring_to_string(GetUserParamName(parameterEthalon)) + "'";
	this->methodsDescription[1] = "Фазировка параметра '" + wstring_to_string(GetUserParamName(parameterEthalon)) + "' относительно 'ППР'";
}

void VVerifyPPR::Execute(VProgressBox & prog)
{
	if(allowExecute)
	{
		double _abscoordExecute;
		int _valueExecute;
		if(method_index)
		{
			_abscoordExecute = -this->abscoordExecute;
			_valueExecute = -this->valueExecute;
			dataSet->GetParameter(this->parameterEthalon).ShiftByKadr(_valueExecute);
			dataSet->SetChanged(parameterEthalon);			
		}
		else
		{
			_abscoordExecute = this->abscoordExecute;
			_valueExecute = this->valueExecute;
			prog.SetMessage(0, string_to_wstring(this->methodsDescription[method_index]).c_str());

			bool sh_ppr = false;
			if (dataSet->Load(PRM_PPR_WAYPASSED))
			{
				dataSet->GetParameter(PRM_PPR_WAYPASSED).ShiftByKadr(_valueExecute);
				dataSet->SetChanged(PRM_PPR_WAYPASSED);
				sh_ppr =  true;
			}

			if (dataSet->Load(PRM_PPR_WIDTH))
			{
				dataSet->GetParameter(PRM_PPR_WIDTH).ShiftByKadr(_valueExecute);
				dataSet->SetChanged(PRM_PPR_WIDTH);
				sh_ppr =  true;
			}

			if (dataSet->Load(PRM_PPR_IZN_LEFT))
			{
				dataSet->GetParameter(PRM_PPR_IZN_LEFT).ShiftByKadr(_valueExecute);
				dataSet->SetChanged(PRM_PPR_IZN_LEFT);
				sh_ppr =  true;
			}

			if (dataSet->Load(PRM_PPR_IZN_RIGHT))
			{
				dataSet->GetParameter(PRM_PPR_IZN_RIGHT).ShiftByKadr(_valueExecute);
				dataSet->SetChanged(PRM_PPR_IZN_RIGHT);
				sh_ppr =  true;
			}

			if (dataSet->Load(PRM_PPR_NAK_LEFT))
			{
				dataSet->GetParameter(PRM_PPR_NAK_LEFT).ShiftByKadr(_valueExecute);
				dataSet->SetChanged(PRM_PPR_NAK_LEFT);
				sh_ppr =  true;
			}

			if (dataSet->Load(PRM_PPR_NAK_RIGHT))
			{
				dataSet->GetParameter(PRM_PPR_NAK_RIGHT).ShiftByKadr(_valueExecute);
				dataSet->SetChanged(PRM_PPR_NAK_RIGHT);
				sh_ppr =  true;
			}

			if (dataSet->Load(PRM_PPR_POD_LEFT))
			{
				dataSet->GetParameter(PRM_PPR_POD_LEFT).ShiftByKadr(_valueExecute);
				dataSet->SetChanged(PRM_PPR_POD_LEFT);
				sh_ppr =  true;
			}

			if (dataSet->Load(PRM_PPR_POD_RIGHT))
			{
				dataSet->GetParameter(PRM_PPR_POD_RIGHT).ShiftByKadr(_valueExecute);
				dataSet->SetChanged(PRM_PPR_POD_RIGHT);
				sh_ppr =  true;
			}

			/// Отбраковки ппр двигаем 1 (ОДИН!!!!!) раз. Они ОбЩИЕ на всех
			if (sh_ppr)
			{
				dataSet->GetParameter(PRM_PPR_WIDTH).ShiftCorrupt(_valueExecute);
				dataSet->SetChanged(PRM_CORRUPT);
			}

			this->verifyMark.message[1] = "Проведена фазировка";
			this->verifyMark.message[2] = "Потенциальный коэффициент корреляции макс.";
			this->verifyMark.verifyLevel = VERIFYING_ALL_RIGHT;
		}
		//Сохранить историю операций
		wchar_t histMessage[256] = {0};
		swprintf(histMessage, 255, L"%s: %i кадров (%.1f метров)", string_to_wstring(this->methodsDescription[method_index]).c_str(), _valueExecute, _abscoordExecute);
		VOperationRecord record;
		record.SetTextDescription(wstring_to_string(histMessage).c_str(),256);
		dataSet->GetHistory().Add(record);
		dataSet->SetChanged(PRM_HISTORY);
		//Расставляем флаги
		this->isExecute = true;
		this->allowVerify = true;
		this->allowExecute = false;
		char _message [100];
		sprintf(_message, "Проведена фазировка");
		this->verifyMark.message[1] = (const char *) _message;
		this->verifyMark.message[2] = "Потенциальный коэффициент корреляции макс.";
		this->verifyMark.verifyLevel = VERIFYING_ALL_RIGHT;
	}
}

//-------------------------------------Оценка параметров на контрольных участках ----------------------------------------------
VVerifyWidLev::VVerifyWidLev(VDataSetPtr _dataSet, VIBDatabasePtr _db)
{
	dataSet = _dataSet;
	needAnotherFile = false;
	allowExecute = false;
	allowVerify = true;
	name_test = "Контрольные участки";
	///здесь выставляем границы выхода МО параметров 
	///за допустимые значения на контрольных участках
	this->width_range = 3;
	this->level_range = 3;
	if(this->Load(_db))
		this->verifyMark.verifyLevel = VERIFYING_WARNING;
	else
	{
		this->verifyMark.message.push_back("Данные не загружены");
		this->verifyMark.verifyLevel = VERIFYING_NOT_LOAD;
		this->allowVerify = false;
	}
}

bool VVerifyWidLev::Load(VIBDatabasePtr _db)
{
	if(!dataSet->Load(PRM_ABS_COORD) || !dataSet->Load(PRM_LEVEL) || !dataSet->Load(PRM_WIDTH) 
		|| !dataSet->Makeinfo(_db, false, false))
		return false;
	else
	{
		std::vector<VAprinfoControlAreaRecord> _apr_area_control = dataSet->GetAprinfo().GetControlArea();
		for(size_t i = 0; i < _apr_area_control.size(); i++)
			if(dataSet->GetAbsCoord().GetItem(0) <= _apr_area_control[i].base && dataSet->GetAbsCoord().GetLastItem() >= _apr_area_control[i].end)
				apr_control_area.push_back(_apr_area_control[i]);
		if(apr_control_area.size())
		{
			for(size_t i = 0; i < apr_control_area.size(); i++)
			{
				size_t start_index = dataSet->GetAbsCoord().FindIndex(apr_control_area[i].base, false);
				size_t end_index = dataSet->GetAbsCoord().FindIndex(apr_control_area[i].end, false);
				MsrControlArea new_record;
				new_record.mo_width = dataSet->GetParameter(PRM_WIDTH).GetMO(start_index, end_index);
				new_record.mo_level = dataSet->GetParameter(PRM_LEVEL).GetMO(start_index, end_index);
				msr_control_area.push_back(new_record);
			}
		}
		else
			return false;
		return true;
	}
}

void VVerifyWidLev::Verify(VProgressBox &prog, bool calc_offset)
{
	this->verifyMark.message.clear();
	if(this->GetVerifyMark().verifyLevel != VERIFYING_NOT_LOAD)
	{
		std::vector<std::string> msgs;
		for(size_t i = 0; i < apr_control_area.size(); i++)
		{
			char message [256];
			sprintf(message, "ШК (%.0fм-%.0fм): разница - %.1f", apr_control_area[i].base, apr_control_area[i].end, apr_control_area[i].mo_width - msr_control_area[i].mo_width);
			this->verifyMark.message.push_back(message);
			sprintf(message, "Возв. (%.0fм-%.0fм): разница - %.1f", apr_control_area[i].base, apr_control_area[i].end, apr_control_area[i].mo_level - msr_control_area[i].mo_level);
			this->verifyMark.message.push_back(message);
		}
		if(msgs.size() > 0)
		{
			this->verifyMark.message.push_back("Найдены расхождения на следующих участках:");
			for(size_t i = 0; i < msgs.size(); i++)
				this->verifyMark.message.push_back(msgs[i]);
			this->verifyMark.verifyLevel = VERIFYING_WARNING;
		}
		else
		{
			this->verifyMark.message.push_back("Расхождения не обнаружены");
			this->verifyMark.verifyLevel = VERIFYING_ALL_RIGHT;
		}
		this->allowVerify = false;
	}
	else
	{
		this->verifyMark.message.push_back("Данные не загружены");
	}
}

void VVerifyWidLev::GetCSVString(std::ofstream &output, const std::string divider)
{
	for(size_t i = 0; i < this->GetVerifyMark().message.size(); i++)
		output<<this->GetVerifyMark().message[i]<<"/";
}

std::string VVerifyWidLev::GetString()
{
	std::string return_value = "";
	for(size_t i = 0; i < this->GetVerifyMark().message.size(); i++)
		return_value+=(this->GetVerifyMark().message[i]+"<br>");
	return return_value;
}

//------------------------------------- Процент отбитых километровых стлобов ---------------------------------------------------
VVerifyPole::VVerifyPole(VDataSetPtr _dataSet, VIBDatabasePtr _db)
{
	dataSet = _dataSet;
	needAnotherFile = false;
	allowExecute = false;
	allowVerify = true;
	name_test = "Процент отбитых километровых столбов";
	if(this->Load(_db))
		this->verifyMark.verifyLevel = VERIFYING_WARNING;		
	else
	{
		this->verifyMark.message.push_back("Данные не загружены");	
		this->verifyMark.verifyLevel = VERIFYING_NOT_LOAD;
	}
}

bool VVerifyPole::Load(VIBDatabasePtr _db)
{
	if(!dataSet->Load(PRM_ABS_COORD) || !dataSet->Load(PRM_OP_MARKS)
		|| !dataSet->Makeinfo(_db, false, false))
		return false;
	else
		return true;
}

void VVerifyPole::Verify(VProgressBox &prog, bool calc_offset)
{
	this->verifyMark.message.clear();
	if(this->GetVerifyMark().verifyLevel != VERIFYING_NOT_LOAD)
	{
		size_t count_pole=0;
		for(size_t i = 0; i < dataSet->GetOpMarks().GetCount(); i++)
			if(dataSet->GetOpMarks().GetItem(i).isPole)
				count_pole++;
		std::vector<VAprinfoKmRecord> kmList = dataSet->GetAprinfo().GetKmList(dataSet->GetAbsCoord().GetItem(0), dataSet->GetAbsCoord().GetLastItem(), false);
		procent = (int) 100*count_pole/kmList.size();
		char message [64];
		sprintf(message, "Процент: %i, Кол-во: %i", procent, count_pole);
		this->verifyMark.message.push_back((const char *) message);
	}
	else
		this->verifyMark.message.push_back("Данные не загружены");
	this->allowVerify = false;
}

void VVerifyPole::GetCSVString(std::ofstream &output, const std::string divider)
{
	output<<this->GetVerifyMark().message[0];
}

std::string VVerifyPole::GetString()
{
	return this->GetVerifyMark().message[0];
}

//-------------------- Класс оценивающий отношение кол-во паспортных кривых к кол-ву фактических -------------------------------
VVerifyCurves::VVerifyCurves(VDataSetPtr _dataSet, VIBDatabasePtr _db)
{
	dataSet = _dataSet;
	needAnotherFile = false;
	allowExecute = false;
	allowVerify = true;
	name_test = "Кол-во пасп. кривых/Кол-во факт. кривых";
	if(this->Load(_db))
		this->verifyMark.verifyLevel = VERIFYING_WARNING;
	else
	{
		this->verifyMark.verifyLevel = VERIFYING_NOT_LOAD;
		this->verifyMark.message.push_back("Данные не загружены");
	}
}

bool VVerifyCurves::Load(VIBDatabasePtr _db)
{
	if(!dataSet->Load(PRM_CURVE_MARKS) || !dataSet->Load(PRM_MODEL_CURVES))
		return false;
	else
		return true;
}

void VVerifyCurves::Verify(VProgressBox &prog, bool calc_offset)
{
	this->verifyMark.message.clear();
	if(this->GetVerifyMark().verifyLevel != VERIFYING_NOT_LOAD)
	{
		count_pasport = dataSet->GetCurves(PRM_MODEL_CURVES).GetCount();
		count_fact = dataSet->GetCurves(PRM_CURVE_MARKS).GetCount();
		char message [64];
		sprintf(message, "%i/%i", count_pasport, count_fact);
		this->verifyMark.message.push_back((const char *) message);
		if(count_pasport != count_fact)
			this->verifyMark.verifyLevel = VERIFYING_ERROR;
		else
			this->verifyMark.verifyLevel = VERIFYING_ALL_RIGHT;
	}
	else
		this->verifyMark.message.push_back("Данные не загружены");
	this->allowVerify = false;
}

void VVerifyCurves::GetCSVString(std::ofstream &output, const std::string divider)
{
	output<<this->GetVerifyMark().message[0];
}

std::string VVerifyCurves::GetString()
{
	return this->GetVerifyMark().message[0];
}

//------------------------------------ Класс выход значений параметра за допустимые гранцы --------------------------------------
VVerifyParameterRange::VVerifyParameterRange(VDataSetPtr _dataSet, PARAMETER_TYPE _param, float _high_range, float _low_range)
{
	dataSet = _dataSet;
	param = _param;
	high_range = _high_range;
	low_range = _low_range;

	needAnotherFile = false;
	allowExecute = false;
	allowVerify = false;
	name_test = wstring_to_string(GetUserParamName(_param)).c_str();
	if(this->Load())
		this->verifyMark.verifyLevel = VERIFYING_WARNING;
	else
	{
		this->verifyMark.verifyLevel = VERIFYING_NOT_LOAD;
		this->verifyMark.message.push_back("Данные не загружены");		
	}		
}

bool VVerifyParameterRange::Load()
{
	if(!dataSet->Load(param) || !dataSet->Load(PRM_ABS_COORD))
		return false;
	else
		return true;
}

void VVerifyParameterRange::Verify(VProgressBox &prog, bool calc_offset)
{
	this->verifyMark.message.clear();
	if(this->GetVerifyMark().verifyLevel != VERIFYING_NOT_LOAD)
	{
		for(size_t i = 0; i < dataSet->GetParameter(param).GetCount(); i++)
		{
			if(dataSet->GetParameter(param).GetItem(i) > high_range || dataSet->GetParameter(param).GetItem(i) < low_range)
			{
				char message [64];
				sprintf(message, "%s выходит за допустимые границы", name_test.c_str());
				this->verifyMark.message.push_back((const char *) message);
				sprintf(message, "%.0f м", dataSet->GetAbsCoord().GetItem(i));
				this->verifyMark.message.push_back((const char *) message);
				this->verifyMark.verifyLevel = VERIFYING_ERROR;
				break;
			}
		}
		if(this->verifyMark.verifyLevel != VERIFYING_ERROR)
		{
			this->verifyMark.verifyLevel = VERIFYING_ALL_RIGHT;
			this->verifyMark.message.push_back("Параметр в допустимых границах");
		}
		this->allowVerify = false;
	}
	else
	{
		this->verifyMark.message.push_back("Данные не загружены");	
	}
}

void VVerifyParameterRange::GetCSVString(std::ofstream &output, const std::string divider)
{
	output<<this->GetVerifyMark().message[0];
}

std::string VVerifyParameterRange::GetString()
{
	return this->GetVerifyMark().message[0];
}

//-------------------------------Класс оценки корректности измеренной координаты по меткам Бредюка---------------------
VVerifyAbscoord::VVerifyAbscoord(VDataSetPtr _dataSet, VIBDatabasePtr _db)
{
	dataSet = _dataSet;
	db = _db;

	needAnotherFile = false;
	allowExecute = false;
	name_test = "Коррекция координаты по меткам RFID";
	if(this->Load())
	{
		char msg [256];
		sprintf(msg, "Загружено %i меток", wayMarkerData.size());
		this->verifyMark.message.push_back((const char *) msg);	 
		this->verifyMark.verifyLevel = VERIFYING_WARNING;
		allowVerify = true;
	}
	else
	{
		this->verifyMark.message.push_back("Данные не загружены");	
		this->verifyMark.verifyLevel = VERIFYING_NOT_LOAD;
		allowVerify = false;
	}
}

bool VVerifyAbscoord::Load()
{
	if(dataSet->Load(PRM_ABS_COORD)
		&& dataSet->Makeinfo(db, false, false))
	{
		dataSet->Load(PRM_SPALA_MARKER);
		dataSet->Load(PRM_OP_MARKS);
		deltaAbscoord.resize(dataSet->GetAbsCoord().GetCount(), 0);
		//Загружаем метку старта
		WayMarker new_marker_start;
		new_marker_start.msr_abscoord = dataSet->GetAbsCoord().GetItem(0);
		new_marker_start.apr_abscoord = new_marker_start.msr_abscoord;
		new_marker_start.isIgnore = false;
		new_marker_start.index = 0;
		new_marker_start.type = START_MARKER;
		wayMarkerData.push_back(new_marker_start);

		//Загружаем километровые столбы
		for(size_t i = 0; i < dataSet->GetOpMarks().GetCount(); i++)
		{
			if(dataSet->GetOpMarks().GetItem(i).isPole)
			{
				WayMarker new_marker;
				new_marker.msr_abscoord = dataSet->GetAbsCoord().GetItem(dataSet->GetOpMarks().GetItem(i).index);
				new_marker.index = dataSet->GetOpMarks().GetItem(i).index;
				VWayCoord wc = dataSet->GetAprinfo().GetRealCoord(new_marker.msr_abscoord);
				if(wc.m>500)
				{
					wc.km++;
					wc.m = 0;
				}
				else
					wc.m = 0;
				new_marker.apr_abscoord = dataSet->GetAprinfo().GetAbsCoord(wc);
				new_marker.isIgnore = dataSet->GetOpMarks().GetItem(i).isIgnore;
				new_marker.type = POLE_MARKER;
				wayMarkerData.push_back(new_marker);
			}
		}

		//Загружаем метки Бредюка
		
		VIBTransaction trans = db->StartTransaction();
		VIBDataHandlerParameter handler;
		std::stringstream sql;
		std::string date_trail = dataSet->GetRailDate().ToStringISO();
		sql << "select r.abscoord as abscoord, r.name as name from apr_rfid r ";
		sql << "inner join apr_rfid_period rp on '" << dataSet->GetRailDate().DateToStringISO().c_str() << "' between rp.start_date and rp.end_date ";
		sql << "inner join adm_way w on rp.adm_way_id=w.id ";
		sql << "inner join adm_direction d on w.adm_direction_id=d.id ";
		sql << "where d.name='" << dataSet->GetAprinfo().GetDirName().c_str() << "' and w.code='" << dataSet->GetAprinfo().GetWayCode().c_str() << "' ";
		sql << "order by abscoord";
		try
		{
			db->ExecuteStatement(trans, sql.str().c_str(), handler);
		}
		catch (...)
		{
			return false;
		}
		if(handler.GetRowCount())
		{
			size_t index_msr = 0;
			for(size_t i = 0; i < handler.GetRowCount(); i++)
			{
				std::string apr_name_str = wstring_to_string(handler.GetParameter(i, 1).GetString());
				for(size_t j = index_msr; i < dataSet->GetSpalaMarker().GetCount(); i++)
				{
					char name_c [12];
					sprintf(name_c, "%0#10x", dataSet->GetSpalaMarker().GetItem(i).value);
					std::string name_str = (const char *) name_c;
					if(name_str == apr_name_str)
					{
						WayMarker new_marker;
						new_marker.msr_abscoord = dataSet->GetAbsCoord().GetItem(dataSet->GetSpalaMarker().GetItem(j).index);
						new_marker.index = dataSet->GetSpalaMarker().GetItem(j).index;
						new_marker.apr_abscoord = handler.GetParameter(i, 0).GetFloat();
						new_marker.name = name_str;
						new_marker.isIgnore = false;
						new_marker.type = SPALA_MARKER;
						wayMarkerData.push_back(new_marker);
						index_msr = j+1;
					}
				}
			}
		}

		//Загружаем метки кривых
		std::vector<VAprinfoCurve> aprinfoCurves = dataSet->GetAprinfo().GetPassportCurveList(dataSet->GetAbsCoord().GetItem(0), dataSet->GetAbsCoord().GetLastItem());
		if(aprinfoCurves.size() && dataSet->Load(PRM_CURVE_MARKS))
		{
			size_t index_apr = 0;
			for(size_t i = 0; i < dataSet->GetCurves().GetCount(); i++)
				for(size_t j = index_apr; j < aprinfoCurves.size(); j++)
				{
					size_t start_index = dataSet->GetCurves().GetItem(i).GetPlanMark(0).index;
					size_t end_index = dataSet->GetCurves().GetItem(i).GetPlanMark(dataSet->GetCurves().GetItem(i).GetMarkCount()-1).index;
					double apr_start_coord = aprinfoCurves[j].planLabel[0].coord;
					double apr_end_coord = aprinfoCurves[j].planLabel[aprinfoCurves[j].planLabel.size()-1].coord;
					//Проверяем пересечение априорной и измеренной кривой в файле
					if(dataSet->GetAbsCoord().GetItem(start_index) > apr_start_coord && dataSet->GetAbsCoord().GetItem(start_index) < apr_end_coord
						|| dataSet->GetAbsCoord().GetItem(end_index) > apr_start_coord && dataSet->GetAbsCoord().GetItem(end_index) < apr_end_coord)
						if(dataSet->GetCurves().GetItem(i).GetMarkCount() == aprinfoCurves[j].planLabel.size())
							for(size_t k = 0; k < dataSet->GetCurves().GetItem(i).GetMarkCount(); k++)
							{
								WayMarker new_marker;
								new_marker.msr_abscoord = dataSet->GetAbsCoord().GetItem(dataSet->GetCurves().GetItem(i).GetPlanMark(k).index);
								new_marker.index = dataSet->GetCurves().GetItem(i).GetPlanMark(k).index;
								new_marker.apr_abscoord = aprinfoCurves[j].planLabel[k].coord;
								new_marker.isIgnore = false;
								new_marker.type = CURVE_MARKER;
								wayMarkerData.push_back(new_marker);
								index_apr = j+1;
								break;
							}
					if(dataSet->GetAbsCoord().GetItem(end_index) < apr_start_coord)
						break;
				}
		}
		
		std::sort(wayMarkerData.begin(), wayMarkerData.end());

		//Загружаем метку финиша
		WayMarker new_marker_end;
		new_marker_end.msr_abscoord = dataSet->GetAbsCoord().GetLastItem();
		new_marker_end.apr_abscoord = new_marker_end.msr_abscoord - wayMarkerData[wayMarkerData.size()-1].msr_abscoord + wayMarkerData[wayMarkerData.size()-1].apr_abscoord;
		new_marker_end.isIgnore = false;
		new_marker_end.index = dataSet->GetAbsCoord().GetCount()-1;
		new_marker_end.type = END_MARKER;
		wayMarkerData.push_back(new_marker_end);
		if(wayMarkerData.size()>2)
			return true;
		else
			return false;
	}
	else
		return false;
}

void VVerifyAbscoord::Verify(VProgressBox & prog, bool calc_offset)
{
	this->Verify();
}

void VVerifyAbscoord::Verify()
{
	this->verifyMark.message.clear();
	if(this->GetVerifyMark().verifyLevel != VERIFYING_NOT_LOAD)
	{
		double b;
		double k;
		double SumDelta = 0;
		double SumDelta2 = 0;
		double SumCoord = 0;
		double SumCoord2 = 0;
		double SumCoordDelta = 0;
		size_t N = 0;

		//#pragma omp parallel for reduction(+:SumDelta,SumDelta2, SumCoord, SumCoord2, SumCoordDelta, N)
		for(size_t i = 0; i < wayMarkerData.size()-1; i++)
		{
			if(!wayMarkerData[i].isIgnore)
			{
				double y1 = wayMarkerData[i].msr_abscoord - wayMarkerData[i].apr_abscoord;
				double x1 = dataSet->GetAbsCoord().GetItem(wayMarkerData[i].index);
				double y2 = 0;
				double x2 = 0;
				size_t j;
				for(j = i+1; j < wayMarkerData.size(); j++)
				{
					if(!wayMarkerData[j].isIgnore)
					{
						y2 = wayMarkerData[j].msr_abscoord - wayMarkerData[j].apr_abscoord;
						x2 = dataSet->GetAbsCoord().GetItem(wayMarkerData[j].index);
						break;
					}
				}
				b = (y1*x2 - y2*x1)/(x2-x1);
				k = (y1-y2)/(x1-x2);
				SumDelta += wayMarkerData[i+1].msr_abscoord - wayMarkerData[i+1].apr_abscoord;
				SumDelta2 += pow((wayMarkerData[i+1].msr_abscoord - wayMarkerData[i+1].apr_abscoord),2);
				SumCoord += dataSet->GetAbsCoord().GetItem(wayMarkerData[i+1].index);
				SumCoord2 += pow((dataSet->GetAbsCoord().GetItem(wayMarkerData[i+1].index)),2);
				SumCoordDelta += (wayMarkerData[i+1].msr_abscoord - wayMarkerData[i+1].apr_abscoord)*(dataSet->GetAbsCoord().GetItem(wayMarkerData[i+1].index));
				N++;
				for(size_t l = wayMarkerData[i].index+1; l <= wayMarkerData[j].index; l++)
					deltaAbscoord[l] = (k*dataSet->GetAbsCoord().GetItem(l) + b);
				i = j-1;
			}
		}

		line_t result((SumCoordDelta-(SumCoord*SumDelta)/N)/(SumCoord2-pow(SumCoord,2)/N), -1, (SumDelta - SumCoord*
			(SumCoordDelta-(SumCoord*SumDelta)/N)/(SumCoord2-pow(SumCoord,2)/N))/N);
		char _msg [127];
		sprintf(_msg, "Параметры апрокс. прямой сдвигов: k=%.1f b=%.1f", result.k(), result.b());
		this->verifyMark.message.push_back((const char *) _msg);
		if(result.k() != 0.0 || result.b() != 0.0)
		{
			this->verifyMark.verifyLevel = VERIFYING_WARNING;
			allowExecute = true;
		}
		else
		{
			this->verifyMark.verifyLevel = VERIFYING_ALL_RIGHT;
			allowExecute = false;
		}
	}
	else
		this->verifyMark.message.push_back("Данные не загружены");
	this->allowVerify = false;	
}

void VVerifyAbscoord::Execute()
{
	if(allowExecute)
	{
		size_t index_marker_data = 0;
		size_t kadr = wayMarkerData[index_marker_data].index;
		for(size_t i = 0; i < dataSet->GetAbsCoord().GetCount(); i++)
		{
			if(i == kadr)
			{
				wayMarkerData[index_marker_data].msr_abscoord -= deltaAbscoord[i];
				index_marker_data++;
				if(index_marker_data < wayMarkerData.size())
					kadr = wayMarkerData[index_marker_data].index;
			}
			dataSet->GetAbsCoord().GetItem(i) -= deltaAbscoord[i];
			deltaAbscoord[i] = 0;
		}
		dataSet->SetChanged(PRM_ABS_COORD);
		this->verifyMark.message[1] = "Проведена фазировка";
		this->verifyMark.verifyLevel = VERIFYING_ALL_RIGHT;
		//Записываем информацию в историю операций
		wchar_t histMessage[256] = {0};
		swprintf(histMessage, 255, L"Автокоррекция координаты по меткам");
		VOperationRecord record;
		record.SetTextDescription(wstring_to_string(histMessage).c_str(),256);
		dataSet->GetHistory().Add(record);
		dataSet->SetChanged(PRM_HISTORY);
		//Расставляем флаги
		allowExecute = false;
		isExecute = true;
	}
}

//Возвращает код метки пути
short int VVerifyAbscoord::GetMarkerCode(size_t in_index)
{
	if(in_index == 0)
		return 0;
	else if (in_index == wayMarkerData.size()-1)
		return 1;
	else if (wayMarkerData[in_index].type == SPALA_MARKER)
		return 2;
	else if (wayMarkerData[in_index].type == POLE_MARKER)
		return 3;
	else if (wayMarkerData[in_index].type == CURVE_MARKER)
		return 4;
	else
		return -1;
}

//Возвращает кол-во точек на графике
size_t VVerifyAbscoord::GetCount()
{
	return wayMarkerData.size();
}

//Возвращает кол-во незабракованных точек на графике
size_t VVerifyAbscoord::GetNotIgnoreCount()
{
	size_t count = 0;
	for(size_t i = 0; i < wayMarkerData.size(); i++)
		if(!wayMarkerData[i].isIgnore)
			count++;
	return count;
}

//Возвращает флаг отбраковки метки на пути
bool VVerifyAbscoord::IsIgnore(size_t in_index)
{
	if(in_index == 0)
		return false;
	else if(in_index == wayMarkerData.size()-1)
		return false;
	else
		return wayMarkerData[in_index].isIgnore;
}
//Установить метку браковки
void VVerifyAbscoord::SetIgnore(size_t in_index)
{
	if(in_index == 0)
		return;
	else if(in_index == wayMarkerData.size()-1)
		return;
	else if(!wayMarkerData[in_index].isIgnore)
	{
		wayMarkerData[in_index].isIgnore=true;
		size_t start_index;
		size_t end_index;
		for(size_t i = in_index-1; i >= 0; i--)
			if(!wayMarkerData[i].isIgnore)
			{
				start_index = i;
				break;
			}
		for(size_t i = in_index+1; i < wayMarkerData.size(); i++)
			if(!wayMarkerData[i].isIgnore)
			{
				end_index = i;
				break;
			}
		double y1 = wayMarkerData[start_index].msr_abscoord - wayMarkerData[start_index].apr_abscoord;
		double x1 = dataSet->GetAbsCoord().GetItem(wayMarkerData[start_index].index);
		double y2 = wayMarkerData[end_index].msr_abscoord - wayMarkerData[end_index].apr_abscoord;
		double x2 = dataSet->GetAbsCoord().GetItem(wayMarkerData[end_index].index);
		double b = (y1*x2 - y2*x1)/(x2-x1);
		double k = (y1-y2)/(x1-x2);
		for(size_t i = wayMarkerData[start_index].index; i < wayMarkerData[end_index].index; i++)
			deltaAbscoord[i] = (k*dataSet->GetAbsCoord().GetItem(i) + b);
	}
	allowExecute = true;
}

/** Браковка однотипных меток */
void VVerifyAbscoord::SetIgnore(MARKER_TYPE in_type)
{
	for(size_t i = 0; i < wayMarkerData.size(); i++)
		if(wayMarkerData[i].type == in_type)
			SetIgnore(i);
}

void VVerifyAbscoord::UnsetIgnore(size_t in_index)
{
	if(in_index == 0)
		return;
	else if(in_index == wayMarkerData.size()-1)
		return;
	else if(wayMarkerData[in_index].isIgnore)
	{
		wayMarkerData[in_index].isIgnore=false;
		size_t start_index;
		size_t end_index;
		for(size_t i = in_index-1; i >= 0; i--)
			if(!wayMarkerData[i].isIgnore)
			{
				start_index = i;
				break;
			}
		for(size_t i = in_index+1; i < wayMarkerData.size(); i++)
			if(!wayMarkerData[i].isIgnore)
			{
				end_index = i;
				break;
			}
		double y1 = wayMarkerData[start_index].msr_abscoord - wayMarkerData[start_index].apr_abscoord;
		double x1 = dataSet->GetAbsCoord().GetItem(wayMarkerData[start_index].index);
		double y2 = wayMarkerData[in_index].msr_abscoord - wayMarkerData[in_index].apr_abscoord;
		double x2 = dataSet->GetAbsCoord().GetItem(wayMarkerData[in_index].index);
		double b = (y1*x2 - y2*x1)/(x2-x1);
		double k = (y1-y2)/(x1-x2);
		for(size_t i = wayMarkerData[start_index].index; i < wayMarkerData[in_index].index; i++)
			deltaAbscoord[i] = (k*dataSet->GetAbsCoord().GetItem(i) + b);
		y1 = wayMarkerData[in_index].msr_abscoord - wayMarkerData[in_index].apr_abscoord;
		x1 = dataSet->GetAbsCoord().GetItem(wayMarkerData[in_index].index);
		y2 = wayMarkerData[end_index].msr_abscoord - wayMarkerData[end_index].apr_abscoord;
		x2 = dataSet->GetAbsCoord().GetItem(wayMarkerData[end_index].index);
		b = (y1*x2 - y2*x1)/(x2-x1);
		k = (y1-y2)/(x1-x2);
		for(size_t i = wayMarkerData[in_index].index; i < wayMarkerData[end_index].index; i++)
			deltaAbscoord[i] = (k*dataSet->GetAbsCoord().GetItem(i) + b);
	}
	allowExecute = true;
}

/** Снятие браковки однотипных меток */
void VVerifyAbscoord::UnsetIgnore(MARKER_TYPE in_type)
{
	for(size_t i = 0; i < wayMarkerData.size(); i++)
		if(wayMarkerData[i].type == in_type)
			UnsetIgnore(i);
}

//Возвращает координату метки по оси X
double VVerifyAbscoord::GetX(size_t in_index)
{
	return dataSet->GetAbsCoord().GetItem(wayMarkerData[in_index].index);
}

//Возвращает координату незабракованной метки по оси X
double VVerifyAbscoord::GetNotIgnoreX(size_t in_index)
{
	size_t index_not_ignore = 0;
	for(size_t i = 0; i < wayMarkerData.size(); i++)
	{
		if(index_not_ignore == in_index && !wayMarkerData[i].isIgnore)
			return dataSet->GetAbsCoord().GetItem(wayMarkerData[i].index);
		if(!wayMarkerData[i].isIgnore)
			index_not_ignore++;
	}
	return 0;
}

//Возвращает координату метки по оси Y
double VVerifyAbscoord::GetY(size_t in_index)
{
	return wayMarkerData[in_index].msr_abscoord-wayMarkerData[in_index].apr_abscoord;
}

//Возвращает координату незабракованной метки по оси Y
double VVerifyAbscoord::GetNotIgnoreY(size_t in_index)
{
	size_t index_not_ignore = 0;
	for(size_t i = 0; i < wayMarkerData.size(); i++)
	{
		if(index_not_ignore == in_index && !wayMarkerData[i].isIgnore)
			return wayMarkerData[i].msr_abscoord-wayMarkerData[i].apr_abscoord;
		if(!wayMarkerData[i].isIgnore)
			index_not_ignore++;
	}
	return 0;
}

//Возвращает минимальное значение по оси Y
double VVerifyAbscoord::GetMinY()
{
	double min_delta = -100.0;
	for(size_t i = 0; i < wayMarkerData.size(); i++)
		min_delta = std::min<double>(wayMarkerData[i].msr_abscoord-wayMarkerData[i].apr_abscoord, min_delta);
	return min_delta;
}
//Возвращает максимальное значение по оси Y
double VVerifyAbscoord::GetMaxY()
{
	double max_delta = 100.0;
	for(size_t i = 0; i < wayMarkerData.size(); i++)
		max_delta = std::max<double>(wayMarkerData[i].msr_abscoord-wayMarkerData[i].apr_abscoord, max_delta);
	return max_delta;
}
//Возвращает максимальное значение по оси X
double VVerifyAbscoord::GetMaxX()
{
	return dataSet->GetAbsCoord().GetLastItem();
}
//Возвращает минимальное значение по оси X
double VVerifyAbscoord::GetMinX()
{
	return dataSet->GetAbsCoord().GetItem(0);
}

//---------------------------------Класс составления отчётной формы по множеству файлов измерений----------------------
VVerifyReport::VVerifyReport(const wchar_t * nameReport)
{
	expTextFile.open(nameReport);
	expTextFile << std::fixed;
	expTextFile << VHtml::GetHeader("Оценка качества файлов");
}

void VVerifyReport::AddRowToReport(const wchar_t * fileName, VIBDatabasePtr CurrentDatabase, VProgressBox & prog, bool addLegend)
{
	VDataSetPtr dataSet = VDataSetPtr(new VDataSet(fileName));
	dataSet->Load(PRM_SPEED);
	VVerifyFileList verifyFileList;
	verifyFileList.LoadList(dataSet, CurrentDatabase, prog);
	verifyFileList.Verify(prog, true);
	if(prog.QueryCancel())
		return;
	if(addLegend)
	{
		expTextFile<<"<table><thead><tr><th>Имя файла</th><th>Дата поездки</th><th>Номер вагона</th>";
		for(size_t i = 0; i < verifyFileList.GetCount(); i++)
			expTextFile<<"<th>"<<verifyFileList.GetItem(i)->name_test<<"</th>";
		expTextFile<<"<th>Ориентация</th><th>Направление</th><th>Длина пути</th></tr></thead>"<<std::endl;
		expTextFile<<"<tbody>";
	}
	expTextFile<<"<tr> \n";
	expTextFile<<"<td>";
	expTextFile<<wstring_to_string(dataSet->GetFileShortName()).c_str();
	expTextFile<<"</td>";
	expTextFile<<"<td>"<<dataSet->GetRailDate().DateToStringISO()<<"</td>";
	expTextFile<<"<td>"<<dataSet->GetDivision()<<"</td>";
	for(size_t i = 0; i < verifyFileList.GetCount(); i++)
	{
		if(verifyFileList.GetItem(i)->GetVerifyMark().verifyLevel!=VERIFYING_NOT_LOAD)
			expTextFile<<"<td>"<<verifyFileList.GetItem(i)->GetString()<<"</td>";
		else
			expTextFile<<"<td>Данные не загружены</td>";
	}
	if(dataSet->GetParameter(PRM_SPEED).GetItem(0) > 0)
		expTextFile<<"<td>к.в.</td>";
	else
		expTextFile<<"<td>к.н.</td>";
	if(dataSet->GetMoveDirection()>0)
		expTextFile<<"<td>вперёд</td>";
	else
		expTextFile<<"<td>назад</td>";
	char msg [32];
	sprintf(msg, "%.0f", dataSet->GetAbsCoord().GetLastItem() - dataSet->GetAbsCoord().GetItem(0));
	expTextFile<<"<td>";
	expTextFile<<msg;
	expTextFile<<"</td>";
	expTextFile<<"</tr>"<<std::endl;
}

void VVerifyReport::CloseReport()
{
	expTextFile << "</tbody></table>" << std::endl;
	expTextFile << VHtml::GetFooter().c_str();
	expTextFile.close();
}