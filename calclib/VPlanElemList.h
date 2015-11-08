#pragma once

#include <vector>
#include <string>
#include "os.h"
#include "types.h"
#include "VParameter.h"

class VDataSet;
class VAprinfo;
class VCurveList;
class VLineMarkList;
class VSidewayList;
class VAbsCoordData;
struct VWaySection;

/// Типы элементов плана
enum PLAN_ELEM_TYPE
{
	PLAN_ELEM_LINE,			/// прямая
	PLAN_ELEM_HALFLINE,		/// неполная прямая
	PLAN_ELEM_CURVE,		/// кривая
	PLAN_ELEM_HALFCURVE,	/// неполная кривая
	PLAN_ELEM_SIDEWAY,		/// боковой путь
	PLAN_ELEM_RADIUS,		/// круговая кривая
	PLAN_ELEM_INTERMEDIATE	/// переходная кривая
};

///Возвращает название типа элемента
std::wstring GetPlanElementTypeName(PLAN_ELEM_TYPE in_type);

/// Элемент плана
struct VPlanElement
{
	PLAN_ELEM_TYPE type;
	VWaySection section;
	double absStart, absEnd;
	uint32_t idxStart, idxEnd;

	//Индекс кривой (FixMe!!! эксперимент)
	size_t curve_index;
	/// Заполняем абсолютную и путевую координаты по индексу
	void SetCoords(const VAbsCoordData & absCoord, const VAprinfo & aprInfo);
	/// Возвращает true если точка находится на элементе
	bool HasPoint(double in_abscoord) const;
	/// Возвращает длину элемента
	double GetLength() const;
};

/// Список элементов плана
class VPlanElemList : public VIndexList<VPlanElement>
{
	
	// добавить элементы прямой
	void AddLineElemList(size_t idxFrom, size_t idxTo, bool detailed_line,
		const VLineMarkList & linemarks, const VAprinfo & aprInfo, 
		const VAbsCoordData & absCoord);

public:

	/// Загрузка из DataSet
	bool Load(VDataSet & ds, bool model, bool detailed_curve, bool detailed_line);

	/// Загрузка из отдельных элементов
	bool Load(const VWaySection & sumsection,
		const VCurveList & curves, const VLineMarkList & linemarks, const VSidewayList & sideWays, 
		const VAprinfo & aprInfo, const VAbsCoordData & absCoord,
		bool detailed_curve, bool detailed_line);

	/// Находит элемент на котором находится заданная точка
	size_t Find(double in_abscoord) const;
	/// Возвращает список элементов на которых находится точка с указанным индексом
	std::vector<size_t> GetAffectedElements(size_t in_index) const;
};
