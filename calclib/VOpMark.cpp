#include "VOpMark.h"

#define	OMB_IGNORE			0x00000001	// Игнорировать
#define	OMB_OTHER			0x00000002	// Прочее
#define	OMB_BRIDGE			0x00000004	// Мост
#define	OMB_RADAR			0x00000008	// Радар
#define	OMB_STATION			0x00000010	// Раздельный пункт
#define	OMB_SWITCH			0x00000020	// Стрелка
#define	OMB_POLE			0x00000040	// Столб
#define	OMB_PICKET			0x00000080	// Пикет
#define	OMB_SW_LEFT			0x00000100	// Стрелка левая (опт.) - остряк
#define	OMB_SW_RIGHT		0x00000200	// Стрелка правая (опт.) - остряк
#define OMB_X_LEFT          0x00000400  // Стрелка левая (опт.) - крестовина
#define OMB_X_RIGHT         0x00000800  // Стрелка правая (опт.) - крестовина

// ---------------------------------------------------------------------------
/// Магнитные зазаоры и метки оператора
// ---------------------------------------------------------------------------

bool VOpMark::Import(const VFileIndexRecord & src)
{
	this->index = src.index;
	this->isIgnore =     (src.flags & OMB_IGNORE) == OMB_IGNORE;
	this->isBridge =     (src.flags & OMB_BRIDGE) == OMB_BRIDGE;
	this->isRadar =      (src.flags & OMB_RADAR) == OMB_RADAR;
	this->isStation =    (src.flags & OMB_STATION) == OMB_STATION;
	this->isSwitch =     (src.flags & OMB_SWITCH) == OMB_SWITCH;
	this->isPole =       (src.flags & OMB_POLE) == OMB_POLE;
	this->isPicket =     (src.flags & OMB_PICKET) == OMB_PICKET;
	this->isOptSwLeft =  (src.flags & OMB_SW_LEFT) == OMB_SW_LEFT;
	this->isOptSwRight = (src.flags & OMB_SW_RIGHT) == OMB_SW_RIGHT;
	this->isOptXLeft =   (src.flags & OMB_X_LEFT) == OMB_X_LEFT;
	this->isOptXRight =  (src.flags & OMB_X_RIGHT) == OMB_X_RIGHT;
	return true;
}

bool VOpMark::Import(uint8_t kms, uint8_t kms2, size_t index)
{
	this->index = index;
	this->isBridge = (kms & 0x04) == 0x04;
	this->isPole = (kms & 0x40) == 0x40;
	this->isStation = (kms & 0x10) == 0x10;
	this->isSwitch = (kms & 0x20) == 0x20;
	this->isRadar = (kms & 0x08) == 0x08;
	this->isPicket = (kms & 0x80) == 0x80;
	this->isOptSwLeft = (kms & 0x01) == 0x01;
	this->isOptSwRight = (kms & 0x02) == 0x02;
	this->isOptXLeft = (kms2 & 0x01) == 0x01;
	this->isOptXRight = (kms2 & 0x02) == 0x02;
	
	return this->isBridge || this->isPole || this->isStation || 
		this->isSwitch || this->isRadar || this->isPicket || 
		this->isOptSwLeft || this->isOptSwRight || this->isOptXLeft || this->isOptXRight;
}

// импорт из слитых флагов в файле INM
bool VOpMark::Import(uint16_t kms, size_t index)
{
	this->index = index;
	this->isBridge = (kms & 0x0004) == 0x0004;
	this->isPole = (kms & 0x0040) == 0x0040;
	this->isStation = (kms & 0x0010) == 0x0010;
	this->isSwitch = (kms & 0x0020) == 0x0020;
	this->isRadar = (kms & 0x0008) == 0x0008;
	this->isPicket = (kms & 0x0080) == 0x0080;
	this->isOptSwLeft = (kms & 0x0001) == 0x0001;
	this->isOptSwRight = (kms & 0x0002) == 0x0002;
	this->isOptXLeft = (kms & 0x0100) == 0x0100;
	this->isOptXRight = (kms & 0x0200) == 0x0200;
	
	return this->isBridge || this->isPole || this->isStation || 
		this->isSwitch || this->isRadar || this->isPicket || 
		this->isOptSwLeft || this->isOptSwRight || this->isOptXLeft || this->isOptXRight;
}

uint16_t VOpMark::ExportToInm() const
{
	uint16_t kms = 0;
	if (this->isBridge)		kms |= 0x0004;
	if (this->isPole)		kms |= 0x0040;
	if (this->isStation)	kms |= 0x0010;
	if (this->isSwitch)		kms |= 0x0020;
	if (this->isRadar)		kms |= 0x0008;
	if (this->isPicket)		kms |= 0x0080;
	if (this->isOptSwLeft)	kms |= 0x0001;
	if (this->isOptSwRight)	kms |= 0x0002;
	if (this->isOptXLeft)	kms |= 0x0100;
	if (this->isOptXRight)	kms |= 0x0200;
	return kms;
}

VFileIndexRecord VOpMark::Export() const
{
	VFileIndexRecord record;
	record.index = this->index;
	record.flags = 0;
	if (this->isIgnore)     record.flags |= OMB_IGNORE;
	if (this->isBridge)     record.flags |= OMB_BRIDGE;
	if (this->isRadar)      record.flags |= OMB_RADAR;
	if (this->isStation)    record.flags |= OMB_STATION;
	if (this->isSwitch)     record.flags |= OMB_SWITCH;
	if (this->isPole)       record.flags |= OMB_POLE;
	if (this->isPicket)     record.flags |= OMB_PICKET;
	if (this->isOptSwLeft)  record.flags |= OMB_SW_LEFT;
	if (this->isOptSwRight) record.flags |= OMB_SW_RIGHT;
	if (this->isOptXLeft)   record.flags |= OMB_X_LEFT;
	if (this->isOptXRight)  record.flags |= OMB_X_RIGHT;
	return record;
}

#define OMB_MAGZ_LEFT		0x00000001	// Левый магнитный зазор
#define OMB_MAGZ_RIGHT		0x00000002	// Правый магнитный зазор

bool VMagZazor::Import(const VFileIndexRecord & src)
{
	this->index = src.index;
	this->isLeft = ((src.flags & OMB_MAGZ_LEFT) == OMB_MAGZ_LEFT);
	this->isRight = ((src.flags & OMB_MAGZ_RIGHT) == OMB_MAGZ_RIGHT);
	return true;
}

bool VMagZazor::Import(uint8_t kms2, size_t index)
{
	this->index = index;
	this->isLeft = ((kms2 & 0x40) == 0x40);
	this->isRight = ((kms2 & 0x20) == 0x20);
	return this->isLeft || this->isRight;
}

// импорт из слитых флагов в файле INM
bool VMagZazor::Import(uint16_t kms, size_t index)
{
	this->index = index;
	this->isLeft = ((kms & 0x4000) == 0x4000);
	this->isRight = ((kms & 0x2000) == 0x2000);
	return this->isLeft || this->isRight;
}

VFileIndexRecord VMagZazor::Export() const
{
	VFileIndexRecord record;
	record.index = this->index;
	record.flags = 0;
	if (this->isLeft)  record.flags |= OMB_MAGZ_LEFT;
	if (this->isRight) record.flags |= OMB_MAGZ_RIGHT;
	return record;
}

uint16_t VMagZazor::ExportToInm() const
{
	uint16_t kms = 0;
	if (this->isLeft)	kms |= 0x4000;
	if (this->isRight)	kms |= 0x2000;
	return kms;
}

bool VUgon::Import(const VFileIndexRecord & src)
{
	this->index = src.index;
	this->value = src.value;
	return true;
}

VFileIndexRecord VUgon::Export() const
{
	VFileIndexRecord record;
	record.index = this->index;
	record.value = this->value;
	return record;
}

bool VSpalaMarker::Import(const VFileIndexRecord & src)
{
	this->index = src.index;
	this->value = src.flags;
	return true;
}

VFileIndexRecord VSpalaMarker::Export() const
{
	VFileIndexRecord record;
	record.index = this->index;
	record.flags = this->value;
	return record;
}

bool VSpalaMarkerList::Shift(float value, VAbsCoordData & abscoord)
{
	bool mark = true;
	for (size_t i=0; i < this->GetCount(); i++)
	{
		uint32_t index_new = abscoord.FindIndex((abscoord.GetItem(this->GetItem(i).index) + value),true);
		if(!index_new)
			mark = false;
		else
			this->GetItem(i).index = index_new;
	}
	return mark;
}