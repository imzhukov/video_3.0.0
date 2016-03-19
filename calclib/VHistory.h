/*
Файл VHistory.h
История операций произведенных над файлом
*/

#pragma once

#include <stdlib.h>
#include <vector>
#include <algorithm>
#include "types.h"
#include "c4flib.h"
#include "VParameter.h"

using namespace std;

// Коды операций

/** Создание файла при поездке в RT*/
#define OC_CREATION 0xFFFFFFFF
/** Неопределенная операция*/
#define OC_UNDEFINED 0L
/** Автоматический пересчет координат*/
#define OC_AUTOCORRECTION 1L
/** Ручной пересчет координат*/
#define OC_COORDCORRECTION 2L
/** Замена априорной информации*/
#define OC_CHANGEAPRINFO 3L
/** Свиг ШК*/
#define OC_SHIFTSHABLON 4L
/** Свиг уровня*/
#define OC_SHIFTLEVEL 5L
/** Свиг уклона*/
#define OC_SHIFTGRADIENT 6L
/** Свиг левого износа*/
#define OC_SHIFTIZNOSLEFT 7L
/** Свиг правого износа*/
#define OC_SHIFTIZNOSRIGHT 8L
/** Полная разметка кривых*/
#define OC_FULLMARKCURVES 9L
/** Изменение разметки кривых*/
#define OC_MARKCURVES 10L
/** Изменение разметки профиля*/
#define OC_MARKPROFILE 11L
/** Изменение разметки прямых*/
#define OC_MARKPLAN 12L
/** Коррекция сбоев износа*/
#define OC_CORRECTIZNOS 13L
/** Сдвиг левой рихтовки*/
#define OC_SHIFTRICHL 14L
/** Сдвиг правой рихтовки*/
#define OC_SHIFTRICHR 14L
/** Сдвиг ШК спереди*/
#define OC_SHIFTWIDTHFRONT 15L
/** Сдвиг ШК сзади*/
#define OC_SHIFTWIDTHBACK 16L
/** Коррекция координат по эталонному файлу*/
#define OC_CORRECTIONBYFILE 17L
/** Коррекция координат по кривым из БД*/
#define OC_CORRECTIONBYCURVES 18L
/** Замена даты поездки*/
#define OC_DATECORRECTION 19L
/** Замена номера вагона*/
#define OC_DIVISIONCORRECTION 20L
/** Замена номера дороги*/
#define OC_ROADNUMCORRECTION 21L
/** Коррекция курса*/
#define OC_CORRECTCOURSE 22L
/** Пересчет ср. кривизны на новой базе*/
#define OC_CALC_AVG_CRV 23L
/** Пересчет ср. уровня на новой базе*/
#define OC_CALC_AVG_LVL 24L
/** Фазировка опт. параметров*/
#define OC_OPTICS_SYNCH 25L
/** Отбраковка опт. параметров*/
#define OC_OPTICS_CHKERRORS 26L
/** Бэкап курса перед подвижкой*/
#define OC_KURS_BACKUP 27L
/** Подвижка курса*/
#define OC_KURS_SHIFT 28L
/** Восстановление оригинального курса*/
#define OC_KURS_SHIFT_UNDO 29L
/** Моделирование кривых*/
#define OC_FULLMODELCURVES 30L
/** Пересчет ШК с подменой значений*/
#define OC_WIDTH_FALSIFICATION 31L
/** Сдвиг курса*/
#define OC_SHIFTCOURSE 34L
/** Сдвиг ширины колеи (ППР)*/
#define OC_SHIFT_SHABLON_PPR 35L
/** Сдвиг левого наклона (ППР)*/
#define OC_SHIFT_NAKLON_LEFT_PPR 36L
/** Сдвиг правого наклона (ППР)*/
#define OC_SHIFT_NAKLON_RIGHT_PPR 37L
/** Сдвиг левой подуклонки*/
#define OC_SHIFT_INCLINE_LEFT_PPR 38L
/** Сдвиг правой подуклонки*/
#define OC_SHIFT_INCLINE_RIGHT_PPR 39L
/** Сдвиг левого износа (ППР)*/
#define OC_SHIFT_IZNOS_LEFT_PPR 40L
/** Сдвиг правого износа (ППР)*/
#define OC_SHIFT_IZNOS_RIGHT_PPR 41L
/** Перешифровка отступлений*/
#define OC_RECALC 42L

class VHistory;

/** Запись об операции*/
class VOperationRecord
{
friend VHistory;
private:
	/** Дата и время записи*/
	VTime operation_time;
	/** Код операции*/
	uint32_t operation_code;
	/** Размер текстового описания*/
	uint32_t text_length;
	/** Текстовое описание*/
	char * text_description;
	/** Размер блока произвольных данных*/
	uint32_t data_size;
	/** Блок произвольных данных, определяемых типом операции*/
	void * data;
public:	
	/** Конструктор*/
	VOperationRecord();
	/** Копирующий конструктор*/
	VOperationRecord(const VOperationRecord &source);
	/** Деструктор*/
	~VOperationRecord();

	//Методы

	/** Возвращает дату записи*/
	const VTime& GetDateTime() const;	
	/** Возвращает общий размер данных*/
	size_t GetTotalSize() const;
	/** Сохраняет в блоке памяти*/
	bool SaveToMemory(void * lpData);
	/** Загружает из блока памяти*/
	bool LoadFromMemory(void * lpData);
	/** Возвращает код операции*/
	unsigned int GetCode() const;
	/** Устанавливает код операции*/
	void SetCode(unsigned int uiCode);
	/** Возвращает длину текстового описания*/
	uint32_t GetTextLength() const;
	/** Возвращает указатель на текстовое описание*/
	const char * GetTextDescription() const;
	/** Устанавливает текстовое описание*/
	void SetTextDescription(const std::string & sTextDescription);
	/** Устанавливает текстовое описание*/
	void SetTextDescription(const char * cTextDescription, unsigned int uiTextLength);
	/** Возвращает размер произвольных данных*/
	size_t GetDataSize() const;
	/** Возвращает указатель на блок данных*/
	const void * GetData() const;
	/** Записывает произвольные данные*/
	void SetData(void * lpData, unsigned int uiDataSize);
	/** Оператор присваивания*/
	VOperationRecord& operator=(const VOperationRecord &source);	
};


/** Список записей о проведенных операциях*/
class VHistory : public VParameterProto<VOperationRecord>
{
private:

	//Данные

	/** Массив записей*/
	vector<VOperationRecord> m_Operations;

public:

	/** Конструктор*/
	VHistory();

	/** Деструктор*/
	~VHistory();	

	/** Загрузка данных из открытого файла*/
	bool Load(VMainFile & file);	
	bool Load(VRemoteFile & file);	

	/** Сохранение данных в открытом файле*/
	bool Save(VMainFile & file);	

	/** Возвращает количество элементов*/
	size_t GetCount() const;

	/** Возвращает размер всех записей*/
	size_t GetTotalSize() const;

	/** Удаляет элемент*/
	bool RemoveAt(size_t index);

	/** Возвращает указатель на элемент*/
	const VOperationRecord  & GetItem(size_t index) const;

	/** Возвращает указатель на элемент*/
	VOperationRecord  & GetItem(size_t index);

	/** Добавляет элемент*/
	void Add(const VOperationRecord &operationRecord);

	/** Проверяет есть ли хоть одна запись об определенной операции*/
	bool OperationExists(unsigned int uiCode);

	/** Выгрузить записи*/
	void Unload()
	{
		this->m_Operations.clear();
		VParameterProto<VOperationRecord>::Unload();
	}

	/** Извлечь версию программы создавшей файл*/
	std::string ExtractCreatorVersion() const;
};
