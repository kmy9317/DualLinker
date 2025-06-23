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

	GameplayTags.InputTag_Attack_MainHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag.Attack.MainHand"),
		FString("")
	);
	
	/**
	*  Ability Tags
	*/
	GameplayTags.Ability_ActivateFail_ActivationGroup = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.ActivateFail.ActivationGroup"),
		FString("")
	);
	
	GameplayTags.Ability_ChangeEquip = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.ChangeEquip"),
		FString("")
	);

	GameplayTags.Ability_Attack1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Attack1"),
		FString("")
	);

	GameplayTags.Ability_Attack2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Ability.Attack2"),
		FString("")
	);
	
	/**
	*  Gameplay Event Tags
	*/
	GameplayTags.GameplayEvent_Montage_End = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("GameplayEvent.Montage.End"),
			FString("")
		);
	
	GameplayTags.GameplayEvent_ChangeEquip = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayEvent.ChangeEquip"),
		FString("")
	);

	GameplayTags.GameplayEvent_Trace = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayEvent.Trace"),
		FString("")
	);

	/**
	*  Status Tags
	*/
	GameplayTags.Status_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Status.Attack"),
		FString("")
	);

	/**
	* Player Status Tags
	*/
	GameplayTags.Player_Status_ChangeEquip = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("GameplayEvent.ChangeEquip"),
		FString("")
	);

	/**
	* Set By Caller
	*/
	GameplayTags.SetByCaller_BaseDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("SetByCaller.BaseDamage"),
		FString("")
	);
}

