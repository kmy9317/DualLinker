// Fill out your copyright notice in the Description page of Project Settings.


#include "DLPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "DualLinker/Input/DLInputComponent.h"
#include "DualLinker/Input/DLInputConfig.h"
#include "DualLinker/DLGameplayTags.h"
#include "DualLinker/System/DLInputMappingSubsystem.h"
#include "DualLinker/Character/DLPlayerCharacter.h"
#include "DualLinker/Camera/DLPlayerCameraManager.h"
#include "DualLinker/Camera/DLCameraComponent.h"
#include "DualLinker/Camera/DLCameraMode.h"

ADLPlayerController::ADLPlayerController()
{
    PlayerCameraManagerClass = ADLPlayerCameraManager::StaticClass();

}

void ADLPlayerController::BeginPlay()
{
    Super::BeginPlay();
    SetupEnhancedInput();
}

void ADLPlayerController::SetupEnhancedInput()
{
    if (UDLInputMappingSubsystem* InputMappingSubsystem = GetGameInstance()->GetSubsystem<UDLInputMappingSubsystem>())
    {
        // 이미 로드된 경우 즉시 적용
        if (UInputMappingContext* LoadedMapping = InputMappingSubsystem->GetDefaultInputMappingContext())
        {
            InputMappingSubsystem->ApplyDefaultInputMapping(this);
        }
        else
        {
            // 비동기 로드가 끝난 후 적용
            InputMappingSubsystem->OnDefaultInputMappingLoaded.AddDynamic(this, &ADLPlayerController::ApplyLoadedInputMapping);
        }
    }
}

void ADLPlayerController::SetupDefaultCameraMode(const TSubclassOf<UDLCameraMode> CameraMode)
{
    if (GetPawn())
    {
        if (UDLCameraComponent* CameraComponent = UDLCameraComponent::FindCameraComponent(GetPawn()))
        {
            DefaultCameraMode = CameraMode;
            CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
        }
    }
}

void ADLPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
}

void ADLPlayerController::BindInputActions(UDLInputConfig* InInputConfig)
{
    UDLInputComponent* DLInputComponent = CastChecked<UDLInputComponent>(InputComponent);
    const FDLGameplayTags& GameplayTags = FDLGameplayTags::Get();

    DLInputComponent->BindNativeAction(InInputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, false);
    DLInputComponent->BindNativeAction(InInputConfig, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, false);
}

void ADLPlayerController::OnCameraPenetratingTarget()
{
    // TODO: Hidden 로직
}

void ADLPlayerController::Input_Move(const FInputActionValue& Value)
{
    if (APawn* ControlledPawn = GetPawn())
    {
        if (ADLPlayerCharacter* PlayerCharacter = Cast<ADLPlayerCharacter>(ControlledPawn))
        {
            PlayerCharacter->Move(Value.Get<FVector2D>());
        }
    }
}

void ADLPlayerController::Input_LookMouse(const FInputActionValue& Value)
{
    if (APawn* ControlledPawn = GetPawn())
    {
        if (ADLPlayerCharacter* PlayerCharacter = Cast<ADLPlayerCharacter>(ControlledPawn))
        {
            PlayerCharacter->Look(Value.Get<FVector2D>());
        }
    }
}

TSubclassOf<UDLCameraMode> ADLPlayerController::DetermineCameraMode() const
{
    if (DefaultCameraMode) return DefaultCameraMode;
    return nullptr;
}

void ADLPlayerController::ApplyLoadedInputMapping(UInputMappingContext* LoadedMapping)
{
    if (LoadedMapping)
    {
        if (UDLInputMappingSubsystem* InputMappingSubsystem = GetGameInstance()->GetSubsystem<UDLInputMappingSubsystem>())
        {
            InputMappingSubsystem->ApplyDefaultInputMapping(this);
        }
    }
}