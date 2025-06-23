// Fill out your copyright notice in the Description page of Project Settings.


#include "TravelManagerSubsystem.h"
#include "Engine/AssetManager.h"
#include "Engine/LocalPlayer.h"

FString UTravelManager_TravelRequest::GetMapName() const
{
	// AssetManager, MapID로 FAssetData를 반환하고. 이를 통해 PackageName으로 Map 경로를 반환한다. 
	FAssetData MapAssetData;

	// 에셋의 정보 데이터만 가져옴 (로딩x)
	if (UAssetManager::Get().GetPrimaryAssetData(MapID, MapAssetData))
	{
		return MapAssetData.PackageName.ToString();
	}
	return FString();
}

FString UTravelManager_TravelRequest::ConstructTravelURL() const
{
	FString CombinedExtraArgs;

	for (const auto& ExtraArg : ExtraArgs)
	{
		if (ExtraArg.Key.IsEmpty())
		{
			continue;
		}

		/**
		* ?를 separate로 복수개의 ExtraArgs를 추가함:
		* - Key 값 유무에 따라, =(assignment)를 통해 알맞은 cmdArgs를 생성
		*/
		if (ExtraArg.Value.IsEmpty())
		{
			CombinedExtraArgs += FString::Printf(TEXT("?%s"), *ExtraArg.Key);
		}
		else
		{
			CombinedExtraArgs += FString::Printf(TEXT("?%s=%s"), *ExtraArg.Key, *ExtraArg.Value);
		}
	}

	// Map 경로 앞에 추가하여, 최종 TravelURL 생성
	return FString::Printf(TEXT("%s%s"), *GetMapName(), *CombinedExtraArgs);
}

void UTravelManagerSubsystem::RequestTravel(APlayerController* InPlayer, UTravelManager_TravelRequest* Request)
{
	ULocalPlayer* LocalPlayer = (InPlayer != nullptr) ? InPlayer->GetLocalPlayer() : nullptr;
	if (!LocalPlayer)
	{
		return;
	}

	// TravelRequest에서 MapID와 ExtraArgs를 통해 URL을 생성하여, MapLoad를 시작한다
	GetWorld()->ServerTravel(Request->ConstructTravelURL());
}
