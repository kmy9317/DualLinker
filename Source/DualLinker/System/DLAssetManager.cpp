// Fill out your copyright notice in the Description page of Project Settings.


#include "DLAssetManager.h"
#include "DualLinker/DLGameplayTags.h"

UDLAssetManager& UDLAssetManager::Get()
{
    check(GEngine);
    UDLAssetManager* DLAssetManager = Cast<UDLAssetManager>(GEngine->AssetManager);
    return *DLAssetManager;
}

void UDLAssetManager::StartInitialLoading()
{
    Super::StartInitialLoading();
    FDLGameplayTags::InitializeNativeGameplayTags();
}
