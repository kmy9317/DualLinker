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
	FGameplayTag InputTag_Attack_MainHand;

	/**
	*  Ability Tags
	*/
	FGameplayTag Ability_ActivateFail_ActivationGroup;
	
	FGameplayTag Ability_ChangeEquip;
	FGameplayTag Ability_Attack1;
	FGameplayTag Ability_Attack2;

	/**
	*  Gameplay Event Tags
	*/
	FGameplayTag GameplayEvent_Montage_End;
	FGameplayTag GameplayEvent_ChangeEquip;
	FGameplayTag GameplayEvent_Trace;

	/**
	*  Status Tags
	*/
	FGameplayTag Status_Attack;
	
	/**
	* Player Status Tags
	*/
	FGameplayTag Player_Status_ChangeEquip;

	/**
	 * Set By Caller
	 */
	FGameplayTag SetByCaller_BaseDamage;
	
private:
	// static 변수 초기화는 .cpp에 해주는 것을 잊지 말기!
	static FDLGameplayTags GameplayTags;
};