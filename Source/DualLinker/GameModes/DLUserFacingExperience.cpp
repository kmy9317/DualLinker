// Fill out your copyright notice in the Description page of Project Settings.


#include "DLUserFacingExperience.h"
#include "DualLinker/System/TravelManagerSubsystem.h"

UTravelManager_TravelRequest* UDLUserFacingExperience::CreateTravelRequest() const
{
	const FString ExperienceName = ExperienceID.PrimaryAssetName.ToString();

	// - UTravelManager_TravelRequest는 UObject로 생성해놓고, 알아서 GC가 된다:
	// - 해당 객체는 현재 프레임에서 사용하기 때문에, GC에 대한 염려가 필요없다: 만약 다음 프레임이든 추가적인 프레임 상에서 해당 객체를 사용할 경우, Lifetime 관리 필요!
	//   - 그렇지 않으면 dangling 난다!
	UTravelManager_TravelRequest* Result = NewObject<UTravelManager_TravelRequest>();
	Result->MapID = MapID;
	Result->ExtraArgs.Add(TEXT("Experience"), ExperienceName);

	return Result;
}