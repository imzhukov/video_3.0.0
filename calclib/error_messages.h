#pragma once

/// Сообщения об ошибках
extern const wchar_t * ERRMSG_OK;					/// Успех!!!
extern const wchar_t * ERRMSG_DATA_NOT_LOADED;		/// Загрузка не производилась
extern const wchar_t * ERRMSG_BLOCK_NOT_FOUND;		/// Отсутствует блок данных в файле
extern const wchar_t * ERRMSG_BLOCK_ACCESS_FAIL;	/// Блок данных в файле недоступен
extern const wchar_t * ERRMSG_BLOCK_INCORRECT;		/// Некорректный блок данных
extern const wchar_t * ERRMSG_BLOCK_CREATION_FAIL;	/// Не удалось создать блок данных
extern const wchar_t * ERRMSG_VERSION_INCORRECT;	/// Некорректная версия записи
extern const wchar_t * ERRMSG_RECORD_INCORRECT;		/// Некорректная запись
extern const wchar_t * ERRMSG_DATA_INCOMPLETE;		/// Неожиданный конец блока
extern const wchar_t * ERRMSG_BLOCK_CREATION_FAILED;/// Не удалось создать блок
extern const wchar_t * ERRMSG_BLOCK_WRITE_FAILED;	/// Не удалось записать блок в файл
extern const wchar_t * ERRMSG_DB_NO_CONNECTION;		/// Отсутствует соединение с БД
extern const wchar_t * ERRMSG_FILE_NOT_OPEN;		/// Файл данных не загружен
extern const wchar_t * ERRMSG_MISSING_PARAMETER;	/// Необходимый параметр не загружен
extern const wchar_t * ERRMSG_MEMORY_ALLOC_ERROR;	/// Ошибка при выделении памяти
extern const wchar_t * ERRMSG_DATA_CREATION_ERROR;	/// Ошибка при заполнении блока
extern const wchar_t * ERRMSG_APRINFO_HOLE_IN_SPD;	/// Пропуски в скоростях по приказу
extern const wchar_t * ERRMSG_APRINFO_NO_RAIL_INFO;	/// Отсутствует информация о рельсах/стыках
extern const wchar_t * ERRMSG_WRONG_OPTION;         /// Неизвестное значение в настройках