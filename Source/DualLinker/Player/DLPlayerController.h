// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DLPlayerController.generated.h"

class UCharacterControlComponent;
class UInputMappingContext;
class UDLInputConfig;

struct FInputActionValue;

/**
 * 
 */
UCLASS()
class DUALLINKER_API ADLPlayerController : public APlayerController
{
    GENERATED_BODY()
public:
    ADLPlayerController();

    void BindInputActions(UDLInputConfig* InInputConfig);

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;


public:
    // Enhanced Input √ ±‚»≠
    void SetupEnhancedInput();

private:

    void Input_Move(const FInputActionValue& Value);
    void Input_LookMouse(const FInputActionValue& Value);

    UFUNCTION()
    void ApplyLoadedInputMapping(UInputMappingContext* LoadedMapping);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputMappingContext> DLContext;

    UPROPERTY()
    TObjectPtr<UCharacterControlComponent> CharacterControl;
};
