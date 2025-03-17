// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "DLAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class DUALLINKER_API UDLAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
    static UDLAssetManager& Get();

protected:
    virtual void StartInitialLoading() override;
};
