#include "DLItemFragment_Equippable.h"

UDLItemFragment_Equippable::UDLItemFragment_Equippable()
{
}

bool UDLItemFragment_Equippable::IsEquippableClassType(ECharacterType ClassType) const
{
	return (EquippableClassFlags & (1 << (uint32)ClassType)) > 0;
}

