// Fill out your copyright notice in the Description page of Project Settings.

#include "DLGameModeBase.h"
#include "DualLinker/Player/DLPlayerController.h"
#include "DualLinker/Character/DLEdelCharacter.h"
#include "DualLinker/System/DLExperienceSubsystem.h"
#include "DualLinker/Player/DLPlayerState.h"
#include "DLExperienceDefinition.h"
#include "DualLinker/Character/DLPawnData.h"
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
        ExperienceSubsystem->ResetLoadingState();
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
        FPrimaryAssetId ExperienceId;
        UAssetManager& AssetManager = UAssetManager::Get();

        // fall back to the default experience
        // 일단 기본 옵션으로 default하게 B_DLDefaultExperience로 설정하자
        if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
        {
            // Experience의 Value를 가져와서, PrimaryAssetId를 생성해준다. 이때, DLExperienceDefintion의 Class 이름을 사용한다
            const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));

            // 철자 오류 및 스캔이 안된 경우를 제외하고 제대로 된 값을 가져온다.
            // FPrimaryAssetType은 프로젝트 세팅에서 AssetBaseClass에 지정한 타입들에 대한 에셋들에 일종의 카테고리 이름이다.
            // FName인자는 이 카테고리 내 특정 에셋을 가져오기 위한 것.
            ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UDLExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromOptions));
        }
        if (!ExperienceId.IsValid())
        {
            ExperienceId = FPrimaryAssetId(FPrimaryAssetType("DLExperienceDefinition"), FName("B_DefaultExperience"));
        }       
        ExperienceSubsystem->LoadExperience(TSoftObjectPtr<UDLExperienceDefinition>(AssetManager.GetPrimaryAssetPath(ExperienceId)));
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
            DLPC->ReceivePawnData(CurrentExperience->PawnDataList);        
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