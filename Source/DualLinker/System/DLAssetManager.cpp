// Fill out your copyright notice in the Description page of Project Settings.

#include "DLAssetManager.h"
#include "Logging/LogMacros.h"
#include "DualLinker/DLGameplayTags.h"
#include "DualLinker/Data/DLCharacterPartsData.h"

UDLAssetManager& UDLAssetManager::Get()
{
    check(GEngine);
    UDLAssetManager* DLAssetManager = Cast<UDLAssetManager>(GEngine->AssetManager);
    return *DLAssetManager;
}

void UDLAssetManager::StartInitialLoading()
{
    Super::StartInitialLoading();

	GetOrLoadTypedGameData<UDLCharacterPartsData>(CharacterDataPath);
    FDLGameplayTags::InitializeNativeGameplayTags();
}

UPrimaryDataAsset* UDLAssetManager::LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType)
{
	UPrimaryDataAsset* Asset = nullptr;

	if (!DataClassPath.IsNull())
	{
		if (GIsEditor)
		{
			Asset = DataClassPath.LoadSynchronous();
			LoadPrimaryAssetsWithType(PrimaryAssetType);
		}
		else
		{
			TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);
			if (Handle.IsValid())
			{
				Handle->WaitUntilComplete(0.0f, false);

				Asset = Cast<UPrimaryDataAsset>(Handle->GetLoadedAsset());
			}
		}
	}

	if (Asset)
	{
		GameDataMap.Add(DataClass, Asset);
	}
	else
	{
		// It is not acceptable to fail to load any GameData asset. It will result in soft failures that are hard to diagnose.
		UE_LOG(LogTemp, Fatal, TEXT("Failed to load GameData asset at %s. Type %s. This is not recoverable and likely means you do not have the correct data to run %s."), *DataClassPath.ToString(), *PrimaryAssetType.ToString(), FApp::GetProjectName());
	}

	return Asset;
}

const UDLCharacterPartsData& UDLAssetManager::GetDefaultCharacterPartsData()
{
    return GetOrLoadTypedGameData<UDLCharacterPartsData>(CharacterDataPath);
}
