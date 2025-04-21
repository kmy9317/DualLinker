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