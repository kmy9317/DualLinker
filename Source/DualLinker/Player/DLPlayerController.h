// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DualLinker/Camera/DLCameraAssistInterface.h"
#include "GameplayTagContainer.h"

#include "DLPlayerController.generated.h"

class ADLPlayerState;
class UDLAbilitySystemComponent;
class UInputMappingContext;
class UDLInputConfig;
class UDLCameraMode;
class UDLPawnData;

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

    UFUNCTION(BlueprintCallable, Category = "DL|PlayerController")
    ADLPlayerState* GetDLPlayerState() const;
    
    UFUNCTION(BlueprintCallable, Category = "DL|PlayerController")
    UDLAbilitySystemComponent* GetDLAbilitySystemComponent() const;
    
    void ReceivePawnData(const TArray<TSoftObjectPtr<UDLPawnData>> InPawnDataList);

    void BindInputActions(UDLInputConfig* InInputConfig);

    //~APlayerController interface
    virtual void UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents) override;
    //~End of APlayerController interface

    //~IDLCameraAssistInterface interface
    virtual void OnCameraPenetratingTarget() override;
    //~End of IDLCameraAssistInterface interface

    // Enhanced Input 초기화
    void SetupEnhancedInput();
    void SetupDefaultCameraMode(const TSubclassOf<UDLCameraMode> CameraMode);

protected:
    //~APlayerController interface
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;
    virtual void OnPossess(APawn* NewPawn) override;
    virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
    //~End of APlayerController interface

    void Input_AbilityInputTagStarted(FGameplayTag InputTag);
    void Input_AbilityInputTagPressed(FGameplayTag InputTag);
    void Input_AbilityInputTagReleased(FGameplayTag InputTag);
    
protected:
    bool bHideViewTargetPawnNextFrame = false;

private:

    void Input_Move(const FInputActionValue& Value);
    void Input_LookMouse(const FInputActionValue& Value);

    void Input_ChangeEquip_Weapon_Primary();
    void Input_ChangeEquip_Weapon_Secondary();

    TSubclassOf<UDLCameraMode> DetermineCameraMode() const;

    UFUNCTION()
    void ApplyLoadedInputMapping(UInputMappingContext* LoadedMapping);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputMappingContext> DLContext;

    TSubclassOf<UDLCameraMode> DefaultCameraMode;


};
