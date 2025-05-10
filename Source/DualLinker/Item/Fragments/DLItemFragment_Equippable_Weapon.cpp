#include "DLItemFragment_Equippable_Weapon.h"

#include "DualLinker/Item/DLItemInstance.h"
#include "UObject/ObjectSaveContext.h"


UDLItemFragment_Equippable_Weapon::UDLItemFragment_Equippable_Weapon()
{
    EquipmentType = EEquipmentType::Weapon;
}

void UDLItemFragment_Equippable_Weapon::OnInstanceCreated(UDLItemInstance* ItemInstance) const
{
	Super::OnInstanceCreated(ItemInstance);
}
