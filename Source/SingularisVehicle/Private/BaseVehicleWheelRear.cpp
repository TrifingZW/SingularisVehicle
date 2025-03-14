/* =====================================================================
 * BaseVehicleWheelRear.cpp
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#include "BaseVehicleWheelRear.h"

UBaseVehicleWheelRear::UBaseVehicleWheelRear()
{
	AxleType = EAxleType::Rear; // 设置轮轴类型为后轴 (Rear)，表示该轮子是后轴的一部分，通常用于后轮驱动或四轮驱动车辆。
	bAffectedByHandbrake = true; // 设置轮子会受到手刹的影响。为true时，手刹拉起时，轮子会停止转动，适用于需要手刹控制的轮子。
	bAffectedByEngine = true; // 设置轮子会受到发动机动力的影响。为true时，轮子参与车辆的动力传递，通常用于驱动轮（如前驱或后驱车辆）。

	WheelRadius = 40.f; // 轮胎半径，单位是厘米（cm）。表示轮胎的尺寸，决定了轮胎的大小和地面接触面积。
	WheelWidth = 20.0f; // 轮胎宽度，单位是厘米（cm）。表示轮胎的宽度，影响牵引力、操控性等。
	FrictionForceMultiplier = 2.0f; // 摩擦力倍增因子。用于调整轮胎与地面之间的摩擦力，值越大，地面抓地力越强。
	SlipThreshold = 100.0f; // 滑移阈值。用于控制轮胎在滑移开始时的行为，一旦超过此阈值，轮胎开始产生滑移效果。
	SkidThreshold = 100.0f; // 打滑阈值。决定轮胎何时进入打滑状态。值较大时，轮胎更容易打滑，影响车辆的稳定性。
	MaxSteerAngle = 0.0f; // 最大转向角度。表示轮胎可旋转的最大角度，通常对于后轮，转向角为0，后轮一般不参与转向。
	MaxHandBrakeTorque = 6000.0f; // 最大手刹扭矩。表示手刹时施加的最大扭矩，影响手刹操作的效果，值越大，手刹越强烈。
}
