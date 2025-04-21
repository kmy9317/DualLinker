// Fill out your copyright notice in the Description page of Project Settings.
#include "DLCharacterPartsManager.h"

#include "Components/SkeletalMeshComponent.h"
#include "DualLinker/Data/DLCharacterPartsData.h" 
#include "DualLinker/Character/DLCharacterBase.h"
#include "DualLinker/DLDefine.h"

UDLCharacterPartsManager::UDLCharacterPartsManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDLCharacterPartsManager::BeginPlay()
{
	Super::BeginPlay();
	InitializeParts();
}

void UDLCharacterPartsManager::InitializeParts()
{
	// TODO: 기본 Hero캐릭터 외 파츠 초기화 담당
}

USkeletalMeshComponent* UDLCharacterPartsManager::GetBaseMesh() const
{
	return GetOwner() ? GetOwner()->FindComponentByClass<USkeletalMeshComponent>() : nullptr;
}

USkeletalMeshComponent* UDLCharacterPartsManager::GetOrCreateMeshComponent(FName PartID)
{
	if (USkeletalMeshComponent** Found = MeshPartsMap.Find(PartID))
	{
		return *Found;
	}

	USkeletalMeshComponent* Base = GetBaseMesh();
	if (!Base) return nullptr;

	// 새 서브메쉬 컴포넌트 생성
	auto* NewComp = NewObject<USkeletalMeshComponent>(GetOwner(), USkeletalMeshComponent::StaticClass(), PartID);
	NewComp->AttachToComponent(Base, FAttachmentTransformRules::KeepRelativeTransform);
	NewComp->SetRelativeLocationAndRotation(FVector::Zero(), FRotator::ZeroRotator.Quaternion());
	NewComp->SetLeaderPoseComponent(Base);
	NewComp->RegisterComponent();
	MeshPartsMap.Add(PartID, NewComp);
	return NewComp;
}

void UDLCharacterPartsManager::ApplyMeshPart(const FDLCharacterMeshPart& MeshPart, ECharacterType CharType)
{
    const FString Prefix = UEnum::GetValueAsString(CharType).RightChop(12);

    struct FMeshCategory
    {
        const TArray<FDLCharacterMeshSlot>& Slots;
        FString Suffix;
    };

    const TArray<FMeshCategory> Categories =
    {
        { MeshPart.HairMeshes,    TEXT("Hair")    },
        { MeshPart.HelmetMeshes,    TEXT("Helmet")    },
        { MeshPart.BodyMeshes,    TEXT("Body")    },
        { MeshPart.DefaultMeshes, TEXT("")        }
    };

    // 3) 공통 로직 한 번만 작성
    for (const FMeshCategory& Cat : Categories)
    {
        const TArray<FDLCharacterMeshSlot>& Slots = Cat.Slots;
        const FString& Suf = Cat.Suffix;

        for (int32 i = 0; i < Slots.Num(); ++i)
        {
            // PartID 예시: "Edel_Hair_0", "Edel_0" 등
            FName PartID;
            if (Suf.IsEmpty())
            {
                PartID = FName(*FString::Printf(TEXT("%s_%d"), *Prefix, i));
            }
            else
            {
                PartID = FName(*FString::Printf(TEXT("%s_%s_%d"), *Prefix, *Suf, i));
            }

            if (USkeletalMeshComponent* Comp = GetOrCreateMeshComponent(PartID))
            {
                const FDLCharacterMeshSlot& Slot = Slots[i];

                // Mesh 세팅
                if (Slot.Mesh.IsValid())
                {
                    Comp->SetSkeletalMesh(Slot.Mesh.Get());
                }
                else if (USkeletalMesh* LoadedMesh = Slot.Mesh.LoadSynchronous())
                {
                    Comp->SetSkeletalMesh(LoadedMesh);
                }

                // Material 세팅
                for (int32 MatIdx = 0; MatIdx < Slot.Materials.Num(); ++MatIdx)
                {
                    if (Slot.Materials[MatIdx].IsValid())
                    {
                        Comp->SetMaterial(MatIdx, Slot.Materials[MatIdx].Get());
                    }
                    else if (UMaterialInterface* LoadedMaterial = Slot.Materials[MatIdx].LoadSynchronous())
                    {
                        Comp->SetMaterial(MatIdx, LoadedMaterial);
                    }
                }
            }
        }
    }
}

