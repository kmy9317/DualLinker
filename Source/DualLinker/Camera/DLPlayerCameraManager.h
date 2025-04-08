// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "DLPlayerCameraManager.generated.h"

#define DL_CAMERA_DEFAULT_FOV (80.0f)
#define DL_CAMERA_DEFAULT_PITCH_MIN (-89.0f)
#define DL_CAMERA_DEFAULT_PITCH_MAX (89.0f)

/**
 * 
 */
UCLASS()
class DUALLINKER_API ADLPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:
	ADLPlayerCameraManager();
};
