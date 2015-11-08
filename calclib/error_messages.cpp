#include "tchar.h"

/// Сообщения об ошибках
extern const wchar_t * ERRMSG_OK = L"";
extern const wchar_t * ERRMSG_DATA_NOT_LOADED = L"Загрузка не производилась";
extern const wchar_t * ERRMSG_BLOCK_NOT_FOUND = L"Отсутствует блок данных в файле";
extern const wchar_t * ERRMSG_BLOCK_ACCESS_FAIL = L"Блок данных в файле недоступен";
extern const wchar_t * ERRMSG_BLOCK_INCORRECT = L"Некорректный блок данных";
extern const wchar_t * ERRMSG_BLOCK_CREATION_FAIL = L"Не удалось создать блок данных";
extern const wchar_t * ERRMSG_VERSION_INCORRECT = L"Некорректная версия или размер записи";
extern const wchar_t * ERRMSG_RECORD_INCORRECT = L"Некорректная запись в файле";
extern const wchar_t * ERRMSG_DATA_INCOMPLETE = L"Неожиданный конец блока";
extern const wchar_t * ERRMSG_BLOCK_CREATION_FAILED = L"Не удалось создать блок";
extern const wchar_t * ERRMSG_BLOCK_WRITE_FAILED = L"Не удалось записать блок в файл";
extern const wchar_t * ERRMSG_DB_NO_CONNECTION = L"Отсутствует соединение с БД";
extern const wchar_t * ERRMSG_FILE_NOT_OPEN = L"Файл данных не загружен";
extern const wchar_t * ERRMSG_MISSING_PARAMETER = L"Необходимый параметр не загружен";
extern const wchar_t * ERRMSG_MEMORY_ALLOC_ERROR = L"Ошибка при выделении памяти";
extern const wchar_t * ERRMSG_DATA_CREATION_ERROR = L"Ошибка при заполнении блока";
extern const wchar_t * ERRMSG_APRINFO_HOLE_IN_SPD = L"Пропуски в скоростях по приказу";
extern const wchar_t * ERRMSG_APRINFO_NO_RAIL_INFO = L"Отсутствует информация о рельсах/стыках";
extern const wchar_t * ERRMSG_WRONG_OPTION = L"Неизвестное значение в настройках";