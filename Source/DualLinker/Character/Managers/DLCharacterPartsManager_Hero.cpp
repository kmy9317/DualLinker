// Fill out your copyright notice in the Description page of Project Settings.


#include "DLCharacterPartsManager_Hero.h"
#include "DualLinker/System/DLAssetManager.h"
#include "DualLinker/Data/DLCharacterPartsData.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"

UDLCharacterPartsManager_Hero::UDLCharacterPartsManager_Hero()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDLCharacterPartsManager_Hero::InitializeParts()
{
	ECharacterType Type = OwnerCharacterType;
	if (Type == ECharacterType::Count)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid CharacterType on %s"), *GetOwner()->GetName());
		return;
	}

	// DLAssetManager 에서 기본 파츠 데이터 동기 로드
	const UDLCharacterPartsData& PartsData = UDLAssetManager::Get().GetDefaultCharacterPartsData();

	// 해당 타입의 FDLCharacterMeshPart 꺼내옴
	FDLCharacterMeshPart MeshPart;
	if (!PartsData.GetMeshPartForCharacter(Type, MeshPart))
	{
		UE_LOG(LogTemp, Warning, TEXT("No mesh part for type %d on %s"), (uint8)Type, *GetOwner()->GetName());
		return;
	}

	ApplyMeshPart(MeshPart, Type);
}
