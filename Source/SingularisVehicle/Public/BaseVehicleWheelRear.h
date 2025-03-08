/* =====================================================================
 * BaseVehicleWheelRear.h
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#pragma once

#include "CoreMinimal.h"
#include "ChaosVehicleWheel.h"
#include "BaseVehicleWheelRear.generated.h"

/**
 * 基本载具的后轮定义
 */
UCLASS(abstract)
class UBaseVehicleWheelRear : public UChaosVehicleWheel
{
	GENERATED_BODY()

public:
	UBaseVehicleWheelRear();
};
