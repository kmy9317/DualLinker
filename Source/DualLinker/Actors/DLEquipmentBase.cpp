#include "DLEquipmentBase.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "DualLinker/System/DLAssetManager.h"
#include "DualLinker/Item/Fragments/DLItemFragment_Equippable_Weapon.h"
#include "DualLinker/Equipment/DLEquipManagerComponent.h"
#include "DualLinker/AbilitySystem/DLAbilitySystemComponent.h"
#include "DualLinker/Character/DLPlayerCharacter.h"
#include "DualLinker/Data/DLItemData.h"
#include "Kismet/KismetMathLibrary.h"

ADLEquipmentBase::ADLEquipmentBase()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>("ArrowComponent");
	ArrowComponent->PrimaryComponentTick.bStartWithTickEnabled = false;
	SetRootComponent(ArrowComponent);
	
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	//MeshComponent->SetCollisionProfileName("Weapon");
	MeshComponent->SetGenerateOverlapEvents(false);
	MeshComponent->SetupAttachment(GetRootComponent());
	MeshComponent->PrimaryComponentTick.bStartWithTickEnabled = false;
	MeshComponent->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	
	TraceDebugCollision = CreateDefaultSubobject<UBoxComponent>("TraceDebugCollision");
	TraceDebugCollision->SetCollisionProfileName("NoCollision");
	TraceDebugCollision->SetGenerateOverlapEvents(false);
	TraceDebugCollision->SetupAttachment(GetRootComponent());
	TraceDebugCollision->PrimaryComponentTick.bStartWithTickEnabled = false;
}

void ADLEquipmentBase::Destroyed()
{
	if (ADLPlayerCharacter* DLPlayerCharacter = Cast<ADLPlayerCharacter>(GetOwner()))
	{	
		if (UDLEquipManagerComponent* EquipManager = DLPlayerCharacter->FindComponentByClass<UDLEquipManagerComponent>())
		{
			TArray<FDLEquipEntry>& Entries = EquipManager->GetAllEntries();
			if (Entries[(int32)EquipmentSlotType].GetEquipmentActor() == this)
			{
				Entries[(int32)EquipmentSlotType].SetEquipmentActor(nullptr);
			}
		}
		if (USkeletalMeshComponent* CharacterMeshComponent = DLPlayerCharacter->GetMesh())
		{
			const UDLItemTemplate& ItemTemplate = UDLItemData::Get().FindItemTemplateByID(ItemTemplateID);
			const UDLItemFragment_Equippable_Attachment* AttachmentFragment = ItemTemplate.FindFragmentByClass<UDLItemFragment_Equippable_Attachment>();
			// 해당 부착 장비의 AnimLayer를 Unlink
			if (AttachmentFragment && AttachmentFragment->AnimLayerClass)
			{
				CharacterMeshComponent->UnlinkAnimClassLayers(AttachmentFragment->AnimLayerClass);
			}
		}
	}
	Super::Destroyed();
}

void ADLEquipmentBase::Init(int32 InTemplateID, EEquipmentSlotType InEquipmentSlotType)
{	
	// 장비 Spawn시 초기화 담당 코드
	ItemTemplateID = InTemplateID;
	EquipmentSlotType = InEquipmentSlotType;
	PropertyInitialization();
}

void ADLEquipmentBase::ChangeBlockState(bool bShouldBlock)
{	
	bCanBlock = bShouldBlock;
	SetBlockingCollision();
}

void ADLEquipmentBase::SetBlockingCollision()
{
	// MeshComponent->SetCollisionResponseToChannel(D1_ObjectChannel_Projectile, bCanBlock ? ECR_Block : ECR_Ignore);
}

UAbilitySystemComponent* ADLEquipmentBase::GetAbilitySystemComponent() const
{
	if (ADLPlayerCharacter* DLPlayerCharacter = Cast<ADLPlayerCharacter>(GetOwner()))
	{
		return DLPlayerCharacter->GetAbilitySystemComponent();
	}
	return nullptr;
}

void ADLEquipmentBase::ProcessEquip_Implementation(UDLItemInstance* ItemInstance)
{
	ADLPlayerCharacter* DLPlayerCharacter = Cast<ADLPlayerCharacter>(GetOwner());
	if (DLPlayerCharacter == nullptr)
		return;
	
	check(ItemTemplateID > 0);
	
	const UDLItemTemplate& ItemTemplate = UDLItemData::Get().FindItemTemplateByID(ItemTemplateID);
	const UDLItemFragment_Equippable_Attachment* AttachmentFragment = ItemTemplate.FindFragmentByClass<UDLItemFragment_Equippable_Attachment>();
	if (AttachmentFragment == nullptr)
		return;

	if (USkeletalMeshComponent* CharacterMeshComponent = DLPlayerCharacter->GetMesh())
	{
		// 해당 부착 장비에 맞는 AnimLayer를 연결
		if (AttachmentFragment->AnimLayerClass)
		{
			CharacterMeshComponent->LinkAnimClassLayers(AttachmentFragment->AnimLayerClass);
		}
		PlayEquipMontage();	
	}
}

void ADLEquipmentBase::PlayEquipMontage()
{
	ADLPlayerCharacter* DLPlayerCharacter = Cast<ADLPlayerCharacter>(GetOwner());
	if (DLPlayerCharacter == nullptr)
		return;

	USkeletalMeshComponent* CharacterMeshComponent = DLPlayerCharacter->GetMesh();
	if (CharacterMeshComponent == nullptr)
		return;
	
	UAnimMontage* EquipMontage = UDLAssetManager::GetAssetByPath<UAnimMontage>(GetEquipMontage());
	if (UAnimInstance* AnimInstance = CharacterMeshComponent->GetAnimInstance())
	{
		if (AnimInstance->GetCurrentActiveMontage() != EquipMontage)
		{
			DLPlayerCharacter->PlayAnimMontage(EquipMontage);
		}
	}
}

void ADLEquipmentBase::PropertyInitialization()
{
	if (bInitialized) return;
	if (ItemTemplateID <= 0 || EquipmentSlotType == EEquipmentSlotType::Count) return;
	
	if (GetOwner())
	{
		UDLEquipManagerComponent* EquipManager = GetOwner()->FindComponentByClass<UDLEquipManagerComponent>();
		UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
		if (EquipManager && ASC)
		{
			if (UDLItemInstance* ItemInstance = EquipManager->GetEquippedItemInstance(EquipmentSlotType))
			{
				TArray<FDLEquipEntry>& Entries = EquipManager->GetAllEntries();
				Entries[(int32)EquipmentSlotType].SetEquipmentActor(this);
			
				// 장비 장착에 대한 후처리 코드로써 c++코드 외에 블루프린트에서 추가 로직 작성 가능
				ProcessEquip(ItemInstance);
				bInitialized = true;
				return;
			}
		}
	}
}

TSoftObjectPtr<UAnimMontage> ADLEquipmentBase::GetEquipMontage() const
{
	if (ItemTemplateID <= 0)
		return nullptr;
	
	// 할당받은 ItemTemplateID에 해당하는 템플릿의 Attachemnt Fragment내 EquipMontage정보를 가져오도록 함
	const UDLItemTemplate& ItemTemplate = UDLItemData::Get().FindItemTemplateByID(ItemTemplateID);
	const UDLItemFragment_Equippable_Attachment* AttachmentFragment = ItemTemplate.FindFragmentByClass<UDLItemFragment_Equippable_Attachment>();
	if (AttachmentFragment == nullptr)
		return nullptr;

	return AttachmentFragment->EquipMontage;
}

UAnimMontage* ADLEquipmentBase::GetHitMontage(AActor* InstigatorActor, const FVector& HitLocation, bool IsBlocked)
{
	UAnimMontage* SelectedMontage = nullptr;
	
	if (InstigatorActor && ItemTemplateID > 0)
	{
		const UDLItemTemplate& ItemTemplate = UDLItemData::Get().FindItemTemplateByID(ItemTemplateID);
		if (const UDLItemFragment_Equippable_Attachment* AttachmentFragment = ItemTemplate.FindFragmentByClass<UDLItemFragment_Equippable_Attachment>())
		{
			TSoftObjectPtr<UAnimMontage> SelectedMontagePath = nullptr;
			
			// block가능한 장비로 block 성공시의 Montage
			if (IsBlocked)
			{
				SelectedMontagePath = AttachmentFragment->BlockHitMontage;
			}
			// Instigator와 장비 Owner와의 방향에 따라 HitMontage 결정
			else
			{
				AActor* CharacterActor = GetOwner();
				FVector CharacterLocation = CharacterActor->GetActorLocation();
				FVector CharacterDirection = CharacterActor->GetActorForwardVector();
			
				FRotator FacingRotator = UKismetMathLibrary::Conv_VectorToRotator(CharacterDirection);
				FRotator CharacterToHitRotator = UKismetMathLibrary::Conv_VectorToRotator((HitLocation - CharacterLocation).GetSafeNormal());
			
				FRotator DeltaRotator = UKismetMathLibrary::NormalizedDeltaRotator(CharacterToHitRotator, FacingRotator);
				float YawAbs = FMath::Abs(DeltaRotator.Yaw);
				
				if (YawAbs < 60.f)
				{
					SelectedMontagePath = AttachmentFragment->FrontHitMontage;
				}
				else if (YawAbs > 120.f)
				{
					SelectedMontagePath = AttachmentFragment->BackHitMontage;
				}
				else if (DeltaRotator.Yaw < 0.f)
				{
					SelectedMontagePath = AttachmentFragment->LeftHitMontage;
				}
				else
				{
					SelectedMontagePath = AttachmentFragment->RightHitMontage;
				}
			}

			if (SelectedMontagePath.IsNull() == false)
			{
				SelectedMontage = UDLAssetManager::GetAssetByPath<UAnimMontage>(SelectedMontagePath);
			}
		}
	}
	
	return SelectedMontage;
}
