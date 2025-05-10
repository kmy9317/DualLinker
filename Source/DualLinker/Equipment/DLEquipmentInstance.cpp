// Fill out your copyright notice in the Description page of Project Settings.


#include "DLEquipmentInstance.h"
#include "DLEquipmentDefinition.h"
#include "GameFramework/Character.h"

UDLEquipmentInstance::UDLEquipmentInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

APawn* UDLEquipmentInstance::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

void UDLEquipmentInstance::SpawnEquipmentActors(const TArray<FDLEquipmentActorToSpawn>& ActorsToSpawn)
{
	if (APawn* OwningPawn = GetPawn())
	{
		// 현재 Owner인 Pawn의 RootComponent를 AttachTarget 대상으로 한다
		USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
		if (ACharacter* Char = Cast<ACharacter>(OwningPawn))
		{
			// 만약 캐릭터라면, SkeletalMeshComponent가 있으면 GetMesh로 반환하여, 여기에 붙인다
			AttachTarget = Char->GetMesh();
		}

		// 한개의 장비에 여러가지 장착물이 필요한 경우를 고려
		for (const FDLEquipmentActorToSpawn& SpawnInfo : ActorsToSpawn)
		{
			AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, FTransform::Identity, OwningPawn);
			NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/true);
			NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);

			SpawnedActors.Add(NewActor);
		}
	}
}

void UDLEquipmentInstance::DestroyEquipmentActors()
{
	for (AActor* Actor : SpawnedActors)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}
}

APawn* UDLEquipmentInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const
{
	APawn* Result = nullptr;
	if (UClass* ActualPawnType = PawnType)
	{
		if (GetOuter()->IsA(ActualPawnType))
		{
			Result = Cast<APawn>(GetOuter());
		}
	}
	return Result;
}

void UDLEquipmentInstance::OnEquipped()
{
	K2_OnEquipped();
}

void UDLEquipmentInstance::OnUnequipped()
{
	K2_OnUnequipped();
}
