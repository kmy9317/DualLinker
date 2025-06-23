// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "DLCameraMode.h"

#include "DLCameraComponent.generated.h"

class UDLCameraModeStack;

/** 템플릿 전방 선언 */
template <class TClass> class TSubclassOf;

DECLARE_DELEGATE_RetVal(TSubclassOf<UDLCameraMode>, FDLCameraModeDelegate);

/**
 * 
 */
UCLASS()
class DUALLINKER_API UDLCameraComponent : public UCameraComponent
{
	GENERATED_BODY()
public:
	UDLCameraComponent();

	static UDLCameraComponent* FindCameraComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UDLCameraComponent>() : nullptr); }

	AActor* GetTargetActor() const { return GetOwner(); }
	void UpdateCameraModes();

	/**-Begin CameraComponent interface */
	virtual void OnRegister() final;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) final;
	/**-End CameraComponent interface */

	 /** 카메라의 blending 기능을 지원하는 stack */
	UPROPERTY()
	TObjectPtr<UDLCameraModeStack> CameraModeStack;

	/** 현재 CameraMode를 가져오는 Delegate */
	FDLCameraModeDelegate DetermineCameraModeDelegate;
};
