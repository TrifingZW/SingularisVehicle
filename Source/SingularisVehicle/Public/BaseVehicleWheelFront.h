/* =====================================================================
 * BaseVehicleWheelFront.h
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#pragma once

#include "CoreMinimal.h"
#include "ChaosVehicleWheel.h"
#include "BaseVehicleWheelFront.generated.h"

/**
 * 基本载具的前轮定义
 */
UCLASS(abstract)
class UBaseVehicleWheelFront : public UChaosVehicleWheel
{
	GENERATED_BODY()

public:
	UBaseVehicleWheelFront();
};
