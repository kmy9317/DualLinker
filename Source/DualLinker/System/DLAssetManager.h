// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "DLAssetManager.generated.h"

class UDLCharacterPartsData;
class UDLItemData;

/**
 * 
 */
UCLASS(Config = Game)
class DUALLINKER_API UDLAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
    static UDLAssetManager& Get();

	template<typename AssetType>
	static AssetType* GetAssetByPath(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	const UDLCharacterPartsData& GetDefaultCharacterPartsData();
	const UDLItemData& GetItemData();

protected:
	virtual void StartInitialLoading() override;

	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);

	// Thread safe way of adding a loaded asset to keep in memory.
	void AddLoadedAsset(const UObject* Asset);

	template<typename GameDataClass>
	const GameDataClass& GetOrLoadTypedGameData(const TSoftObjectPtr<GameDataClass>& DataPath)
	{
		if (TObjectPtr<UPrimaryDataAsset> const* pResult = GameDataMap.Find(GameDataClass::StaticClass()))
		{
			return *CastChecked<GameDataClass>(*pResult);
		}

		// Does a blocking load if needed
		return *CastChecked<const GameDataClass>(LoadGameDataOfClass(GameDataClass::StaticClass(), DataPath, GameDataClass::StaticClass()->GetFName()));
	}

	UPrimaryDataAsset* LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType);

protected:

	UPROPERTY(Transient)
	TMap<TObjectPtr<UClass>, TObjectPtr<UPrimaryDataAsset>> GameDataMap;

	UPROPERTY(Config)
	TSoftObjectPtr<UDLCharacterPartsData> CharacterDataPath;

	UPROPERTY(Config)
	TSoftObjectPtr<UDLItemData> ItemDataPath;

private:
	// Assets loaded and tracked by the asset manager.
	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	// Used for a scope lock when modifying the list of load assets.
	FCriticalSection LoadedAssetsCritical;
};

template<typename AssetType>
AssetType* UDLAssetManager::GetAssetByPath(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	AssetType* LoadedAsset = nullptr;

	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

	if (AssetPath.IsValid())
	{
		LoadedAsset = AssetPointer.Get();
		if (!LoadedAsset)
		{
			LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
			ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());
		}

		if (LoadedAsset && bKeepInMemory)
		{
			// Added to loaded asset list.
			Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
		}
	}

	return LoadedAsset;
}
