// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DLCharacterBase.h"
#include "DLPlayerCharacter.generated.h"

class USpringArmComponent;
class UDLCameraComponent;
class UDLEquipmentManagerComponent;
class UDLEquipManagerComponent;
class ADLPlayerController;

/**
 * 
 */
UCLASS()
class DUALLINKER_API ADLPlayerCharacter : public ADLCharacterBase
{
	GENERATED_BODY()
public:
    ADLPlayerCharacter(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, Category = "DL|Character")
    ADLPlayerController* GetDLPlayerController() const;

    virtual void Move(const FVector2D& InputVector);
    virtual void Look(const FVector2D& InputVector);
protected:
    virtual void BeginPlay() override;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float WalkSpeed = 400.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float SprintSpeed = 800.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    TObjectPtr<UDLCameraComponent> CameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
    TObjectPtr<UDLEquipmentManagerComponent> EquipmentManagerComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
    TObjectPtr<UDLEquipManagerComponent> EquipManagerComponent;

public:
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    int32 DefaultWeaponID = 1001;
};
