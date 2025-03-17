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
	* Enhanced Input Tags
	*/
	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Look_Mouse;

private:
	// static 변수 초기화는 .cpp에 해주는 것을 잊지 말기!
	static FDLGameplayTags GameplayTags;
};