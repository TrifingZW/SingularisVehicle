/* =====================================================================
 * BaseWheeledVehiclePawn.h
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2024 TrifingZW <TrifingZW@gmail.com>
 * 
 * Copyright (c) 2024 TrifingZW
 * Licensed under MIT License
 * ===================================================================== */

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "BaseWheeledVehiclePawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UChaosWheeledVehicleMovementComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogBaseWheeledVehiclePawn, Log, All);

/**
 *  基本载具 Pawn 类
 *  处理所有载具类型的通用功能
 *  包括输入处理和相机管理
 *  
 *  具体的载具配置由子类处理
 */
UCLASS(abstract)
class SINGULARISVEHICLE_API ABaseWheeledVehiclePawn : public AWheeledVehiclePawn
{
	// 宏，用于自动生成与类相关的代码，确保引擎可以识别和使用此类。
	GENERATED_BODY()

	/** 前置摄像头弹簧臂 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* FrontSpringArm;

	/** 前置摄像头组件 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FrontCamera;

	/** 后置摄像头弹簧臂 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* BackSpringArm;

	/** 后置摄像头组件 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* BackCamera;

	/** 将指针转换为 Chaos 车辆运动组件 */
	TObjectPtr<UChaosWheeledVehicleMovementComponent> ChaosVehicleMovement;

protected:
	/** 转向 Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SteeringAction;

	/** 油门 Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ThrottleAction;

	/** 制动 Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* BrakeAction;

	/** 手刹 Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* HandbrakeAction;

	/** 看向周围 Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAroundAction;

	/** 切换摄像头 Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ToggleCameraAction;

	/** 重置载具 Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ResetVehicleAction;

	/** 记录哪个摄像头是激活的 */
	bool bFrontCameraActive = false;

public:
	ABaseWheeledVehiclePawn();

	// 开始实现 Pawn 接口
	virtual void SetupPlayerInputComponent(UInputComponent* Component) override;
	// 结束 Pawn 接口

	// 开始 Actor 接口
	virtual void Tick(float Delta) override;
	// 结束 Actor 接口

protected:
	/** 处理转向输入 */
	void Steering(const FInputActionValue& Value);

	/** 处理油门输入 */
	void Throttle(const FInputActionValue& Value);

	/** 处理制动输入 */
	void Brake(const FInputActionValue& Value);

	/** 处理制动器开始/停止输入 */
	void StartBrake(const FInputActionValue& Value);
	void StopBrake(const FInputActionValue& Value);

	/** 处理手刹的开始/停止输入 */
	void StartHandbrake(const FInputActionValue& Value);
	void StopHandbrake(const FInputActionValue& Value);

	/** 处理环顾输入 */
	void LookAround(const FInputActionValue& Value);

	/** 处理切换相机输入 */
	void ToggleCamera(const FInputActionValue& Value);

	/** 处理重置车辆输入 */
	void ResetVehicle(const FInputActionValue& Value);

	/** 打开或关闭刹车灯时调用 */
	UFUNCTION(BlueprintImplementableEvent, Category="Vehicle")
	void BrakeLights(bool bBraking);

public:
	/** Returns 前置摄像头弹簧臂子对象 */
	FORCEINLINE USpringArmComponent* GetFrontSpringArm() const { return FrontSpringArm; }
	/** Returns 前摄像头子对象 */
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FrontCamera; }
	/** Returns 后置摄像头弹簧臂子对象 */
	FORCEINLINE USpringArmComponent* GetBackSpringArm() const { return BackSpringArm; }
	/** Returns 后摄像头子对象 */
	FORCEINLINE UCameraComponent* GetBackCamera() const { return BackCamera; }
	/** Returns 已转换的 Chaos 车辆运动子对象 */
	FORCEINLINE const TObjectPtr<UChaosWheeledVehicleMovementComponent>& GetChaosVehicleMovement() const { return ChaosVehicleMovement; }

private:
	void SetVehicleMovementParameters() const;
};
