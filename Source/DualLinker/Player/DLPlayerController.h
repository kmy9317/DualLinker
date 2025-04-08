// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DualLinker/Camera/DLCameraAssistInterface.h"
#include "DLPlayerController.generated.h"

class UInputMappingContext;
class UDLInputConfig;
class UDLCameraMode;

struct FInputActionValue;

/**
 * 
 */
UCLASS()
class DUALLINKER_API ADLPlayerController : public APlayerController, public IDLCameraAssistInterface
{
    GENERATED_BODY()
public:
    ADLPlayerController();

    void BindInputActions(UDLInputConfig* InInputConfig);

    //~IDLCameraAssistInterface interface
    virtual void OnCameraPenetratingTarget() override;
    //~End of IDLCameraAssistInterface interface


protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;


public:
    // Enhanced Input √ ±‚»≠
    void SetupEnhancedInput();
    void SetupDefaultCameraMode(const TSubclassOf<UDLCameraMode> CameraMode);

private:

    void Input_Move(const FInputActionValue& Value);
    void Input_LookMouse(const FInputActionValue& Value);

    TSubclassOf<UDLCameraMode> DetermineCameraMode() const;

    UFUNCTION()
    void ApplyLoadedInputMapping(UInputMappingContext* LoadedMapping);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputMappingContext> DLContext;

    TSubclassOf<UDLCameraMode> DefaultCameraMode;
};
