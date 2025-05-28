// Fill out your copyright notice in the Description page of Project Settings.


#include "DLPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "DualLinker/Character/DLPawnData.h"
#include "DualLinker/Input/DLInputComponent.h"
#include "DualLinker/Input/DLInputConfig.h"
#include "DualLinker/DLGameplayTags.h"
#include "DualLinker/System/DLInputMappingSubsystem.h"
#include "DualLinker/Character/DLPlayerCharacter.h"
#include "DualLinker/Camera/DLPlayerCameraManager.h"
#include "DualLinker/Camera/DLCameraComponent.h"
#include "DualLinker/Camera/DLCameraMode.h"
#include "DualLinker/AbilitySystem/DLAbilitySystemComponent.h"
#include "DLPlayerState.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "DualLinker/Animation/DLAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

ADLPlayerController::ADLPlayerController()
{
    PlayerCameraManagerClass = ADLPlayerCameraManager::StaticClass();
}

void ADLPlayerController::ReceivePawnData(const TArray<TSoftObjectPtr<UDLPawnData>> InPawnDataList)
{
    if (!(InPawnDataList.Num() > 0)) return;

    if (ADLPlayerState* PS = GetPlayerState<ADLPlayerState>())
    { 
        PS->PawnData = InPawnDataList[(int32)PS->CurrentCharacterType].Get();
        if (PS->PawnData)
        {
            // 1) 입력 바인딩
            if (PS->PawnData->InputConfig)
            {
                BindInputActions(PS->PawnData->InputConfig);
            }

            // 2) 카메라 모드 설정
            if (PS->PawnData->DefaultCameraMode)
            {
                SetupDefaultCameraMode(PS->PawnData->DefaultCameraMode);
            }

            if (PS->PawnData->AbilitySets.Num() > 0)
            {
                // 3) AbilitySet 부여 (PlayerState → ASC)
                PS->ApplyAbilitySets(PS->PawnData->AbilitySets);
            }
        }
    }
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

void ADLPlayerController::OnPossess(APawn* NewPawn)
{
    Super::OnPossess(NewPawn);

    if (UAbilitySystemComponent* ASC = Cast<IAbilitySystemInterface>(NewPawn)->GetAbilitySystemComponent())
    {
        ASC->InitAbilityActorInfo(
            GetPlayerState<ADLPlayerState>(),
            NewPawn
        );   
        if (ACharacter* PlayerCharacter = Cast<ACharacter>(NewPawn))
        {
            if (UDLAnimInstance* DLAnimInstance = Cast<UDLAnimInstance>(PlayerCharacter->GetMesh()->GetAnimInstance()))
            {
                DLAnimInstance->InitializeWithAbilitySystem(ASC);
            }
        }
    }  
}

void ADLPlayerController::BindInputActions(UDLInputConfig* InInputConfig)
{
    UDLInputComponent* DLInputComponent = CastChecked<UDLInputComponent>(InputComponent);
    const FDLGameplayTags& GameplayTags = FDLGameplayTags::Get();

    DLInputComponent->BindNativeAction(InInputConfig, GameplayTags.InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, false);
    DLInputComponent->BindNativeAction(InInputConfig, GameplayTags.InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, false);
    DLInputComponent->BindNativeAction(InInputConfig, GameplayTags.InputTag_ChangeEquip_Primary, ETriggerEvent::Triggered, this, &ThisClass::Input_ChangeEquip_Weapon_Primary, false);
    DLInputComponent->BindNativeAction(InInputConfig, GameplayTags.InputTag_ChangeEquip_Secondary, ETriggerEvent::Triggered, this, &ThisClass::Input_ChangeEquip_Weapon_Secondary, false);

}

void ADLPlayerController::UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents)
{
    // OutHiddenComponents는 렌더링 엔진이 무시할 컴포넌트들
    Super::UpdateHiddenComponents(ViewLocation, OutHiddenComponents);

    if (bHideViewTargetPawnNextFrame)
    {
        AActor* const ViewTargetPawn = PlayerCameraManager ? Cast<AActor>(PlayerCameraManager->GetViewTarget()) : nullptr;
        if (ViewTargetPawn)
        {
            auto AddToHiddenComponents = [&OutHiddenComponents](const TInlineComponentArray<UPrimitiveComponent*>& InComponents)
                {
                    for (UPrimitiveComponent* Comp : InComponents)
                    {
                        // 컴포넌트가 월드에 등록되어 있어야 렌더링 정보 존재
                        if (Comp->IsRegistered())
                        {
                            // 렌더링 시스템에서 식별하는 ID
                            OutHiddenComponents.Add(Comp->GetPrimitiveSceneId());

                            for (USceneComponent* AttachedChild : Comp->GetAttachChildren())
                            {
                                static FName NAME_NoParentAutoHide(TEXT("NoParentAutoHide"));
                                UPrimitiveComponent* AttachChildPC = Cast<UPrimitiveComponent>(AttachedChild);
                                if (AttachChildPC && AttachChildPC->IsRegistered() && !AttachChildPC->ComponentTags.Contains(NAME_NoParentAutoHide))
                                {
                                    OutHiddenComponents.Add(AttachChildPC->GetPrimitiveSceneId());
                                }
                            }
                        }
                    }
                };

            //TODO 모든 컴포넌트를 숨기지 않고 일부만 숨기고 싶을 경우 이 처리를 인터페이스 기반으로 위임
            //TODO 거리 기반 투명도 조절 등으로 좀 더 부드럽게 처리

            // 폰의 컴포넌트들을 hidden 로직
            TInlineComponentArray<UPrimitiveComponent*> PawnComponents;
            ViewTargetPawn->GetComponents(PawnComponents);
            AddToHiddenComponents(PawnComponents);

            //// 무기 숨기기
            //if (ViewTargetPawn->CurrentWeapon)
            //{
            //	TInlineComponentArray<UPrimitiveComponent*> WeaponComponents;
            //	ViewTargetPawn->CurrentWeapon->GetComponents(WeaponComponents);
            //	AddToHiddenComponents(WeaponComponents);
            //}
        }

        bHideViewTargetPawnNextFrame = false;
    }
}

void ADLPlayerController::OnCameraPenetratingTarget()
{
    // TODO: Hidden 로직
    bHideViewTargetPawnNextFrame = true;
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

void ADLPlayerController::Input_ChangeEquip_Weapon_Primary()
{
    FGameplayEventData Payload;
    Payload.EventMagnitude = (int32)EEquipState::Weapon_Primary;
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), FDLGameplayTags::Get().GameplayEvent_ChangeEquip, Payload);
}

void ADLPlayerController::Input_ChangeEquip_Weapon_Secondary()
{
    FGameplayEventData Payload;
    Payload.EventMagnitude = (int32)EEquipState::Weapon_Secondary;
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetPawn(), FDLGameplayTags::Get().GameplayEvent_ChangeEquip, Payload);
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