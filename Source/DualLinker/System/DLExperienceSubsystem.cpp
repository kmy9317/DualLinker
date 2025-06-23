// Fill out your copyright notice in the Description page of Project Settings.


#include "DLExperienceSubsystem.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "DualLinker/GameModes/DLExperienceDefinition.h"
#include "DualLinker/Character/DLPawnData.h"

void UDLExperienceSubsystem::CallOrRegister_OnExperienceLoaded(FOnExperienceLoaded::FDelegate&& Delegate)
{
    if (IsExperienceLoaded())
    {
        Delegate.Execute(CurrentExperience);
    }
    else
    {
        OnExperienceLoaded.Add(MoveTemp(Delegate));
    }
}

void UDLExperienceSubsystem::LoadExperience(TSoftObjectPtr<UDLExperienceDefinition> NewExperienceDefinition)
{
    if (UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
    {
        FStreamableManager& Streamable = AssetManager->GetStreamableManager();

        // 기존 에셋을 언로드
        UnloadPreviousExperienceAssets();
        PreviouslyLoadedAssets.Empty();

        FSoftObjectPath NewExperiencePath = NewExperienceDefinition.ToSoftObjectPath();
        TSubclassOf<UDLExperienceDefinition> AssetClass = Cast<UClass>(NewExperiencePath.TryLoad());

        CurrentExperience = AssetClass->GetDefaultObject<UDLExperienceDefinition>();
        //check(CurrentExperience);

        PreviouslyLoadedAssets.Add(NewExperiencePath);
        TArray<FSoftObjectPath> AssetsToLoad;

        for (const auto& PawnData : CurrentExperience->PawnDataList)
        {    
            FSoftObjectPath AssetPath = PawnData.ToSoftObjectPath();
            AssetsToLoad.Add(AssetPath);
            PreviouslyLoadedAssets.Add(AssetPath);
        }

        if (AssetsToLoad.Num() > 0)
        {
            TWeakObjectPtr<UDLExperienceSubsystem> WeakThis(this);

            Streamable.RequestAsyncLoad(AssetsToLoad, [WeakThis]()
                {
                    if (WeakThis.IsValid())
                    {
                        UDLExperienceSubsystem* Subsystem = WeakThis.Get();
                        // Experience 로드 완료 이벤트 호출
                        Subsystem->OnAssetLoaded();
                    }
                });
        }
        else
        {
            // 로드할 추가 에셋이 없으면 즉시 완료 처리
            OnAssetLoaded();
        }
    }
}

void UDLExperienceSubsystem::UnloadPreviousExperienceAssets()
{
    if (UAssetManager* AssetManager = UAssetManager::GetIfInitialized())
    {
        FStreamableManager& Streamable = AssetManager->GetStreamableManager();

        for (const FSoftObjectPath& AssetPath : PreviouslyLoadedAssets)
        {
            Streamable.Unload(AssetPath);
        }

        PreviouslyLoadedAssets.Empty();
        UE_LOG(LogTemp, Log, TEXT("Unloaded previous experience assets."));
    }
}

void UDLExperienceSubsystem::OnAssetLoaded()
{
    bIsExperienceLoaded = true;
    OnExperienceLoaded.Broadcast(CurrentExperience);

}

bool UDLExperienceSubsystem::IsExperienceLoaded() const
{
    return bIsExperienceLoaded && CurrentExperience != nullptr;
}

const UDLExperienceDefinition* UDLExperienceSubsystem::GetCurrentExperienceChecked() const
{
    if (bIsExperienceLoaded && CurrentExperience != nullptr) return CurrentExperience;
    return nullptr;
}

void UDLExperienceSubsystem::ResetLoadingState()
{
    bIsExperienceLoaded = false;
    CurrentExperience = nullptr;
}

bool UDLExperienceSubsystem::ShouldShowLoadingScreen() const
{
    return !IsExperienceLoaded();
}