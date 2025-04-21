// Fill out your copyright notice in the Description page of Project Settings.


#include "DLCharacterPartsData.h"

UDLCharacterPartsData::UDLCharacterPartsData()
{

}

bool UDLCharacterPartsData::GetMeshPartForCharacter(ECharacterType CharacterType, FDLCharacterMeshPart& OutMeshPart) const
{
    if (const FDLCharacterMeshPart* FoundPart = PartsMap.Find(CharacterType))
    {
        OutMeshPart = *FoundPart;
        return true;
    }
    return false;
}
