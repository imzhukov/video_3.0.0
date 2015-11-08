#pragma once
#include <map>
#include <string>
#include <vector>

#include "dblib.h"

// генератор sql-запросов 'insert'
class VSQLGenerator
{
	// имя таблицы
	std::string table;			

	// имена параметров
	std::vector<std::string> prm_names;

	// параметры
	std::vector<VIBParameter> prm_bodies;

	// прямые вставки в код
	std::map<std::string, std::string> insql;	

	// проверка наличия параметра в списке (prm или insql)
	bool NameExists(const std::string & name) const;

public:

	// инициализация для заполнения таблицы t
	void Init(const std::string & t);

	// добавление готового параметра (blob)
	bool AddParameter(const std::string & name, const VIBParameter & prm);

	// добавление целочисленного значения
	bool AddInteger(const std::string & name, const __int64 & value);

	// добавление значения с плавающей точкой
	bool AddFloat(const std::string & name, const double & value);

	// добавление строкового значения (char)
	bool AddString(const std::string & name, const std::string & text);

	// добавление строкового значения (wide-char)
	bool AddString(const std::string & name, const std::wstring & text);

	// добавление таймштампа
	bool AddTimestamp(const std::string & name, const VTime & value);

	// вставка целикового SQL-подзапроса
	bool AddSQL(const std::string & name, const std::string & text);

	// готовый оператор 'insert into ...'
	std::string GetInsertSQL() const;

	// готовый оператор 'update ... set ...'
	std::string GetUpdateSQL(const std::string & condition) const;

	// набор параметров
	const std::vector<VIBParameter> & GetParameters() const;
};
