/* =====================================================================
 * BaseVehicleWheelFront.cpp
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#include "BaseVehicleWheelFront.h"

UBaseVehicleWheelFront::UBaseVehicleWheelFront()
{
	AxleType = EAxleType::Front; // 设置轮轴类型为前轴 (Front)，表示该轮子是前轴的一部分，通常用于前轮驱动或四轮驱动的车辆。
	bAffectedBySteering = true; // 设置轮子是否受到转向的影响。为true时，轮子会根据转向输入转动，通常用于前轮转向。
	MaxSteerAngle = 40.f; // 设置最大转向角度为40度。决定了前轮可以旋转的最大角度，影响车辆的转向灵敏度。

	WheelRadius = 40.0f; // 设置轮胎的半径为39厘米，影响轮胎的大小和地面接触面积。
	WheelWidth = 20.0f; // 设置轮胎的宽度为35厘米，影响轮胎的抓地力和稳定性。
	FrictionForceMultiplier = 3.0f; // 设置摩擦力倍增因子为3.0，调整轮胎与地面之间的摩擦力，值越大，轮胎的抓地力越强。

	MaxBrakeTorque = 1500.0f; // 设置最大刹车扭矩为4500，影响车辆刹车时施加的最大力量，值越大，刹车效果越强。
	MaxHandBrakeTorque = 3000.0f; // 设置最大手刹扭矩为6000，影响手刹的效果，值越大，手刹的强度越大，能更有效地让车轮停止转动。
}
