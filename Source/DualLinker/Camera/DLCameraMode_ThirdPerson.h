// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLCameraMode.h"
#include "DLPenetrationAvoidanceFeeler.h"

#include "DLCameraMode_ThirdPerson.generated.h"

class UCurveVector;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class DUALLINKER_API UDLCameraMode_ThirdPerson : public UDLCameraMode
{
	GENERATED_BODY()

public:
	UDLCameraMode_ThirdPerson();

	virtual void UpdateView(float DeltaTime) override;
	void UpdatePreventPenetration(float DeltaTime);
	void PreventCameraPenetration(class AActor const& ViewTarget, FVector const& SafeLoc, FVector& CameraLoc, float const& DeltaTime, float& DistBlockedPct, bool bSingleRayOnly);

	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
	TObjectPtr<const UCurveVector> TargetOffsetCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	float PenetrationBlendInTime = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	float PenetrationBlendOutTime = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	bool bPreventPenetration = true;

	/** If true, try to detect nearby walls and move the camera in anticipation.  Helps prevent popping. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	bool bDoPredictiveAvoidance = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	float CollisionPushOutDistance = 2.f;

	/** When the camera's distance is pushed into this percentage of its full distance due to penetration */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	float ReportPenetrationPercent = 0.f;

	/**
	 * 카메라가 오브젝트나 월드 geometry에 너무 가까워지거나, 관통하지 않도록 안전한 위치로 보정하는 역할
	 * Index: 0  : 기본 feeler로 가장 중심(주요, 중앙) ray를 사용하여 주된 충돌 감지를 수행
	 *			   이 feeler는 기본적으로 카메라와 SafeLocation 사이의 충돌 여부를 결정하는 기준이 됨
	 * Index: 1+ : 보조(또는 예측) feeler로서, 만약 bDoPredictiveAvoidance가 true인 경우 사용
	 *             이 feeler들은 플레이어가 현재 방향에서 벗어나 다른 방향(예측 가능한 회전 방향)으로 이동할 가능성을 미리 탐지하여
	 *			   카메라가 갑자기 충돌하는 것을 미리 감지하고 카메라 위치 보정을 미리 준비할 수 있게 함      
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	TArray<FDLPenetrationAvoidanceFeeler> PenetrationAvoidanceFeelers;

	UPROPERTY(Transient)
	float AimLineToDesiredPosBlockedPct;
};
