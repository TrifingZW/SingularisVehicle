/* =====================================================================
 * BaseWheeledVehiclePawn.cpp
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#include "BaseWheeledVehiclePawn.h"

#include "ChaosWheeledVehicleMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#define LOCTEXT_NAMESPACE "BaseWheeledVehiclePawn"

DEFINE_LOG_CATEGORY(LogBaseWheeledVehiclePawn);

ABaseWheeledVehiclePawn::ABaseWheeledVehiclePawn()
{
	// 构造前置摄像头和弹簧臂
	FrontSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Front Spring Arm"));
	FrontSpringArm->SetupAttachment(GetMesh());
	FrontSpringArm->TargetArmLength = 0.0f;
	FrontSpringArm->bDoCollisionTest = false;
	FrontSpringArm->bEnableCameraRotationLag = true;
	FrontSpringArm->CameraRotationLagSpeed = 15.0f;
	FrontSpringArm->SetRelativeLocation(FVector(30.0f, 0.0f, 120.0f));

	FrontCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Front Camera"));
	FrontCamera->SetupAttachment(FrontSpringArm);
	FrontCamera->bAutoActivate = false;

	// 构造后置摄像头和弹簧臂
	BackSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Back Spring Arm"));
	BackSpringArm->SetupAttachment(GetMesh());
	BackSpringArm->TargetArmLength = 650.0f;
	BackSpringArm->SocketOffset.Z = 150.0f;
	BackSpringArm->bDoCollisionTest = false;
	BackSpringArm->bInheritPitch = false;
	BackSpringArm->bInheritRoll = false;
	BackSpringArm->bEnableCameraRotationLag = true;
	BackSpringArm->CameraRotationLagSpeed = 2.0f;
	BackSpringArm->CameraLagMaxDistance = 50.0f;

	BackCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Back Camera"));
	BackCamera->SetupAttachment(BackSpringArm);

	// 配置载具网格
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(FName("Vehicle"));

	// 获取 Chaos 车辆移动组件
	ChaosVehicleMovement = CastChecked<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement());

	// 设置车轮基本参数
	SetVehicleMovementParameters();
}

void ABaseWheeledVehiclePawn::SetupPlayerInputComponent(UInputComponent* Component)
{
	Super::SetupPlayerInputComponent(Component);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(Component))
	{
		// 转向 
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &ABaseWheeledVehiclePawn::Steering);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &ABaseWheeledVehiclePawn::Steering);

		// 油门 
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ABaseWheeledVehiclePawn::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ABaseWheeledVehiclePawn::Throttle);

		// 制动 
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &ABaseWheeledVehiclePawn::Brake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Started, this, &ABaseWheeledVehiclePawn::StartBrake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &ABaseWheeledVehiclePawn::StopBrake);

		// 手刹 
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Started, this, &ABaseWheeledVehiclePawn::StartHandbrake);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &ABaseWheeledVehiclePawn::StopHandbrake);

		// 环顾
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &ABaseWheeledVehiclePawn::LookAround);

		// 切换相机
		EnhancedInputComponent->BindAction(ToggleCameraAction, ETriggerEvent::Triggered, this, &ABaseWheeledVehiclePawn::ToggleCamera);

		// 重置车辆
		EnhancedInputComponent->BindAction(ResetVehicleAction, ETriggerEvent::Triggered, this, &ABaseWheeledVehiclePawn::ResetVehicle);
	}
	else
	{
		UE_LOG(LogBaseWheeledVehiclePawn,
		       Error,
		       TEXT(
			       "'%s' 未能找到 Enhanced Input 组件！此模板是基于 Enhanced Input 系统构建的。如果您打算使用旧版输入系统，则需要更新此 C++ 文件。"
		       ),
		       *GetNameSafe(this));
	}
}

void ABaseWheeledVehiclePawn::Tick(const float Delta)
{
	Super::Tick(Delta);

	// 如果车辆在空中，添加一些角度阻尼
	// 角度阻尼（Angular Damping）：是指物体在旋转时受到的阻力，它会减缓物体的旋转速度
	// 此处的作用是让车辆在空中不会无限旋转，而是会逐渐减速
	const bool bMovingOnGround = ChaosVehicleMovement->IsMovingOnGround();
	GetMesh()->SetAngularDamping(bMovingOnGround ? 0.0f : 3.0f);

	/*// 将相机的偏航角重新对准正面
	// 偏航角（Yaw） ：是物体在三维空间中绕垂直轴（通常是 Y 轴）旋转的角度
	// 此处的作用是让后置摄像头的偏航角（Yaw）平滑地回到 0 度，以确保它始终面向车辆的正前方
	float CameraYaw = BackSpringArm->GetRelativeRotation().Yaw;
	CameraYaw = FMath::FInterpTo(CameraYaw, 0.0f, Delta, 1.0f);
	BackSpringArm->SetRelativeRotation(FRotator(0.0f, CameraYaw, 0.0f));*/
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ABaseWheeledVehiclePawn::Steering(const FInputActionValue& Value)
{
	// 获取转向的输入幅度并给车辆运动组件
	const float SteeringValue = Value.Get<float>();
	ChaosVehicleMovement->SetSteeringInput(SteeringValue);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ABaseWheeledVehiclePawn::Throttle(const FInputActionValue& Value)
{
	// 获取油门的输入幅度并给车辆运动组件
	const float ThrottleValue = Value.Get<float>();
	ChaosVehicleMovement->SetThrottleInput(ThrottleValue);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ABaseWheeledVehiclePawn::Brake(const FInputActionValue& Value)
{
	// 获取刹车的输入幅度并给车辆运动组件
	const float BreakValue = Value.Get<float>();
	ChaosVehicleMovement->SetBrakeInput(BreakValue);
}

void ABaseWheeledVehiclePawn::StartBrake([[maybe_unused]] const FInputActionValue& Value)
{
	// 制动输入时启动刹车灯
	BrakeLights(true);
}

void ABaseWheeledVehiclePawn::StopBrake([[maybe_unused]] const FInputActionValue& Value)
{
	// 关闭刹车灯
	BrakeLights(false);
	// 将制动输入重置为零
	ChaosVehicleMovement->SetBrakeInput(0.0f);
}

void ABaseWheeledVehiclePawn::StartHandbrake([[maybe_unused]] const FInputActionValue& Value)
{
	// 启动手刹
	ChaosVehicleMovement->SetHandbrakeInput(true);

	// 打开刹车灯
	BrakeLights(true);
}

void ABaseWheeledVehiclePawn::StopHandbrake([[maybe_unused]] const FInputActionValue& Value)
{
	// 关闭手刹
	ChaosVehicleMovement->SetHandbrakeInput(false);

	// 关闭刹车灯
	BrakeLights(false);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ABaseWheeledVehiclePawn::LookAround(const FInputActionValue& Value)
{
	// 获取环顾的输入幅度并给后置弹簧臂添加本地旋转
	const float LookValue = Value.Get<float>();
	BackSpringArm->AddLocalRotation(FRotator(0.0f, LookValue, 0.0f));
}

void ABaseWheeledVehiclePawn::ToggleCamera([[maybe_unused]] const FInputActionValue& Value)
{
	// 切换摄像头
	bFrontCameraActive = !bFrontCameraActive;
	FrontCamera->SetActive(bFrontCameraActive);
	BackCamera->SetActive(!bFrontCameraActive);
}


void ABaseWheeledVehiclePawn::ResetVehicle([[maybe_unused]] const FInputActionValue& Value)
{
	// 重置到当前略高的位置
	const FVector ResetLocation = GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);

	/// 重置为我们的偏航角。忽略俯仰和滚转
	FRotator ResetRotation = GetActorRotation();
	ResetRotation.Pitch = 0.0f;
	ResetRotation.Roll = 0.0f;

	// 将演员传送到重置点并重置物理状态
	SetActorTransform(FTransform(ResetRotation, ResetLocation, FVector::OneVector), false, nullptr, ETeleportType::TeleportPhysics);

	GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);

	UE_LOG(LogBaseWheeledVehiclePawn, Error, TEXT("Reset Vehicle"));
}

void ABaseWheeledVehiclePawn::SetVehicleMovementParameters() const
{
	// 注意：以下代码为虚幻引擎5 Chaos车辆物理系统的C++配置，用于定义车辆的物理行为

	// ==================== 底盘设置 ====================
	ChaosVehicleMovement->ChassisHeight = 144.0f; // 底盘离地高度（单位：厘米），影响车辆重心和稳定性
	ChaosVehicleMovement->DragCoefficient = 0.31f; // 空气阻力系数，值越低车辆空气动力学性能越好

	// ==================== 车轮设置 ====================
	ChaosVehicleMovement->bLegacyWheelFrictionPosition = true; // 启用旧版车轮摩擦力计算方式（兼容性选项）
	ChaosVehicleMovement->WheelSetups.SetNum(4); // 定义车辆有4个车轮

	/*// 前轮-左 (索引0)
	ChaosVehicleMovement->WheelSetups[0].WheelClass = UVehicleSportsWheelFront::StaticClass(); // 使用前轮物理类（含摩擦、悬挂等参数）
	ChaosVehicleMovement->WheelSetups[0].BoneName = FName("Phys_Wheel_FL"); // 绑定到骨骼"Phys_Wheel_FL"（左前轮）
	ChaosVehicleMovement->WheelSetups[0].AdditionalOffset = FVector(0.0f, 0.0f, 0.0f); // 车轮位置微调（此处无偏移）

	// 前轮-右 (索引1)
	ChaosVehicleMovement->WheelSetups[1].WheelClass = UVehicleSportsWheelFront::StaticClass(); // 前轮物理类
	ChaosVehicleMovement->WheelSetups[1].BoneName = FName("Phys_Wheel_FR"); // 骨骼"Phys_Wheel_FR"（右前轮）
	ChaosVehicleMovement->WheelSetups[1].AdditionalOffset = FVector(0.0f, 0.0f, 0.0f); // 无额外偏移

	// 后轮-左 (索引2)
	ChaosVehicleMovement->WheelSetups[2].WheelClass = UVehicleSportsWheelRear::StaticClass(); // 使用后轮物理类（可能与前轮不同）
	ChaosVehicleMovement->WheelSetups[2].BoneName = FName("Phys_Wheel_BL"); // 骨骼"Phys_Wheel_BL"（左后轮）
	ChaosVehicleMovement->WheelSetups[2].AdditionalOffset = FVector(0.0f, 0.0f, 0.0f); // 无偏移

	// 后轮-右 (索引3)
	ChaosVehicleMovement->WheelSetups[3].WheelClass = UVehicleSportsWheelRear::StaticClass(); // 后轮物理类
	ChaosVehicleMovement->WheelSetups[3].BoneName = FName("Phys_Wheel_BR"); // 骨骼"Phys_Wheel_BR"（右后轮）
	ChaosVehicleMovement->WheelSetups[3].AdditionalOffset = FVector(0.0f, 0.0f, 0.0f); // 无偏移*/

	// ==================== 引擎设置 ====================
	// 注意：扭矩曲线需在蓝图中定义（控制不同转速下的扭矩输出）
	ChaosVehicleMovement->EngineSetup.MaxTorque = 750.0f; // 最大扭矩（单位：牛米），决定加速能力
	ChaosVehicleMovement->EngineSetup.MaxRPM = 7000.0f; // 发动机最高转速（转/分钟），超限会断油
	ChaosVehicleMovement->EngineSetup.EngineIdleRPM = 900.0f; // 怠速转速（车辆静止时维持的转速）
	ChaosVehicleMovement->EngineSetup.EngineBrakeEffect = 0.2f; // 发动机制动强度（0.0-1.0），松油门时的减速效果
	ChaosVehicleMovement->EngineSetup.EngineRevUpMOI = 5.0f; // 发动机旋转惯量，值越大转速变化越慢
	ChaosVehicleMovement->EngineSetup.EngineRevDownRate = 600.0f; // 转速下降速率（RPM/秒），影响换挡响应

	// ==================== 变速箱设置 ====================
	ChaosVehicleMovement->TransmissionSetup.bUseAutomaticGears = true; // 启用自动换挡
	ChaosVehicleMovement->TransmissionSetup.bUseAutoReverse = true; // 启用自动倒车（低速时自动切倒挡）
	ChaosVehicleMovement->TransmissionSetup.FinalRatio = 2.81f; // 主减速器传动比，放大所有档位扭矩
	ChaosVehicleMovement->TransmissionSetup.ChangeUpRPM = 6000.0f; // 升挡转速阈值（达到此值自动升挡）
	ChaosVehicleMovement->TransmissionSetup.ChangeDownRPM = 2000.0f; // 降挡转速阈值（低于此值自动降挡）
	ChaosVehicleMovement->TransmissionSetup.GearChangeTime = 0.2f; // 换挡耗时（秒），模拟换挡延迟
	ChaosVehicleMovement->TransmissionSetup.TransmissionEfficiency = 0.9f; // 传动效率（0.0-1.0），90%效率意味10%动力损耗

	// 定义前进挡传动比（数值越大扭矩越大，极速越低）
	ChaosVehicleMovement->TransmissionSetup.ForwardGearRatios.SetNum(5); // 5个前进挡
	ChaosVehicleMovement->TransmissionSetup.ForwardGearRatios[0] = 4.25f; // 1挡传动比（高扭矩，低速度）
	ChaosVehicleMovement->TransmissionSetup.ForwardGearRatios[1] = 2.52f; // 2挡传动比
	ChaosVehicleMovement->TransmissionSetup.ForwardGearRatios[2] = 1.66f; // 3挡传动比
	ChaosVehicleMovement->TransmissionSetup.ForwardGearRatios[3] = 1.22f; // 4挡传动比
	ChaosVehicleMovement->TransmissionSetup.ForwardGearRatios[4] = 1.0f; // 5挡传动比（超比挡，高速低油耗）

	// 定义倒挡传动比
	ChaosVehicleMovement->TransmissionSetup.ReverseGearRatios.SetNum(1); // 1个倒挡
	ChaosVehicleMovement->TransmissionSetup.ReverseGearRatios[0] = 4.04f; // 倒挡传动比（通常接近1挡）

	// ==================== 转向设置 ====================
	// 注意：转向曲线需在蓝图中定义（控制转向角度随车速变化）
	ChaosVehicleMovement->SteeringSetup.SteeringType = ESteeringType::Ackermann; // 阿克曼转向几何（优化过弯）
	ChaosVehicleMovement->SteeringSetup.AngleRatio = 0.7f; // 转向角度比率（0.0-1.0），值越小转向越平缓
}

#undef LOCTEXT_NAMESPACE
