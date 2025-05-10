#pragma once

UENUM(BlueprintType)
enum class ECharacterType : uint8
{
    // Player
    Edel    UMETA(DisplayName = "Edel"),
    Sieg    UMETA(DisplayName = "Sieg"),

    // Enemy

    Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EEquipmentSlotType : uint8
{
	Unarmed_LeftHand,
	Unarmed_RightHand,

	Primary_LeftHand,
	Primary_RightHand,
	Primary_TwoHand,

	Secondary_LeftHand,
	Secondary_RightHand,
	Secondary_TwoHand,

	Utility_Primary,
	Utility_Secondary,
	Utility_Tertiary,
	Utility_Quaternary,

	Helmet,
	Chest,
	Legs,
	Hands,
	Foot,

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	Weapon,
	Armor,
	Utility,

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWeaponSlotType : uint8
{
	Primary,
	Secondary,

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Unarmed,
	OneHandSword,
	TwoHandSword,
	Shield,
	Gun,

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWeaponHandType : uint8
{
	LeftHand,
	RightHand,
	TwoHand,

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EArmorType : uint8
{
	Helmet,
	Chest,
	Legs,
	Hands,
	Foot,

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EUtilityType : uint8
{
	Drink,
	LightSource,

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EUtilitySlotType : uint8
{
	Primary,
	Secondary,
	Tertiary,
	Quaternary,

	Count	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EEquipState : uint8
{
	Unarmed,

	Weapon_Primary,
	Weapon_Secondary,

	Utility_Primary,
	Utility_Secondary,
	Utility_Tertiary,
	Utility_Quaternary,

	Count	UMETA(Hidden)
};
