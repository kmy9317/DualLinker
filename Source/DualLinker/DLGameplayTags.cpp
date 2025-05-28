#include "DLGameplayTags.h"
#include "GameplayTagsManager.h"

FDLGameplayTags FDLGameplayTags::GameplayTags;

void FDLGameplayTags::InitializeNativeGameplayTags()
{
	/*
	 * Character Types
	 */

	GameplayTags.Character_Sieg = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Character.Sieg"),
		FString("")
	);

	GameplayTags.Character_Edel = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Character.Edel"),
		FString("")
	);

	/**
	* Input Tags
	*/
	GameplayTags.InputTag_Move = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Move"),
		FString("")
	);

	GameplayTags.InputTag_Look_Mouse = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Look.Mouse"),
		FString("")
	);

	GameplayTags.InputTag_ChangeEquip_Primary = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.ChangeEquip.Primary"),
		FString("")
	);

	GameplayTags.InputTag_ChangeEquip_Secondary = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.ChangeEquip.Secondary"),
		FString("")
	);

	/**
	*  Ability Tags
	*/
	GameplayTags.Ability_ChangeEquip = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.ChangeEquip"),
		FString("")
	);

	/**
	*  Gameplay Event Tags
	*/
	GameplayTags.GameplayEvent_ChangeEquip = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayEvent.ChangeEquip"),
		FString("")
	);

	/**
	* Player Status Tags
	*/
	GameplayTags.Player_Status_ChangeEquip = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayEvent.ChangeEquip"),
		FString("")
	);

}

