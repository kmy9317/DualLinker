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
	 * These are the feeler rays that are used to find where to place the camera.
	 * Index: 0  : This is the normal feeler we use to prevent collisions.
	 * Index: 1+ : These feelers are used if you bDoPredictiveAvoidance=true, to scan for potential impacts if the player
	 *             were to rotate towards that direction and primitively collide the camera so that it pulls in before
	 *             impacting the occluder.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Collision")
	TArray<FDLPenetrationAvoidanceFeeler> PenetrationAvoidanceFeelers;

	UPROPERTY(Transient)
	float AimLineToDesiredPosBlockedPct;
};
