#pragma once

#include <valarray>
#include "VDataSet.h"
#include "VParameter.h"
#include "VProgressInterface.h"
#include "parameters.h"
#include "VHtml.h"
#include "VExportImportTextFile.h"

/*static const uint32_t 
	ERROR_WIDTH				= 0x00000002, // аваpия шаблона
	ERROR_WIDTH_FRONT		= 0x00000100, // авария "шк спереди"
	ERROR_WIDTH_BACK		= 0x00004000; // авария "шк сзади"*/

//Список ошибок
enum VERIFYING_LEVEL
{
	VERIFYING_ALL_RIGHT,
	VERIFYING_WARNING,
	VERIFYING_ERROR,
	VERIFYING_NOT_LOAD,
	VERIFYING_NOT_CALC
};

//Структура описывающая резултаты теста
struct VVerifyMark
{
	std::vector<std::string> message;	//сообщения
	VERIFYING_LEVEL verifyLevel;		//степень ошибки
};

//Базовый класс для верификации файла
class VVerifyFile
{
protected:
	VVerifyMark verifyMark;							//Структура, оценивающая степень верификации
	std::vector<std::string> methodsDescription;	//Описания способов устранения сдвигов

public:
	//Конструктор класса
	VVerifyFile()
	{
		needAnotherFile = false;
		allowExecute = false;
		allowVerify = true;
		isExecute = false;
	}
	//Блок данных
	VDataSetPtr dataSet;
	//необходимость другого файла
	bool needAnotherFile;
	//возможность исправления
	bool allowExecute;
	//возможность проведения дополнительной проверки
	bool allowVerify;
	//Флаг проведения автоматического устранения дефекта
	bool isExecute;
	//название теста
	std::string name_test;
	//Метод загрузки данных
	virtual bool Load() {return true;};
	//Метод, оценивающий степень верификации с другими параметрами из того же файла
	virtual void Verify(VProgressBox & prog, bool calc_offset) = 0;
	//Метод, оценивающий степень верификации с параметрами из других файлов
	virtual void Verify(VDataSetPtr dataSetAnoth, VProgressBox & prog, bool calc_offset) {};
	//Метод автоматического устранения дефекта
	virtual void Execute(VProgressBox & prog) {};
	//Метод получения информации о степени верификации
	VVerifyMark GetVerifyMark() 
	{
		return verifyMark;
	};
	//Метод возвращающий строку в CSV
	virtual void GetCSVString(std::ofstream & output, const std::string divider) {};
	//Метод возвращающий кол-во способов, которыми можно исправить ошибку
	short int GetCountMethod()
	{
		return methodsDescription.size();
	};
	//Метод возвращающий описание методов
	std::string GetMethodDescription(short int method_index) 
	{
		return methodsDescription[method_index];
	};
	//Функция проверки исправления различных параметров
	virtual bool IsChanged()
	{
		return false;
	};
	//Метод, возвращающий строку с описанием результатов теста
	virtual std::string GetString()
	{ 
		return "";
	};
};
typedef boost::shared_ptr<VVerifyFile> VVerifyFilePtr; 

//Контейнер для всех классов оценивающих верификацию файла
class VVerifyFileList : public VIndexList <VVerifyFilePtr>
{
public:
	//Информация о файле
	std::string orientation;	//ориентация вагона
	std::string date_trail;		//время поездки
	std::string direction;		//направление движения
	VDataSetPtr dataSet;		//Блок данных

	// Метод загрузки всех тестов
	void LoadList(VDataSetPtr _dataSet, VIBDatabasePtr db, VProgressBox & prog);
	// Метод для оценки по всем тестам
	void Verify(VProgressBox & prog, bool calc_offset);
	// Метод для автоматического устранения всех ошибок
	void Execute(VProgressBox & prog);
	// Метод для пересчёта верифицируемых данных
	void Refresh(){};
	// Метод для вывода всей информации в CSV
	void GetCSVResults(const std::string in_path, FILE_TYPE in_type);
	// Функция сохранения всех сфазированных параметров
	void Save();
	//Метод обновления результатов тестов
	void Update();
}; 

//Класс фазировки параметров
class VVerifyParameter : public VVerifyFile
{
protected:
	PARAMETER_TYPE parameterEthalon;    //название эталонного параметра
	PARAMETER_TYPE parameterPhased;     //название фазируемого параметра
	double coefVerifyOld;				//коэффициент корреляции между ШК и ШК Спереди старый
	double coefVerifyNew;				//коэффициент корреляции новый
	int valueExecute;					//значение, на которое необходимо сместить параметры
	double abscoordExecute;				//значение, на которое необходимо сместить параметры в метрах
	//Метод загрузки сбойных параметров
	void LoadCorrupt();
	//Метод фазировки
	int method_index;
public:
	//Конструктор класса
	VVerifyParameter(PARAMETER_TYPE _parameterPhased, PARAMETER_TYPE _parameterEthalon, VDataSetPtr _dataSet, short int _method_index);
	//Метод загрузки данных
	virtual bool Load();
	//Метод, оценивающий степень верификации с другими параметрами из того же файла
	void Verify(VProgressBox & prog, bool calc_offset);
	//Метод, оценивающий степень верификации с параметрами из других файлов
	void Verify(VDataSetPtr dataSetAnoth, VProgressBox & prog, bool calc_offset) {};
	//Метод автоматического устранения дефекта
	virtual void Execute(VProgressBox & prog);
	//Метод возвращающий строку в CSV
	void GetCSVString(std::ofstream & output, const std::string divider);
	//Функция, возвращающая эталонный параметр
	PARAMETER_TYPE GetEthalonParameter();
	//Функция, возвращающая фазируемый параметр
	PARAMETER_TYPE GetPhasedParameter();
	//Функция проверки исправления различных параметров
	bool IsChanged();
	//Метод, возвращающий строку с описанием результатов теста
	std::string GetString();
};
typedef boost::shared_ptr<VVerifyParameter> VVerifyParameterPtr;

//Новый класс фазировки Курса
class VVerifyKurs : public VVerifyParameter
{
public:
	//Конструктор класса
	VVerifyKurs(VDataSetPtr _dataSet, short int _method_index);
	//Метод автоматического устранения сдвига
	void Execute(VProgressBox & prog);
	//Функция проверки исправления различных параметров
	bool IsChanged();
};
typedef boost::shared_ptr<VVerifyKurs> VVerifyKursPtr;

//Класс фазировки ППР
class VVerifyPPR : public VVerifyParameter
{
public:
	//Конструктор класса
	VVerifyPPR(PARAMETER_TYPE _parameterPhased, PARAMETER_TYPE _parameterEthalon, VDataSetPtr _dataSet, short int _method_index);
	//Метод автоматического устранения сдвига
	void Execute(VProgressBox & prog);
};
typedef boost::shared_ptr<VVerifyPPR> VVerifyPPRPtr;


//Класс оценки МО ШК и Возвышения на контрольном участке
class VVerifyWidLev : public VVerifyFile
{
	//Средние значения ШК и уровня на контрольных участках из БД
	std::vector<VAprinfoControlAreaRecord> apr_control_area;
	//Структура, описывающая МО ШК и уровня по результатам измерений
	struct MsrControlArea
	{
		double mo_width;
		double mo_level;
	};
	//Средние значения ШК и уровня на контрольных участках по результатам измерений
	std::vector<MsrControlArea> msr_control_area;
	//Допустимое отклонение для среднего значения ШК
	double width_range;
	//Допустимое отклонение для среднего значения Уровня
	double level_range;
public:
	//Конструктор класса
	VVerifyWidLev(VDataSetPtr _dataSet, VIBDatabasePtr _db);
	//Метод загрузки данных
	bool Load(VIBDatabasePtr _db);
	//Метод оценивающий разницу априорных и измеренных МО на контрольных участках
	void Verify(VProgressBox & prog, bool calc_offset);
	//Метод возвращающий строку в CSV
	void GetCSVString(std::ofstream & output, const std::string divider);
	//Функция, возвращающая кол-во способов, которыми можно устранить проблему
	short int GetCountMethod()
	{
		return 0;
	};
	//Метод, возвращающий строку с описанием результатов теста
	std::string GetString();
};
typedef boost::shared_ptr<VVerifyWidLev> VVerifyWidLevPtr;

//Класс оценивающий кол-ва отбитых километровых столбов
class VVerifyPole : public VVerifyFile
{
	int procent;
public:
	//Конструктор класса
	VVerifyPole(VDataSetPtr _dataSet, VIBDatabasePtr _db);
	//Метод загрузки данных
	bool Load(VIBDatabasePtr _db);
	//Метод оценивающий процент отбитых км столбов
	void Verify(VProgressBox & prog, bool calc_offset);
	//Метод возвращающий строку в CSV
	void GetCSVString(std::ofstream & output, const std::string divider);
	//Функция, возвращающая кол-во способов, которыми можно устранить проблему
	short int GetCountMethod()
	{
		return 0;
	};
	//Метод, возвращающий строку с описанием результатов теста
	std::string GetString();
};
typedef boost::shared_ptr<VVerifyPole> VVerifyPolePtr;

//Класс оценивающий отношение Паспортных кривых к Фактическим
class VVerifyCurves : public VVerifyFile
{
	size_t count_pasport;
	size_t count_fact;
public:
	//Конструктор класса
	VVerifyCurves(VDataSetPtr _dataSet, VIBDatabasePtr _db);
	//Метод загрузки данных
	bool Load(VIBDatabasePtr _db);
	//Метод оценивающий отношение
	void Verify(VProgressBox & prog, bool calc_offset);
	//Метод возвращающий строку в CSV
	void GetCSVString(std::ofstream & output, const std::string divider);
	//Функция, возвращающая кол-во способов, которыми можно устранить проблему
	short int GetCountMethod()
	{
		return 0;
	};
	//Метод, возвращающий строку с описанием результатов теста
	std::string GetString();
};
typedef boost::shared_ptr<VVerifyCurves> VVerifyCurvesPtr;

//Класс, оценивающий выход значений параметра за физически допустимые границы
class VVerifyParameterRange : public VVerifyFile
{
	PARAMETER_TYPE param;
	float high_range;
	float low_range;
public:
	//Конструктор класса
	VVerifyParameterRange(VDataSetPtr _dataSet, PARAMETER_TYPE _param, float _high_range, float _low_range);
	//Метод загрузки данных
	bool Load();
	//Метод оценивающий отношение
	void Verify(VProgressBox & prog, bool calc_offset);
	//Метод возвращающий строку в CSV
	void GetCSVString(std::ofstream & output, const std::string divider);
	//Функция, возвращающая кол-во способов, которыми можно устранить проблему
	short int GetCountMethod()
	{
		return 0;
	};
	//Метод, возвращающий строку с описанием результатов теста
	std::string GetString();
};
typedef boost::shared_ptr<VVerifyParameterRange> VVerifyParameterRangePtr;

enum MARKER_TYPE
{
	ANYTHING_MARKER,
	START_MARKER,
	END_MARKER,
	SPALA_MARKER,
	POLE_MARKER,
	CURVE_MARKER,
	ZAZOR_MARKER,
};

struct WayMarker
{
	MARKER_TYPE type;
	std::string name;
	double msr_abscoord;
	double apr_abscoord;
	uint32_t index;
	bool isIgnore;

	bool operator < (WayMarker way_marker)
	{
		return msr_abscoord < way_marker.msr_abscoord;
	}
};

//Коррекция координаты с помощью меток RFID
class VVerifyAbscoord : public VVerifyFile
{
	//Вектор с путевыми метками
	std::vector<WayMarker> wayMarkerData;
	//Массив с рекомендуемыми сдвигами
	valarray<double> deltaAbscoord;
	VIBDatabasePtr db;
public:
	//Конструктор класса
	VVerifyAbscoord(VDataSetPtr _dataSet, VIBDatabasePtr _db);
	//Метод загрузки данных
	bool Load();

	void Verify(VProgressBox & prog, bool calc_offset);
	//Метод оценки скорректированности абсолютной координаты
	void Verify();
	//Метод устранения дефекта
	void Execute();

	/* Методы для графики */
	//Возвращает код метки пути
	short int GetMarkerCode(size_t in_index);
	//Возвращает кол-во точек на графике
	size_t GetCount();
	//Возвращает кол-во незабракованных точек на графике
	size_t GetNotIgnoreCount();
	//Возвращает флаг отбраковки метки на пути
	bool IsIgnore(size_t in_index);
	//Установить метку браковки
	void SetIgnore(size_t in_index);
	void SetIgnore(MARKER_TYPE in_type);
	//Снять отбраковку
	void UnsetIgnore(size_t in_index);
	void UnsetIgnore(MARKER_TYPE in_type);
	//Возвращает координату метки по оси X
	double GetX(size_t in_index);
	//Возвращает координату незабракованной метки по оси X
	double GetNotIgnoreX(size_t in_index);
	//Возвращает координату метки по оси Y
	double GetY(size_t in_index);
	//Возвращает координату незабракованной метки по оси Y
	double GetNotIgnoreY(size_t in_index);
	//Возвращает минимальное значение по оси Y
	double GetMinY();
	//Возвращает максимальное значение по оси Y
	double GetMaxY();
	//Возвращает максимальное значение по оси X
	double GetMaxX();
	//Возвращает минимальное значение по оси X
	double GetMinX();
};
typedef boost::shared_ptr<VVerifyAbscoord> VVerifyAbscoordPtr;

//Класс составления отчётной формы, содержащей информацию 
//о верификации файлов
class VVerifyReport
{
	ofstream expTextFile;
public:
	//Конструктор класса
	VVerifyReport(const wchar_t * reportName);
	//Метод добавления в ведомость строчки с результатами тестов
	void AddRowToReport (const wchar_t * fileName, VIBDatabasePtr CurrentDatabase, VProgressBox & prog, bool addLegend = false);
	//Метод окончания составления ведомости
	void CloseReport();
};