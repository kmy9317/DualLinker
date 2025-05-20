#pragma once

#include "Containers/UnrealString.h"
#include "Containers/Map.h"
#include "GameplayTagContainer.h"

/** forward declaration */
class UGameplayTagsManager;

/**
 * DLGameplayTags
 * - singleton containing native gameplay tags
 */
struct FDLGameplayTags
{
	/**
	 * static methods
	 */
	static const FDLGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();

	/**
	* Character Type Tags
	*/
	FGameplayTag Character_Sieg;
	FGameplayTag Character_Edel;

	/**
	* Input Tags
	*/
	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Look_Mouse;
	FGameplayTag InputTag_ChangeEquip_Primary;
	FGameplayTag InputTag_ChangeEquip_Secondary;

	/**
	*  Ability Tags
	*/
	FGameplayTag Ability_ChangeEquip;

	/**
	*  Gameplay Event Tags
	*/
	FGameplayTag GameplayEvent_ChangeEquip;

	/**
	* Player Status Tags
	*/
	FGameplayTag Player_Status_Equipped;
	FGameplayTag Player_Status_ChangeEquip;

private:
	// static 변수 초기화는 .cpp에 해주는 것을 잊지 말기!
	static FDLGameplayTags GameplayTags;
};