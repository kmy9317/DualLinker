// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DLCharacterPartsData.generated.h"

class USkeletalMesh;
class UMaterialInterface;

USTRUCT(BlueprintType)
struct FDLCharacterMeshSlot
{
    GENERATED_BODY()

    // 해당 슬롯의 기본 메쉬
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh Slot")
    TSoftObjectPtr<USkeletalMesh> Mesh;

    // 해당 메쉬에 적용할 머티리얼 배열 (필요시 여러 머티리얼을 적용 가능)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh Slot")
    TArray<TSoftObjectPtr<UMaterialInterface>> Materials;
};

USTRUCT(BlueprintType)
struct FDLCharacterMeshPart
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Mesh Part")
    TArray<FDLCharacterMeshSlot> HairMeshes;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Mesh Part")
    TArray<FDLCharacterMeshSlot> HelmetMeshes;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Mesh Part")
    TArray<FDLCharacterMeshSlot> BodyMeshes;

    // 나머지 정적 메쉬 옵션 배열
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Mesh Part")
    TArray<FDLCharacterMeshSlot> DefaultMeshes;
};

UCLASS()
class DUALLINKER_API UDLCharacterPartsData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
    UDLCharacterPartsData();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Appearance")
    TMap<ECharacterType, FDLCharacterMeshPart> PartsMap;

    UFUNCTION(BlueprintCallable, Category = "Character Appearance")
    bool GetMeshPartForCharacter(ECharacterType CharacterType, FDLCharacterMeshPart& OutMeshPart) const;
};
