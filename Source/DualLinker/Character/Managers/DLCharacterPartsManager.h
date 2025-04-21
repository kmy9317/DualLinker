// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DualLinker/DLDefine.h"

#include "DLCharacterPartsManager.generated.h"

class USkeletalMeshComponent;
class UDLCharacterPartsData;

struct FDLCharacterMeshPart;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUALLINKER_API UDLCharacterPartsManager : public UActorComponent
{
	GENERATED_BODY()

	friend class ADLCharacterBase;
public:
	UDLCharacterPartsManager();

	virtual void BeginPlay() override;

protected:
	/** 실제 파츠 부착 진행 초기화 (서브클래스에서 override) */
	virtual void InitializeParts();

	/** 주어진 MeshPart 를 BaseMesh 부착 */
	void ApplyMeshPart(const FDLCharacterMeshPart& MeshPart, ECharacterType CharType);

	/** Base 의 SkeletalMeshComponent */
	USkeletalMeshComponent* GetBaseMesh() const;

	/** PartID(FName)로 부품 컴포넌트를 찾아내거나 새로 생성. */
	USkeletalMeshComponent* GetOrCreateMeshComponent(FName PartID);

protected:
	ECharacterType OwnerCharacterType;

private:
	/** PartID → Sub?mesh 컴포넌트 맵 */
	UPROPERTY()
	TMap<FName, USkeletalMeshComponent*> MeshPartsMap;
	
};
