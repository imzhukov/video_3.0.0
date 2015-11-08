#pragma once

#include "calclib.h"
#include "dbfunc.h"
#include "axes.h"
#include "elements.h"
#include "cairo_ex.h"

/** Показать / скрыть слой*/
class VSetLayerVisible : public VAction
{
private : 	
	/** Слой*/
	VLayerPtr layer;	
public:
	/** Конструктор*/
	VSetLayerVisible(VLayerPtr in_layer, const std::wstring& in_name)
	{		
		layer=in_layer;
		name=in_name;
	};
	/** Выполнение*/
	void Execute()
	{
		layer->SetVisible(!layer->GetVisible());		
	};		
};
typedef boost::shared_ptr<VSetLayerVisible> VSetLayerVisiblePtr;

/** Изменить масштаб в группе областей по координате*/
class VZoomInXAction : public VAction
{
private:
	/** Указатель на систему координат*/
	std::vector<VAxesPtr> axes;	
public:
	/** Конструктор*/
	VZoomInXAction(const std::vector<VAxesPtr>& in_axes, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VZoomInXAction> VZoomInXActionPtr;

/** Изменить масштаб в группе областей по координате*/
class VZoomOutXAction : public VAction
{
private:
	/** Указатель на систему координат*/
	std::vector<VAxesPtr> axes;	
public:
	/** Конструктор*/
	VZoomOutXAction(const std::vector<VAxesPtr>& in_axes, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VZoomOutXAction> VZoomOutXActionPtr;


/** Изменить масштаб в области по параметру*/
class VZoomInYAction : public VAction
{
private:
	/** Указатель на систему координат*/
	VAxesWeakPtr axes;	
public:
	/** Конструктор*/
	VZoomInYAction(VAxesWeakPtr in_axes, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VZoomInYAction> VZoomInYActionPtr;

/** Изменить масштаб в области по параметру*/
class VZoomOutYAction : public VAction
{
private:
	/** Указатель на систему координат*/
	VAxesWeakPtr axes;	
public:
	/** Конструктор*/
	VZoomOutYAction(VAxesWeakPtr in_axes, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VZoomOutYAction> VZoomOutYActionPtr;

/** Передвинуть начало координат*/
class VMoveOriginY : public VAction
{
private:
	/** Указатель на систему координат*/
	VAxesWeakPtr axes;	
	/** Шаг*/
	double step;
public:
	/** Конструктор*/
	VMoveOriginY(VAxesWeakPtr in_axes, double in_step, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VMoveOriginY> VMoveOriginYPtr;

/** Установить масштаб по координате*/
class VSetScaleX : public VAction
{
private:
	/** Указатель на систему координат*/
	std::vector<VAxesPtr> axes;
	/** Список доступный масштабов*/
	VScaleQueue scales;
	/** Диалог выбора*/
	VAUISelectItemDialogPtr select_dialog;
public:
	/** Конструктор*/
	VSetScaleX(const std::vector<VAxesPtr>& in_axes, const VScaleQueue& in_scales, 
		VAUISelectItemDialogPtr in_select_dialog, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VSetScaleX> VSetScaleXPtr;

/** Изменить размер шрифта*/
class VChangeFontSize : public VAction
{
private:
	/** Шаг изменения размера шрифта*/
	double step;
public:
	/** Конструктор*/
	VChangeFontSize(double in_step, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VChangeFontSize> VChangeFontSizePtr;

/** Автоматически масштабировать график*/
class VAutoScaleAction : public VAction
{
private:
	/** Указатель на систему координат*/
	VAxesWeakPtr axes;	
public:
	/** Конструктор*/
	VAutoScaleAction(VAxesWeakPtr in_axes, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();	
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VAutoScaleAction> VAutoScaleActionPtr;

/** Установить масштаб по умолчанию*/
class VDefaultScaleAction : public VAction
{
private:
	/** Указатель на систему координат*/
	VAxesWeakPtr axes;	
public:
	/** Конструктор*/
	VDefaultScaleAction(VAxesWeakPtr in_axes, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();	
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VDefaultScaleAction> VDefaultScaleActionPtr;

/** Сдвинуть координаты в файле*/
class VShiftCoordinatesAction : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
	/** Величина изменения масштаба*/
	double step;
	/** Диалог запроса сдвига*/
	mutable VAUIInputDialogPtr input_dialog;
public:
	/** Конструктор*/
	VShiftCoordinatesAction(VDataSetPtr in_data_set, double in_step, const std::wstring& in_name);
	/** Конструктор*/
	VShiftCoordinatesAction(VDataSetPtr in_data_set, double in_step, const std::wstring& in_name, VAUIInputDialogPtr in_input_dialog);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VShiftCoordinatesAction> VShiftCoordinatesActionPtr;

/** Растянуть или сжать координаты в файле*/
class VScaleCoordinatesAction : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
	/** Величина изменения масштаба*/
	double scale;
public:
	/** Конструктор*/
	VScaleCoordinatesAction(VDataSetPtr in_data_set, double in_scale, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VScaleCoordinatesAction> VScaleCoordinatesActionPtr;


/** Установить значения флагов параметра в выделенной точке*/
class VSetActiveFlagsAction : public VAction
{
private:	
	
	/** Данные*/
	VDataProviderPtr data;
	/** Установить или снять*/
	uint64_t set_flag;	
	
public:
	/** Конструктор*/
	VSetActiveFlagsAction(VDataProviderPtr in_data,
		uint64_t in_set_flag, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VSetActiveFlagsAction> VSetActiveFlagsActionPtr;


/** Действие - копирование параметров в новый файл*/
class VCopyParametersAction : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
	/** Выделение*/
	VSelectionInfoPtr selection;
	/** Диалог*/
	VAUISaveFileDialogPtr save_file_dialog;
public:
	/** Конструктор*/
	VCopyParametersAction(VDataSetPtr in_data_set, VSelectionInfoPtr in_selection, VAUISaveFileDialogPtr in_save_file_dialog, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VCopyParametersAction> VCopyParametersActionPtr;

/** Записать параметр*/
class VSaveParameterAction : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
	/** Тип данных*/
	PARAMETER_TYPE data_type;
public:
	/** Конструктор*/
	VSaveParameterAction(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VSaveParameterAction> VSaveParameterActionPtr;

/** Восстановить параметр*/
class VReloadParameterAction : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
	/** Тип данных*/
	PARAMETER_TYPE data_type;
public:
	/** Конструктор*/
	VReloadParameterAction(VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VReloadParameterAction> VReloadParameterActionPtr;

/** Записать изменения*/
class VSaveChangesAction : public VAction
{
private:
	/** Источник данных*/
	std::vector<VDataSetPtr> data_set;	
public:
	/** Конструктор*/
	VSaveChangesAction(const std::vector<VDataSetPtr>& in_data_set, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VSaveChangesAction> VSaveChangesActionPtr;

/** Удалить данные*/
class VDeleteAllAction : public VAction
{
private:
	/** Данные*/
	VDataProviderPtr data;

public:
	/** Конструктор*/
	VDeleteAllAction(VDataProviderPtr in_data, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
	/** Возвращает признак необходимости запроса подтверждения*/
	virtual bool NeedAccept() const{return true;};
	/** Возвращает строку запроса подтверждения*/
	virtual std::wstring GetAcceptQuestion() const{return L"Данные будут удалены. Продолжить?";};
};
typedef boost::shared_ptr<VDeleteAllAction> VDeleteAllActionPtr;


/** Автоматически разметить профиль*/
class VAutomarkProfileAction : public VAction
{
private:
	/** Данные*/
	VDataSetPtr data_set;
	/** Прогресс*/
	VAUIProgressDialogPtr progress_dialog;
	/** Диалог ввода параметров*/
	VAUIPropertyDialogPtr property_dialog;
public:
	/** Конструктор*/
	VAutomarkProfileAction(VDataSetPtr in_data_set, VAUIProgressDialogPtr in_progress_dialog);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
	/** Возвращает признак необходимости запроса подтверждения*/
	virtual bool NeedAccept() const{return true;};
	/** Возвращает строку запроса подтверждения*/
	virtual std::wstring GetAcceptQuestion() const{return L"Существующие метки будут удалены. Продолжить?";};	
};
typedef boost::shared_ptr<VAutomarkProfileAction> VAutomarkProfileActionPtr;

/** Перенести разметку профиля из БД*/
class VProfileFromDBAction : public VAction
{
private:
	/// Данные
	VDataSetPtr data_set;
	/// База данных
	VIBDatabasePtr database;
	/// Прогресс
	VAUIProgressDialogPtr progress_dialog;

public:
	/** Конструктор*/
	VProfileFromDBAction(VDataSetPtr in_data_set, VIBDatabasePtr in_database, VAUIProgressDialogPtr in_progress_dialog);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
	/** Возвращает признак необходимости запроса подтверждения*/
	virtual bool NeedAccept() const{return true;};
	/** Возвращает строку запроса подтверждения*/
	virtual std::wstring GetAcceptQuestion() const{return L"Существующие метки будут удалены. Продолжить?";};	
};
typedef boost::shared_ptr<VProfileFromDBAction> VProfileFromDBActionPtr;


/** Удалить точку*/
class VDeleteDataItemAction : public VAction
{
private:
	/** Данные*/
	VDataProviderPtr data;
	
public:
	/** Конструктор*/
	VDeleteDataItemAction(VDataProviderPtr in_data, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VDeleteDataItemAction> VDeleteDataItemActionPtr;

/** Сделать объект активным*/
class VActivateAction : public VAction
{
private:
	/** Данные*/
	VActiveObjectPtr data;		
public:
	/** Конструктор*/
	VActivateAction(VActiveObjectPtr in_data, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
	/** Возвращает имя - описание действия*/
	virtual std::wstring& GetName();
};
typedef boost::shared_ptr<VActivateAction> VActivateActionPtr;

/** Сделать данные неактивными*/
class VDeactivateAction : public VAction
{
private:
	/** Данные*/
	VActiveObjectPtr data;		
public:
	/** Конструктор*/
	VDeactivateAction(VActiveObjectPtr in_data, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
	/** Возвращает имя - описание действия*/
	virtual std::wstring& GetName();	
};
typedef boost::shared_ptr<VDeactivateAction> VDeactivateActionPtr;

/** Установить источник уклонов*/
class VSetSlopeType : public VAction
{
private:
	/** Данные*/
	VSlopeDataPtr data;		
	/** Режим работы*/
	PARAMETER_TYPE data_type;
public:
	/** Конструктор*/
	VSetSlopeType(VSlopeDataPtr in_data, PARAMETER_TYPE in_data_type, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VSetSlopeType> VSetSlopeTypePtr;

/** Установить источник оцифровки профиля*/
class VSetHeightType : public VAction
{
private:
	/** Данные*/
	VHeightDataPtr data;		
	/** Режим работы*/
	PARAMETER_TYPE data_type;
public:
	/** Конструктор*/
	VSetHeightType(VHeightDataPtr in_data, PARAMETER_TYPE in_data_type, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VSetHeightType> VSetHeightTypePtr;

/** Объявить выделенный участок боковым путем*/
class VSelectionToSideway : public VAction
{
private:
	/** Данные*/
	VDataProviderPtr data;
	/** Выделение*/
	VSelectionInfoPtr selection;
	/** Тип*/
	uint32_t reason;	
public:
	/** Конструктор*/
	VSelectionToSideway(VDataProviderPtr in_data, VSelectionInfoPtr in_selection, uint32_t in_reason, 
		const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VSelectionToSideway> VSelectionToSidewayPtr;

/** Объявить выделенный участок станционным путем*/
class VExtractStationWay : public VAction
{
private:
	/** Данные*/
	VDataSetPtr data_set;
	/** Выделение*/
	VSelectionInfoPtr selection;
	/** Тип*/
	uint32_t reason;
	/** Указатель да диалог выбора*/
	VAUISelectItemDialogPtr select_dialog;
	/** Код станции*/
	uint32_t station_code;
	/** Название станции*/
	std::string station_name;
	/** Название парка*/
	std::string park_name;
	/** Указатель на БД*/
	VIBDatabasePtr database;
	/** Указатель на транзакцию*/
	VIBTransactionPtr transaction;
public:
	/** Конструктор*/
	VExtractStationWay(VDataSetPtr in_data_set, VIBDatabasePtr in_database, 
		VIBTransactionPtr in_transaction, VSelectionInfoPtr in_selection, 
		VAUISelectItemDialogPtr in_select_dialog, uint32_t in_station_code, const std::string& in_station_name, 
		const std::string& in_park_name, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VExtractStationWay> VExtractStationWayPtr;

/** Привязка объекта станционного пути к метке*/
class VAttachStationObject : public VAction
{
private:
	/** Источник данных о метках*/
	VDataProviderPtr mark_data;	
	/** База данныех*/
	VIBDatabasePtr database; 
	/** Транзакция*/
	VIBTransactionPtr transaction; 
	/** Указатель да диалог выбора*/
	VAUISelectItemDialogPtr select_dialog;
	/** Код станции*/
	uint32_t station_code; 
	/** Парк*/
	std::string park_name;
	/** Путь*/
	std::string way_code;
public:
	/** Конструктор*/
	VAttachStationObject(VDataProviderPtr in_mark_data, 
		VIBDatabasePtr in_database, VIBTransactionPtr in_transaction, VAUISelectItemDialogPtr in_select_dialog,
		uint32_t in_station_code, const std::string& in_park_name, const std::string& in_way_code, 
		const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VAttachStationObject> VAttachStationObjectPtr;

/** Удаление привязки объекта станционного пути к метке*/
class VDetachStationObject : public VAction
{
private:
	/** Источник данных о метках*/
	VDataProviderPtr mark_data;	
	/** База данныех*/
	VIBDatabasePtr database; 
	/** Транзакция*/
	VIBTransactionPtr transaction;
	/** Код станции*/
	uint32_t station_code; 
	/** Парк*/
	std::string park_name;
public:
	/** Конструктор*/
	VDetachStationObject(VDataProviderPtr in_mark_data,
		VIBDatabasePtr in_database, VIBTransactionPtr in_transaction,
		uint32_t in_station_code, const std::string& in_park_name, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VDetachStationObject> VDetachStationObjectPtr;

/** Боковой путь -> выделение*/
class VSidewayToSelection : public VAction
{
private:
	/** Данные*/
	VDataProviderPtr data;
	/** Выделение*/
	VSelectionInfoPtr selection;
	/** Тип*/
	uint32_t reason;
public:
	/** Конструктор*/
	VSidewayToSelection(VDataProviderPtr in_data, VSelectionInfoPtr in_selection, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
	/** Обработка события*/
	bool ProcessEvent(const VEvent& in_event);
};
typedef boost::shared_ptr<VSidewayToSelection> VSidewayToSelectionPtr;

/** Действие - переход к следующей кривой*/
class VActivateNextCurve : public VAction
{
private:	
	/** Источник данных*/
	VDataSetPtr data_set;
	/** Указатель на рисунок*/ //FixMe!!! указатель на внешний объект
	VDrawing * drawing;
	/** Тип меток*/
	PARAMETER_TYPE mark_type;
public:
	/** Конструктор*/
	VActivateNextCurve(VDataSetPtr in_data_set, VDrawing * in_drawing, 
		const std::wstring& in_name, PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;		
};
typedef boost::shared_ptr<VActivateNextCurve> VActivateNextCurvePtr;


/** Нормализация кривой*/
class VNormalizeCurve : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;	
	/** Тип меток*/
	PARAMETER_TYPE mark_type;
public:
	/** Конструктор*/
	VNormalizeCurve(VDataSetPtr in_data_set, const std::wstring& in_name, 
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VNormalizeCurve> VNormalizeCurvePtr;


/** Оптимизация кривой*/
class VOptimizeCurve : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
	/** Тип меток*/
	PARAMETER_TYPE mark_type;
public:
	/** Конструктор*/
	VOptimizeCurve(VDataSetPtr in_data_set, const std::wstring& in_name, 
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VOptimizeCurve> VOptimizeCurvePtr;

/// Сместить уровень вверх
class VIncreaseLevelAction : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
	/** Тип меток*/
	PARAMETER_TYPE mark_type;
public:
	/** Конструктор*/
	VIncreaseLevelAction(VDataSetPtr in_data_set, const std::wstring& in_name,
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VIncreaseLevelAction> VIncreaseLevelActionPtr;

/// Сместить уровень вниз
class VDecreaseLevelAction : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
	/** Тип меток*/
	PARAMETER_TYPE mark_type;
public:
	/** Конструктор*/
	VDecreaseLevelAction(VDataSetPtr in_data_set, const std::wstring& in_name,
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VDecreaseLevelAction> VDecreaseLevelActionPtr;

/** Приравнять координаты меток на уровне к координатам меток на кривизне*/
class VFixLevelCoordinates : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
	/** Тип меток*/
	PARAMETER_TYPE mark_type;
public:
	/** Конструктор*/
	VFixLevelCoordinates(VDataSetPtr in_data_set, const std::wstring& in_name,
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VFixLevelCoordinates> VFixLevelCoordinatesPtr;

/** Рассчитать оптимальное возвышение для паспортной кривой*/
class VCreateLevelOptimalAction : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
public:
	/** Конструктор*/
	VCreateLevelOptimalAction(VDataSetPtr in_data_set, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VCreateLevelOptimalAction> VCreateLevelOptimalActionPtr;

/** Рассчитать рациональное возвышение для паспортной кривой*/
class VCreateLevelRationalAction : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
public:
	/** Конструктор*/
	VCreateLevelRationalAction(VDataSetPtr in_data_set, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VCreateLevelRationalAction> VCreateLevelRationalActionPtr;

/** Рассчитать возвышение близкое к факту*/
class VCreateLevelFactAction : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
public:
	/** Конструктор*/
	VCreateLevelFactAction(VDataSetPtr in_data_set, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VCreateLevelFactAction> VCreateLevelFactActionPtr;


/** Рассчитать оптимальное возвышение (для всех кривых)*/
class VCreateLevelOptimalAllCurves : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
public:
	/** Конструктор*/
	VCreateLevelOptimalAllCurves(VDataSetPtr in_data_set, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VCreateLevelOptimalAllCurves> VCreateLevelOptimalAllCurvesPtr;

/** Рассчитать рациональное возвышение (для всех кривых)*/
class VCreateLevelRationalAllCurves : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
public:
	/** Конструктор*/
	VCreateLevelRationalAllCurves(VDataSetPtr in_data_set, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VCreateLevelRationalAllCurves> VCreateLevelRationalAllCurvesPtr;

/** Рассчитать возвышение близкое к факту (для всех кривых)*/
class VCreateLevelFactAllCurves : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
public:
	/** Конструктор*/
	VCreateLevelFactAllCurves(VDataSetPtr in_data_set, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VCreateLevelFactAllCurves> VCreateLevelFactAllCurvesPtr;


/** Добавить кривую в список*/
class VApplyActiveCurve : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
	/** Тип меток*/
	PARAMETER_TYPE mark_type;
public:
	/** Конструктор*/
	VApplyActiveCurve(VDataSetPtr in_data_set, const std::wstring& in_name,
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;	
};
typedef boost::shared_ptr<VApplyActiveCurve> VApplyActiveCurvePtr;

/** Действие - удалить текущую кривую*/
class VDeleteActiveCurveAction : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
	/** Тип меток*/
	PARAMETER_TYPE mark_type;
public:
	/** Конструктор*/
	VDeleteActiveCurveAction(VDataSetPtr data_set, const std::wstring& in_name,
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
	/** Возвращает признак необходимости запроса подтверждения*/
	bool NeedAccept() const{return true;};
	/** Возвращает строку запроса подтверждения*/
	std::wstring GetAcceptQuestion() const{return L"Разметка кривой будет удалена. Продолжить?";};
};
typedef boost::shared_ptr<VDeleteActiveCurveAction> VDeleteActiveCurveActionPtr;

/** Действие - удалить всю разметку кривых*/
class VDeleteAllCurvesAction : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
	/** Тип меток*/
	PARAMETER_TYPE mark_type;
public:
	/** Конструктор*/
	VDeleteAllCurvesAction(VDataSetPtr in_data_set, const std::wstring& in_name,
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
	/** Возвращает признак необходимости запроса подтверждения*/
	bool NeedAccept() const{return true;};
	/** Возвращает строку запроса подтверждения*/
	std::wstring GetAcceptQuestion() const{return L"Разметка всех кривых будет удалена. Продолжить?";};
};
typedef boost::shared_ptr<VDeleteAllCurvesAction> VDeleteAllCurvesActionPtr;


/** Действие - удалить всю разметку прямых*/
class VDeleteAllLinesAction : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
	/** Тип меток*/
	PARAMETER_TYPE mark_type;
public:
	/** Конструктор*/
	VDeleteAllLinesAction(VDataSetPtr in_data_set, const std::wstring& in_name,
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
	/** Возвращает признак необходимости запроса подтверждения*/
	bool NeedAccept() const{return true;};
	/** Возвращает строку запроса подтверждения*/
	std::wstring GetAcceptQuestion() const{return L"Разметка прямых будет удалена. Продолжить?";};
};
typedef boost::shared_ptr<VDeleteAllLinesAction> VDeleteAllLinesActionPtr;

/** Действие - копирует фактическую разметку в паспортную*/
class VCopyCurvesAction : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;		
	/** Тип меток*/
	PARAMETER_TYPE mark_type;
public:
	/** Конструктор*/
	VCopyCurvesAction(VDataSetPtr in_data_set, const std::wstring& in_name, 
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
	/** Возвращает признак необходимости запроса подтверждения*/
	bool NeedAccept() const{return true;};
	/** Возвращает строку запроса подтверждения*/
	std::wstring GetAcceptQuestion() const{return L"Существующая разметка будет удалена. Продолжить?";};
};
typedef boost::shared_ptr<VCopyCurvesAction> VCopyCurvesActionPtr;

/** Действие - автоматическая разметка кривых*/
class VAutomarkCurvesAction : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;	
	/** Прогресс*/
	VAUIProgressDialogPtr progress_dialog;
	/** Тип меток*/
	PARAMETER_TYPE mark_type;
public:
	/** Конструктор*/
	VAutomarkCurvesAction(VDataSetPtr in_data_set,
		VAUIProgressDialogPtr in_progress_dialog, const std::wstring& in_name, 
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
	/** Возвращает признак необходимости запроса подтверждения*/
	bool NeedAccept() const{return true;};
	/** Возвращает строку запроса подтверждения*/
	std::wstring GetAcceptQuestion() const{return L"Существующая разметка будет удалена. Продолжить?";};
};
typedef boost::shared_ptr<VAutomarkCurvesAction> VAutomarkCurvesActionPtr;

/** Действие - перенос априорных кривых из БД*/
class VTransferPassportCurvesAction : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
	/** База данных*/
	VIBDatabasePtr database;
	/** Прогресс*/
	VAUIProgressDialogPtr progress_dialog;
	/** Тип меток*/
	PARAMETER_TYPE mark_type;
public:
	/** Конструктор*/
	VTransferPassportCurvesAction(VDataSetPtr in_data_set, VIBDatabasePtr in_database,
		VAUIProgressDialogPtr in_progress_dialog, const std::wstring& in_name, 
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
	/** Возвращает признак необходимости запроса подтверждения*/
	bool NeedAccept() const{return true;};
	/** Возвращает строку запроса подтверждения*/
	std::wstring GetAcceptQuestion() const{return L"Существующая разметка будет удалена. Продолжить?";};
};
typedef boost::shared_ptr<VTransferPassportCurvesAction> VTransferPassportCurvesActionPtr;


/** Действие - перенос измеренных кривых из БД*/
class VTransferMeasuredCurvesAction : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
	/** База данных*/
	VIBDatabasePtr database;
	/** Прогресс*/
	VAUIProgressDialogPtr progress_dialog;
	/** Тип меток*/
	PARAMETER_TYPE mark_type;
public:
	/** Конструктор*/
	VTransferMeasuredCurvesAction(VDataSetPtr in_data_set, VIBDatabasePtr in_database,
		VAUIProgressDialogPtr in_progress_dialog, const std::wstring& in_name, 
		PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
	/** Возвращает признак необходимости запроса подтверждения*/
	bool NeedAccept() const{return true;};
	/** Возвращает строку запроса подтверждения*/
	std::wstring GetAcceptQuestion() const{return L"Существующая разметка будет удалена. Продолжить?";};
};
typedef boost::shared_ptr<VTransferMeasuredCurvesAction> VTransferMeasuredCurvesActionPtr;

/** Действие - оптимизация кривых*/
class VOptimizeCurvesAction : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
	/** Прогресс-диалог*/
	VAUIProgressDialogPtr progress_dialog;
	/** Тип меток*/
	PARAMETER_TYPE mark_type;
public:
	/** Конструктор*/
	VOptimizeCurvesAction(VDataSetPtr in_data_set, VAUIProgressDialogPtr in_progress_dialog, 
		const std::wstring& in_name, PARAMETER_TYPE in_mark_type=PRM_CURVE_MARKS);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VOptimizeCurvesAction> VOptimizeCurvesActionPtr;

/** Действие - установить статус активной кривой*/
class VSetCurveStatusAction : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
	/** Диалог выбора*/
	VAUISelectItemDialogPtr select_dialog;
public:
	/** Конструктор*/
	VSetCurveStatusAction(VDataSetPtr data_set, const std::wstring& in_name,
		VAUISelectItemDialogPtr in_select_dialog);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VSetCurveStatusAction> VSetCurveStatusActionPtr;

/** Действие - установить комментарий активной кривой*/
class VSetCurveCommentAction : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
	/** Диалог выбора*/
	VAUISelectItemDialogPtr select_dialog;
	/** Диалог ввода*/
	VAUIInputDialogPtr input_dialog;
public:
	/** Конструктор*/
	VSetCurveCommentAction(VDataSetPtr data_set, const std::wstring& in_name,
		VAUISelectItemDialogPtr in_select_dialog, VAUIInputDialogPtr in_input_dialog);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VSetCurveCommentAction> VSetCurveCommentActionPtr;

/** Действие - ввести скорости на кривой*/
class VSetCurveSpeedsAction : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
	/** Диалог выбора*/
	VAUIPropertyDialogPtr property_dialog;
public:
	/** Конструктор*/
	VSetCurveSpeedsAction(VDataSetPtr data_set, const std::wstring& in_name,
		VAUIPropertyDialogPtr in_property_dialog);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VSetCurveSpeedsAction> VSetCurveSpeedsActionPtr;

/** Действие установить скорости на кривой по паспортным*/
class VResetCurveSpeedsAction : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
public:
	/** Конструктор*/
	VResetCurveSpeedsAction(VDataSetPtr data_set, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VResetCurveSpeedsAction> VResetCurveSpeedsActionPtr;

/** Действие  - группа действий*/
class VGroupAction : public VAction
{
private:
	/** Список действий*/
	std::vector<VActionPtr> actions;
public:
	/** Конструктор*/
	VGroupAction(const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
	/** Добавляет действие в группу*/
	void AddAction(VActionPtr in_action);
};
typedef boost::shared_ptr<VGroupAction> VGroupActionPtr;

/** Обновить данные*/
class VRefreshDataAction : public VAction
{
private:
	/** Данные*/
	VDataProviderPtr data;
public:
	/** Конструктор*/
	VRefreshDataAction(VDataProviderPtr in_data, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VRefreshDataAction> VRefreshDataActionPtr;

/** Действие - Автокоррекция по км. столбам*/
class VCorrectCoordAction : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
	VIBDatabasePtr database;
	POINT_TYPE type;
public:
	/** Конструктор*/
	VCorrectCoordAction(VDataSetPtr in_data_set, VIBDatabasePtr in_database, POINT_TYPE in_type);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VCorrectCoordAction> VCorrectCoordActionPtr;

/** Действие - Сдвиг магнитных зазоров*/
class VShiftZazorAction : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
	/** Диалог задания параметров*/
	VAUIPropertyDialogPtr property_dialog;
public:
	/** Конструктор*/
	VShiftZazorAction(VDataSetPtr in_data_set, const std::wstring& in_name, VAUIPropertyDialogPtr in_property_dialog);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VShiftZazorAction> VShiftZazorActionPtr;

/** Действие - Выделение объекта*/
class VSelectPointAction : public VAction
{
private:
	/** Источник данных с графика*/
	VDataProviderPtr data;
public:
	/** Конструктор*/
	VSelectPointAction(VDataProviderPtr in_data);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VSelectPointAction> VSelectPointActionPtr;

/// Действие - поправка нуля одного параметра под другой
class VAlignParameterMean : public VAction
{
private:
	/// Выделенная область
	VSelectionInfoPtr selection;
	/// Источник данных
	VDataSetPtr data_set;	
	/// Тип исходного параметра
	PARAMETER_TYPE data_type_source;
	/// Тип выходного параметра
	PARAMETER_TYPE data_type_target;

public:
	///Конструктор
	VAlignParameterMean(VSelectionInfoPtr in_selection, VDataSetPtr in_data_set, 
		PARAMETER_TYPE in_data_type_source, PARAMETER_TYPE in_data_type_target, 
		const std::wstring& in_name);
	/// Выполнение
	void Execute();
	/// Возвращает true, если действие доступно
	bool Allowed() const;
};
typedef boost::shared_ptr<VAlignParameterMean> VAlignParameterMeanPtr;

/** Действие - поправка нуля*/
class VShiftParameterAction : public VAction
{
private:	
	/** Указатель на систему координат*/
	VAxesWeakPtr axes;
	/** Источник данных*/
	VDataSetPtr data_set;	
	/** Тип параметра*/
	PARAMETER_TYPE data_type;
	/** Величина поправки*/
	double shift;
public:
	/** Конструктор*/
	VShiftParameterAction(VAxesWeakPtr in_axes, VDataSetPtr in_data_set, PARAMETER_TYPE in_data_type, 
		double in_shift, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VShiftParameterAction> VShiftParameterActionPtr;


/** Действие - отбраковка параметра*/
class VCorruptParameterAction : public VAction
{
private:
	/** Выделенный участок*/
	VSelectionInfoPtr selection_info;
	/** Источник данных*/
	VDataSetPtr data_set;
	/** Признак сбоя*/
	bool corrupt;
	/** Тип параметра*/
	PARAMETER_TYPE data_type;
public:
	/** Конструктор*/
	VCorruptParameterAction(VSelectionInfoPtr in_selection_info, VDataSetPtr in_data_set, bool in_corrupt, 
		PARAMETER_TYPE in_data_type, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VCorruptParameterAction> VCorruptParameterActionPtr;

/** Привязать метки на профиле к профилю*/
class VAdjustProfileMarkHeight  : public VAction
{
private:
	/** Источник данных*/
	VDataSetPtr data_set;
public:
	/** Конструктор*/
	VAdjustProfileMarkHeight(VDataSetPtr data_set, const std::wstring& in_name);
	/** Выполнение*/
	void Execute();
	/** Возвращает true, если действие доступно*/
	bool Allowed() const;
};
typedef boost::shared_ptr<VAdjustProfileMarkHeight> VAdjustProfileMarkHeightPtr;