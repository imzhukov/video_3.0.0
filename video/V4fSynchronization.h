#pragma once

#include "VDppSynchronization.h"
#include "VCoordinatesRecord.h"
#include "V4fFrame.h"

#include "options.h"

class V4fSynchronization : public VDppSynchronization<VCoordinatesRecord, V4fFrame>
{};

DECLARE_VARIABLE_MT(V4fSynchronization, MAIN_SYNCHRONIZATION, V4fSynchronization());