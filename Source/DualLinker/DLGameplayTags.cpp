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

	/*
	 * Primary Attributes
	 */
	GameplayTags.InputTag_Move = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Move"),
		FString("")
	);

	GameplayTags.InputTag_Look_Mouse = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Look.Mouse"),
		FString("")
	);

	// TEMP
	GameplayTags.InputTag_Unequip = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Unequip"),
		FString("")
	);

	GameplayTags.Player_Status_Equipped = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Player.Status.Equipped"),
		FString("")
	);
}

