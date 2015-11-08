#include "VCorrupt.h"

const uint32_t
ERROR_NONE				= 0x00000000, // нет ошибки
ERROR_ABS_COORD			= 0x00000001, // аваpия ДПП
ERROR_WIDTH				= 0x00000002, // аваpия шаблона
ERROR_LEVEL				= 0x00000004, // аваpия уровня
ERROR_DIST_SHORT		= 0x00000008, // аваpия перекоса короткого
ERROR_DIST_LONG			= 0x00000010, // аваpия перекоса коpоткого
ERROR_GRADIENT			= 0x00000020, // аваpия уклона
ERROR_PIT_LEFT			= 0x00000040, // аваpия просадки короткой левой
ERROR_PIT_RIGHT			= 0x00000080, // аваpия просадки короткой правая		
ERROR_WIDTH_BACK		= 0x00000100, // авария "шк сзади"
ERROR_HEIGHT			= 0x00000200, // аваpия высоты
ERROR_RICH_LEFT			= 0x00000400, // аваpия рихтовки короткой левой
ERROR_RICH_RIGHT		= 0x00000800, // аваpия рихтовки короткой правой
ERROR_CURV				= 0x00001000, // аваpия кривизны
ERROR_KURS				= 0x00002000, // аваpия курса		
ERROR_WIDTH_FRONT		= 0x00004000, // авария "шк спереди"	
ERROR_WEAR				= 0x00010000, // авария объединенного износа (от ПЭВМ-1)
ERROR_WEAR_LEFT			= 0x00020000, // авария износа левого
ERROR_WEAR_RIGHT		= 0x00040000, // авария износа правого
ERROR_PPR				= 0x00080000, // авария полного профиля
ERROR_ZAZOR_LEFT		= 0x00100000, // авария левого зазора
ERROR_ZAZOR_RIGHT		= 0x00200000, // авария правого зазора
ERROR_PPR_NEW           = 0x00400000, // авария полного профиля "новые датчики"
ERROR_MAG_ZAZOR_LEFT	= 0x40000000, // авария левого магнитного зазора
ERROR_MAG_ZAZOR_RIGHT	= 0x80000000; // авария правого магнитного зазора

void VParameterCorrupt::Shift(int count)
{
	int i = 0;
	if (count > 0)
	{
		std::copy_backward(this->data.begin(), this->data.end() - count, this->data.end());
		std::fill(this->data.begin(), this->data.begin() + count, true);
	}
	else if (count < 0)
	{
		std::copy(this->data.begin() - count, this->data.end(), this->data.begin());
		std::fill(this->data.end() + count, this->data.end(), true);
	}
}

// Конструктор
VCorruptData::VCorruptData() : isLoaded(false), message(ERRMSG_DATA_NOT_LOADED), absCoord(new VParameterCorrupt), 
width(new VParameterCorrupt), widthFront(new VParameterCorrupt), widthBack(new VParameterCorrupt), 
level(new VParameterCorrupt), distShort(new VParameterCorrupt), distLong(new VParameterCorrupt), 
grad(new VParameterCorrupt), pitLeft(new VParameterCorrupt), pitRight(new VParameterCorrupt), 
richLeft(new VParameterCorrupt), richRight(new VParameterCorrupt), 
curv(new VParameterCorrupt), kurs(new VParameterCorrupt), 
wearSensor(new VParameterCorrupt), wearLeft(new VParameterCorrupt), wearRight(new VParameterCorrupt), 
ppr(new VParameterCorrupt), pprNew(new VParameterCorrupt), 
zazLeft(new VParameterCorrupt), zazRight(new VParameterCorrupt),
mzLeft(new VParameterCorrupt), mzRight(new VParameterCorrupt)
{}

// Выгрузка данных
void VCorruptData::Unload()
{
	this->corrupt.Unload();
	this->absCoord->Unload();
	this->width->Unload();
	this->widthFront->Unload();
	this->widthBack->Unload();
	this->level->Unload();
	this->distShort->Unload();
	this->distLong->Unload();
	this->grad->Unload();
	this->pitLeft->Unload();
	this->pitRight->Unload();
	this->richLeft->Unload();
	this->richRight->Unload();
	this->curv->Unload();
	this->kurs->Unload();
	this->wearSensor->Unload();
	this->wearLeft->Unload();
	this->wearRight->Unload();
	this->ppr->Unload();
	this->pprNew->Unload();
	this->zazLeft->Unload();
	this->zazRight->Unload();
	this->mzLeft->Unload();
	this->mzRight->Unload();
	this->SetLoaded(false);
	this->message = ERRMSG_DATA_NOT_LOADED;
}

// создание блоков
void VCorruptData::CreateBlocks(size_t kadrCount)
{
	this->absCoord->Create(kadrCount);
	this->width->Create(kadrCount);
	this->widthFront->Create(kadrCount);
	this->widthBack->Create(kadrCount);
	this->level->Create(kadrCount);
	this->distShort->Create(kadrCount);
	this->distLong->Create(kadrCount);
	this->grad->Create(kadrCount);
	this->pitLeft->Create(kadrCount);
	this->pitRight->Create(kadrCount);
	this->richLeft->Create(kadrCount);
	this->richRight->Create(kadrCount);
	this->curv->Create(kadrCount);
	this->kurs->Create(kadrCount);
	this->wearSensor->Create(kadrCount);
	this->wearLeft->Create(kadrCount);
	this->wearRight->Create(kadrCount);
	this->ppr->Create(kadrCount);
	this->pprNew->Create(kadrCount);
	this->zazLeft->Create(kadrCount);
	this->zazRight->Create(kadrCount);
	this->mzLeft->Create(kadrCount);
	this->mzRight->Create(kadrCount);
}

// Создать
void VCorruptData::Create(uint32_t kadrCount, bool manualBlock)
{
	this->corrupt.Create(DHT_CORRUPT, VCT_ULONG, kadrCount, sizeof(uint32_t), 0);
	for (size_t i = 0; i < this->corrupt.GetRecCount(); ++i)
	{
		this->corrupt.GetItem<uint32_t>(i) = 0;
	}
	
	if (manualBlock)
	{
		CreateBlocks(kadrCount);
	}

	this->SetLoaded(true);
	this->message = ERRMSG_OK;
}

// Загрузка из шаблонного файла
template <class FILETYPE> bool VCorruptData::Load(FILETYPE & file)
{
	if (this->IsLoaded()) return true;

	this->Unload();

	if (!this->corrupt.Load(file, DHT_CORRUPT, true))
	{
		this->message = this->corrupt.GetErrorMessage();
		return false;
	}

	VBasicParameter hand_corrupt;
	hand_corrupt.Load(file, DHT_HAND_CORRUPT, false);

	VBasicParameter * block; 
	VBasicParameter * wear_block;

	// всё из основной отбраковки
	if (!hand_corrupt.IsLoaded())
	{
		block = &this->corrupt;
		wear_block = &this->corrupt;
	}

	// основное из основной отбраковки, л+п износы из ручной
	else if (hand_corrupt.GetVersion() == 0)
	{
		block = &this->corrupt;
		wear_block = &hand_corrupt;
	}

	// всё из ручной отбраковки
	else // version == 1
	{
		block = &hand_corrupt;
		wear_block = &hand_corrupt;
	}

	size_t kadrCount = kadrCount = block->GetRecCount();

	// инициализация блоков
	this->CreateBlocks(kadrCount);

	// заполнение блоков данными
	uint32_t value;

	if (block->GetRecSize() == 2)
	{
		for (size_t i = 0; i < kadrCount; ++i)
		{
			value = block->GetItem<uint16_t>(i);
			if (value > 0)
			{
				this->absCoord->SetCorrupt(i, (value & ERROR_ABS_COORD) == ERROR_ABS_COORD);
				this->width->SetCorrupt(i, (value & ERROR_WIDTH) == ERROR_WIDTH);
				this->widthFront->SetCorrupt(i, (value & ERROR_WIDTH_FRONT) == ERROR_WIDTH_FRONT);
				this->widthBack->SetCorrupt(i, (value & ERROR_WIDTH_BACK) == ERROR_WIDTH_BACK);
				this->level->SetCorrupt(i, (value & ERROR_LEVEL) == ERROR_LEVEL);
				this->distShort->SetCorrupt(i, (value & ERROR_DIST_SHORT) == ERROR_DIST_SHORT);
				this->distLong->SetCorrupt(i, (value & ERROR_DIST_LONG) == ERROR_DIST_LONG);
				this->grad->SetCorrupt(i, (value & ERROR_GRADIENT) == ERROR_GRADIENT);
				this->pitLeft->SetCorrupt(i, (value & ERROR_PIT_LEFT) == ERROR_PIT_LEFT);
				this->pitRight->SetCorrupt(i, (value & ERROR_PIT_RIGHT) == ERROR_PIT_RIGHT);
				this->richLeft->SetCorrupt(i, (value & ERROR_RICH_LEFT) == ERROR_RICH_LEFT);
				this->richRight->SetCorrupt(i, (value & ERROR_RICH_RIGHT) == ERROR_RICH_RIGHT);
				this->curv->SetCorrupt(i, (value & ERROR_CURV) == ERROR_CURV);
				this->kurs->SetCorrupt(i, (value & ERROR_KURS) == ERROR_KURS);
			}
		}
	}
	else if (block->GetRecSize() == 4)
	{
		for (size_t i = 0; i < kadrCount; ++i)
		{
			value = block->GetItem<uint32_t>(i);
			if (value > 0)
			{
				this->absCoord->SetCorrupt(i, (value & ERROR_ABS_COORD) == ERROR_ABS_COORD);
				this->width->SetCorrupt(i, (value & ERROR_WIDTH) == ERROR_WIDTH);
				this->widthFront->SetCorrupt(i, (value & ERROR_WIDTH_FRONT) == ERROR_WIDTH_FRONT);
				this->widthBack->SetCorrupt(i, (value & ERROR_WIDTH_BACK) == ERROR_WIDTH_BACK);
				this->level->SetCorrupt(i, (value & ERROR_LEVEL) == ERROR_LEVEL);
				this->distShort->SetCorrupt(i, (value & ERROR_DIST_SHORT) == ERROR_DIST_SHORT);
				this->distLong->SetCorrupt(i, (value & ERROR_DIST_LONG) == ERROR_DIST_LONG);
				this->grad->SetCorrupt(i, (value & ERROR_GRADIENT) == ERROR_GRADIENT);
				this->pitLeft->SetCorrupt(i, (value & ERROR_PIT_LEFT) == ERROR_PIT_LEFT);
				this->pitRight->SetCorrupt(i, (value & ERROR_PIT_RIGHT) == ERROR_PIT_RIGHT);
				this->richLeft->SetCorrupt(i, (value & ERROR_RICH_LEFT) == ERROR_RICH_LEFT);
				this->richRight->SetCorrupt(i, (value & ERROR_RICH_RIGHT) == ERROR_RICH_RIGHT);
				this->curv->SetCorrupt(i, (value & ERROR_CURV) == ERROR_CURV);
				this->kurs->SetCorrupt(i, (value & ERROR_KURS) == ERROR_KURS);
				this->wearSensor->SetCorrupt(i, (value & ERROR_WEAR) == ERROR_WEAR);
				this->ppr->SetCorrupt(i, (value & ERROR_PPR) == ERROR_PPR);
				this->pprNew->SetCorrupt(i, (value & ERROR_PPR_NEW) == ERROR_PPR_NEW);
			}

			value = wear_block->GetItem<uint32_t>(i);
			if (value > 0)
			{
				this->wearLeft->SetCorrupt(i, (value & ERROR_WEAR_LEFT) == ERROR_WEAR_LEFT);
				this->wearRight->SetCorrupt(i, (value & ERROR_WEAR_RIGHT) == ERROR_WEAR_RIGHT);
				this->zazLeft->SetCorrupt(i, (value & ERROR_ZAZOR_LEFT) == ERROR_ZAZOR_LEFT);
				this->zazRight->SetCorrupt(i, (value & ERROR_ZAZOR_RIGHT) == ERROR_ZAZOR_RIGHT);
				this->mzLeft->SetCorrupt(i, (value & ERROR_MAG_ZAZOR_LEFT) == ERROR_MAG_ZAZOR_LEFT);
				this->mzRight->SetCorrupt(i, (value & ERROR_MAG_ZAZOR_RIGHT) == ERROR_MAG_ZAZOR_RIGHT);
			}
		}
	}
	else
	{
		this->message = ERRMSG_BLOCK_INCORRECT;
		return false;
	}

	this->SetLoaded(true);
	this->message = ERRMSG_OK;

	return true;
}

template bool VCorruptData::Load(VMainFile & file);
template bool VCorruptData::Load(VRemoteFile & file);

// Вернуть канал отбраковок
VParameterCorruptPtr VCorruptData::GetCorruptChannel(PARAMETER_TYPE param) const
{
	switch(param)
	{
	case PRM_ABS_COORD:			return this->absCoord;
	case PRM_WIDTH:				return this->width;
	case PRM_WIDTH_FRONT:		return this->widthFront;
	case PRM_WIDTH_BACK:		return this->widthBack;
	case PRM_DIST_SHORT:		return this->distShort;
	case PRM_DIST_LONG:			return this->distLong;
	case PRM_PIT_LEFT:			return this->pitLeft;
	case PRM_PIT_RIGHT:			return this->pitRight;
	case PRM_RICH_LEFT:			return this->richLeft;
	case PRM_RICH_RIGHT:		return this->richRight;
	case PRM_GRADIENT:			return this->grad;
	case PRM_LEVEL:				return this->level;
	case PRM_CURV:				return this->curv;
	case PRM_KURS:				return this->kurs;
	case PRM_UNI_WEAR_SENSOR:	return this->wearSensor;
	case PRM_WEAR_LEFT:			return this->wearLeft;
	case PRM_WEAR_RIGHT:		return this->wearRight;
	case PRM_ZAZOR_LEFT:		return this->zazLeft;
	case PRM_ZAZOR_RIGHT:		return this->zazRight;
	case PRM_MAG_ZAZOR_LEFT:	return this->mzLeft;
	case PRM_MAG_ZAZOR_RIGHT:	return this->mzRight;
	
	case PRM_PPR_WIDTH:			
	case PRM_PPR_NAK_LEFT:			
	case PRM_PPR_NAK_RIGHT:			
	case PRM_PPR_POD_LEFT:			
	case PRM_PPR_POD_RIGHT:			
	case PRM_PPR_IZN_LEFT:			
	case PRM_PPR_IZN_RIGHT:		return this->ppr;
	
	case PRM_PPR_IZN_EXT_LEFT:			
	case PRM_PPR_IZN_EXT_RIGHT:		
	case PRM_PPR_IZN_INT_LEFT:			
	case PRM_PPR_IZN_INT_RIGHT:	return this->pprNew;

	default:					return VParameterCorruptPtr(new VParameterCorrupt);
	}
}

// Копирование из другого блока
bool VCorruptData::CopyFrom(const VCorruptData & src, size_t kadrFrom, size_t kadrTo)
{
	this->Unload();

	if (src.corrupt.IsLoaded())
	{
		this->corrupt.Create(DHT_CORRUPT, VCT_ULONG, kadrTo - kadrFrom + 1, sizeof(uint32_t), src.corrupt.GetVersion());
		for (size_t i = kadrFrom; i <= kadrTo; ++i)
		{
			this->corrupt.GetItem<uint32_t>(i - kadrFrom) = src.corrupt.GetItem<uint32_t>(i);
		}
	}

	if (src.width->IsLoaded())
	{
		this->absCoord->CopyFrom(*src.absCoord, kadrFrom, kadrTo);
		this->width->CopyFrom(*src.width, kadrFrom, kadrTo);
		this->widthFront->CopyFrom(*src.widthFront, kadrFrom, kadrTo);
		this->widthBack->CopyFrom(*src.widthBack, kadrFrom, kadrTo);
		this->level->CopyFrom(*src.level, kadrFrom, kadrTo);
		this->distShort->CopyFrom(*src.distShort, kadrFrom, kadrTo);
		this->distLong->CopyFrom(*src.distLong, kadrFrom, kadrTo);
		this->grad->CopyFrom(*src.grad, kadrFrom, kadrTo);
		this->pitLeft->CopyFrom(*src.pitLeft, kadrFrom, kadrTo);
		this->pitRight->CopyFrom(*src.pitRight, kadrFrom, kadrTo);
		this->richLeft->CopyFrom(*src.richLeft, kadrFrom, kadrTo);
		this->richRight->CopyFrom(*src.richRight, kadrFrom, kadrTo);
		this->curv->CopyFrom(*src.curv, kadrFrom, kadrTo);
		this->kurs->CopyFrom(*src.kurs, kadrFrom, kadrTo);
		this->wearSensor->CopyFrom(*src.wearSensor, kadrFrom, kadrTo);
		this->wearLeft->CopyFrom(*src.wearLeft, kadrFrom, kadrTo);
		this->wearRight->CopyFrom(*src.wearRight, kadrFrom, kadrTo);
		this->ppr->CopyFrom(*src.ppr, kadrFrom, kadrTo);
		this->pprNew->CopyFrom(*src.pprNew, kadrFrom, kadrTo);
		this->zazLeft->CopyFrom(*src.zazLeft, kadrFrom, kadrTo);
		this->zazRight->CopyFrom(*src.zazRight, kadrFrom, kadrTo);
		this->mzLeft->CopyFrom(*src.mzLeft, kadrFrom, kadrTo);
		this->mzRight->CopyFrom(*src.mzRight, kadrFrom, kadrTo);
	}

	this->SetLoaded(true);
	this->message = ERRMSG_OK;
	return true;
}

// Сохранение в файл
bool VCorruptData::Save(VMainFile & file)
{
	bool result = true;

	if (this->corrupt.IsAlloc() && 
		!this->corrupt.Save(file, this->corrupt.GetRecCount(), this->corrupt.GetRecSize()))
	{
		result = false;
	}

	if (this->width->IsLoaded())
	{
		VBasicParameter hand_corrupt;
		hand_corrupt.Create(DHT_HAND_CORRUPT, VCT_ULONG, corrupt.GetRecCount(), sizeof(uint32_t), 1);
		for (size_t i = 0; i < hand_corrupt.GetRecCount(); ++i)
		{
			uint32_t value = 0;
			if (this->absCoord->IsCorrupt(i))	value |= ERROR_ABS_COORD;
			if (this->width->IsCorrupt(i))		value |= ERROR_WIDTH;
			if (this->widthFront->IsCorrupt(i))	value |= ERROR_WIDTH_FRONT;
			if (this->widthBack->IsCorrupt(i))	value |= ERROR_WIDTH_BACK;
			if (this->level->IsCorrupt(i))		value |= ERROR_LEVEL;
			if (this->distShort->IsCorrupt(i))	value |= ERROR_DIST_SHORT;
			if (this->distLong->IsCorrupt(i))	value |= ERROR_DIST_LONG;
			if (this->grad->IsCorrupt(i))		value |= ERROR_GRADIENT;
			if (this->pitLeft->IsCorrupt(i))	value |= ERROR_PIT_LEFT;
			if (this->pitRight->IsCorrupt(i))	value |= ERROR_PIT_RIGHT;
			if (this->richLeft->IsCorrupt(i))	value |= ERROR_RICH_LEFT;
			if (this->richRight->IsCorrupt(i))	value |= ERROR_RICH_RIGHT;
			if (this->curv->IsCorrupt(i))		value |= ERROR_CURV;
			if (this->kurs->IsCorrupt(i))		value |= ERROR_KURS;
			if (this->wearSensor->IsCorrupt(i))	value |= ERROR_WEAR;
			if (this->wearLeft->IsCorrupt(i))	value |= ERROR_WEAR_LEFT;
			if (this->wearRight->IsCorrupt(i))	value |= ERROR_WEAR_RIGHT;
			if (this->ppr->IsCorrupt(i))		value |= ERROR_PPR;
			if (this->pprNew->IsCorrupt(i))     value |= ERROR_PPR_NEW;
			if (this->zazLeft->IsCorrupt(i))	value |= ERROR_ZAZOR_LEFT;
			if (this->zazRight->IsCorrupt(i))	value |= ERROR_ZAZOR_RIGHT;
			if (this->mzLeft->IsCorrupt(i))		value |= ERROR_MAG_ZAZOR_LEFT;
			if (this->mzRight->IsCorrupt(i))	value |= ERROR_MAG_ZAZOR_RIGHT;

			hand_corrupt.GetItem<uint32_t>(i) = value;
		}

		if (!hand_corrupt.Save(file, hand_corrupt.GetRecCount(), hand_corrupt.GetRecSize()))
		{
			result = false;
		}
	}

	return result;
}

