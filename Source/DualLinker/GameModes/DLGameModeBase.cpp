// Fill out your copyright notice in the Description page of Project Settings.


#include "DLGameModeBase.h"
#include "DualLinker/Player/DLPlayerController.h"
#include "DualLinker/Character/DLEdelCharacter.h"
#include "DualLinker/System/DLExperienceSubsystem.h"
#include "DualLinker/Player/DLPlayerState.h"
#include "DLExperienceDefinition.h"
#include "DualLinker/Character/DLPawnData.h"
#include "DualLinker/Input/DLInputConfig.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/AssetManager.h"

ADLGameModeBase::ADLGameModeBase()
{
	PlayerControllerClass = ADLPlayerController::StaticClass();
	DefaultPawnClass = ADLEdelCharacter::StaticClass();
    PlayerStateClass = ADLPlayerState::StaticClass();
}

void ADLGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);

    GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::RequestLoadingExperience);
}

void ADLGameModeBase::InitGameState()
{
    Super::InitGameState();

    if (UDLExperienceSubsystem * ExperienceSubsystem = GetGameInstance()->GetSubsystem<UDLExperienceSubsystem>())
    {
        ExperienceSubsystem->CallOrRegister_OnExperienceLoaded(FOnExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
    }
}

UClass* ADLGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
    if (const UDLPawnData* PawnData = GetPawnDataForController(InController))
    {
        if (PawnData->PawnClass)
        {
            return PawnData->PawnClass;
        }
    }

    return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void ADLGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
    if (IsExperienceLoaded())
    {
        // 내부적으로 RestartPlayer가 실행됨.
        Super::HandleStartingNewPlayer_Implementation(NewPlayer);
    }
}

APawn* ADLGameModeBase::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
    FActorSpawnParameters SpawnInfo;
    SpawnInfo.Instigator = GetInstigator();
    SpawnInfo.ObjectFlags |= RF_Transient;
    SpawnInfo.bDeferConstruction = true;

    if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
    {
        if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
        {           
            SpawnedPawn->FinishSpawning(SpawnTransform);
            return SpawnedPawn;
        }
    }

    return nullptr;
}

void ADLGameModeBase::RequestLoadingExperience()
{
    // GameInstanceSubsystem에서 ExperienceSubsystem 가져오기
    UDLExperienceSubsystem* ExperienceSubsystem = GetGameInstance()->GetSubsystem<UDLExperienceSubsystem>();

    if (ExperienceSubsystem)
    {
        // Asset Manager에서 UDLExperienceDefinition 검색 및 로드
        UAssetManager& AssetManager = UAssetManager::Get();
        TArray<FPrimaryAssetId> ExperienceAssets;

        // 등록된 PrimaryAssetType을 기반으로 ExperienceDefinitions 검색
        AssetManager.GetPrimaryAssetIdList(FPrimaryAssetType("DLExperienceDefinition"), ExperienceAssets);

        if (ExperienceAssets.Num() > 0)
        {
            // 첫 번째 ExperienceDefinition을 로드
            FPrimaryAssetId SelectedAsset = ExperienceAssets[0];
            FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(SelectedAsset);

            ExperienceSubsystem->LoadExperience(TSoftObjectPtr<UDLExperienceDefinition>(AssetPath));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("No ExperienceDefinition found in AssetManager!"));
        }
    }
}

bool ADLGameModeBase::IsExperienceLoaded() const
{
    if (UDLExperienceSubsystem* ExperienceSubsystem = GetGameInstance()->GetSubsystem<UDLExperienceSubsystem>())
        return ExperienceSubsystem->IsExperienceLoaded();

    return false;
}

void ADLGameModeBase::BeginPlay()
{
    Super::BeginPlay();
}

void ADLGameModeBase::OnExperienceLoaded(const UDLExperienceDefinition* CurrentExperience)
{
    UE_LOG(LogTemp, Log, TEXT("Experience fully loaded, starting game."));

    if (CurrentExperience && GetWorld())
    {
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (ADLPlayerController* DLPC = Cast<ADLPlayerController>(PC))
        {          
            if (DLPC->GetPawn() == nullptr)
            {
                if (PlayerCanRestart(DLPC))
                {
                    RestartPlayer(DLPC);
                }
            }
            // PawnDataList에서 첫 번째 요소 가져오기
            if (CurrentExperience->PawnDataList.Num() > 0 && CurrentExperience->PawnDataList[0].IsValid())
            {
                UDLPawnData* PawnData = CurrentExperience->PawnDataList[0].LoadSynchronous();
                if (PawnData && PawnData->InputConfig)
                {
                    // PlayerController에서 입력 설정
                    DLPC->BindInputActions(PawnData->InputConfig);
                }
            }
        }
    }
}

const UDLPawnData* ADLGameModeBase::GetPawnDataForController(const AController* InController) const
{
    if (InController)
    {
        if (const ADLPlayerState* DLPS = InController->GetPlayerState<ADLPlayerState>())
        {
            if (const UDLPawnData* PawnData = DLPS->GetPawnData<UDLPawnData>())
            {
                return PawnData;
            }
        }
    }
    if (UDLExperienceSubsystem* ExperienceSubsystem = GetGameInstance()->GetSubsystem<UDLExperienceSubsystem>())
    {
        if (ExperienceSubsystem->IsExperienceLoaded())
        {
            if (const UDLExperienceDefinition* Experience = ExperienceSubsystem->GetCurrentExperienceChecked())
            {
                return (Experience->PawnDataList[0]).Get();
            }
        }
    }
    return nullptr;
}