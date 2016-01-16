#pragma once

#include <boost/asio.hpp>

#include <string> 
#include "strlib.h"
#include "VLog.h"
#include "options.h"

//Boost
#include <boost/format.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/split.hpp>

#define AVI_RECORDING 80301
#define PNG_RECORDING 80302
#define V4F_RECORDING 80303

#define RTSP_CAMERA 5600
#define JAI_CAMERA 5601
#define FIREWIRE_CAMERA 5602

#define CAMERA_CONFIG 3L

///Класс свойств камеры
class VCameraBase {
protected:
	///Тип камеры
	int type;
	///Имя камеры
	std::wstring cameraName;
	///Папка для записи
	//std::wstring directory;
	///Режим съёмки камеры
	int type_rec;
public:
	///Конструктор по умолчанию
	VCameraBase();
	///Конструктор копирования
	VCameraBase(VCameraBase & src);
	///Конструктор с параметрами
	VCameraBase(int _type, std::wstring Name, int _type_rec);
	///Виртуальный деструктор
	virtual ~ VCameraBase(){}
	///Метод для получения имени камеры
	std::wstring GetCameraName();
	///Метод для получения пути к папке для записи
	//std::wstring GetDirectory();
	///Метод для получения типа записи видео
	int GetTypeRec();
	///Метод для получения типа камеры
	int GetType();

	///Метод для получения указателя на имя камеры
	std::wstring * GetCameraNamePtr();
	///Метод для получения указателя на путь к папке для записи
	//std::wstring * GetDirectoryPtr();
	///Метод для получения указателя на тип съёмки
	int * GetTypeRecPtr();

	///Метод для задания нового имени камеры
	void SetCameraName(std::wstring newCameraName );
	///Метод для задания новой папки для записи
	void SetDirectory(std::wstring newDirectory);
	///Метод для задания типа камеры
	bool SetType(int newType);
	///Вернуть строку для файла конфигурации
	virtual std::wstring GetConfigStr() = 0;
};

/// Класс свойств IP камеры
class VRTSPCameraProperties : public VCameraBase
{
	/// Адрес камеры
	std::wstring address;
public:
	/// Конструктор
	VRTSPCameraProperties(const std::wstring& in_name, const std::wstring& in_address, const std::wstring & in_directory, const int & in_type_rec);
	/// Конструктор со строкой из файла конфигурации
	VRTSPCameraProperties(std::wstring conf_str, int version);
	/// Деструктор
	virtual ~VRTSPCameraProperties(){};
	/// Возвращает адрес камеры
	const std::wstring& GetAddress() const;
	///Метод для получения указателя на адрес камеры
	std::wstring * GetAddressPtr();
	///Вернуть строку для файла конфигурации
	std::wstring GetConfigStr();	
};

//Класс свойств подвагонных камер
class VJAICameraProperties : public VCameraBase
{
	/// Номер камеры
	int numCamera;
public:
	/// Конструктор
	VJAICameraProperties(const std::wstring & in_name);
	/// Конструктор со строкой из файла конфигурации
	VJAICameraProperties(std::wstring conf_str, int version);
	/// Деструктор
	virtual ~VJAICameraProperties(){};
	///Метод для получения указателя на номер камеры
	int * GetNumCameraPtr();
	///Вернуть строку для файла конфигурации
	std::wstring GetConfigStr();
};