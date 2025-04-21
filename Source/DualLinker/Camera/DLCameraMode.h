// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DLCameraMode.generated.h"

class UDLCameraComponent;

/**
 * FDLCameraModeView
 */
struct FDLCameraModeView
{
	FDLCameraModeView();

	void Blend(const FDLCameraModeView& Other, float OtherWeight);

	FVector Location;
	FRotator Rotation;
	FRotator ControlRotation;
	float FieldOfView;
};

/**
 * [0,1]을 BlendFunction에 맞게 재매핑을 위한 타입
 */
UENUM(BlueprintType)
enum class EDLCameraModeBlendFunction : uint8
{
	Linear,
	/**
	 * EaseIn/Out은 exponent 값에 의해 조절된다:
	 */
	EaseIn,
	EaseOut,
	EaseInOut,
	COUNT UMETA(Hidden)
};

/**
 * 
 */
UCLASS(Abstract, NotBlueprintable)
class DUALLINKER_API UDLCameraMode : public UObject
{
	GENERATED_BODY()
	
public:
	UDLCameraMode();

	void UpdateCameraMode(float DeltaTime);
	virtual void UpdateView(float DeltaTime);
	void SetBlendWeight(float Weight);
	void UpdateBlending(float DeltaTime);

	UDLCameraComponent* GetDLCameraComponent() const;
	virtual UWorld* GetWorld() const override;
	AActor* GetTargetActor() const;
	FVector GetPivotLocation() const;
	FRotator GetPivotRotation() const;

	/** CameraMode에 의해 생성된 CameraModeView */
	FDLCameraModeView View;

	/** Camera Mode의 FOV */
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "5.0", UIMax = "170", ClampMin = "5.0", Clampmax = "170.0"))
	float FieldOfView;

	/** View에 대한 Pitch [Min, Max] */
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", Clampmax = "89.9"))
	float ViewPitchMin;

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", Clampmax = "89.9"))
	float ViewPitchMax;

	/** 전환이 완료되는데 걸리는 시간을 의미 */
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendTime;

	/** 선형적인 Blend 값 [0, 1] */
	float BlendAlpha;

	/**
	 * 카메라 간의 전환 비율값
	 * 앞서 BlendAlpha의 값을 매핑하여 최종 BlendWeight를 계산
	 */
	float BlendWeight;

	/**
	* EaseIn/Out에 사용한 Exponent
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendExponent;

	/** Blend function */
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	EDLCameraModeBlendFunction BlendFunction;

	/** If true, skips all interpolation and puts camera in ideal location.  Automatically set to false next frame. */
	UPROPERTY(transient)
	uint32 bResetInterpolation : 1;
};

/** Camera Blending을 담당하는 객체 */
UCLASS()
class UDLCameraModeStack : public UObject
{
	GENERATED_BODY()
public:
	UDLCameraModeStack();

	UDLCameraMode* GetCameraModeInstance(TSubclassOf<UDLCameraMode>& CameraModeClass);
	void PushCameraMode(TSubclassOf<UDLCameraMode>& CameraModeClass);
	void EvaluateStack(float DeltaTime, FDLCameraModeView& OutCameraModeView);
	void UpdateStack(float DeltaTime);
	void BlendStack(FDLCameraModeView& OutCameraModeView) const;

	/** 생성된 CameraMode를 관리, 해당 카메라 모드 클래스 타입의 존재 유무 파악용 */
	UPROPERTY()
	TArray<TObjectPtr<UDLCameraMode>> CameraModeInstances;

	/** Camera Matrix Blend 업데이트 진행 큐 */
	UPROPERTY()
	TArray<TObjectPtr<UDLCameraMode>> CameraModeStack;
};