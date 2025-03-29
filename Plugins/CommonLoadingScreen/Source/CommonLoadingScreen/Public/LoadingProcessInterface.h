// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LoadingProcessInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULoadingProcessInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COMMONLOADINGSCREEN_API ILoadingProcessInterface
{
	GENERATED_BODY()

public:
	// Checks to see if this object implements the interface, and if so asks whether or not we should
	// be currently showing a loading screen
	static bool ShouldShowLoadingScreen(UObject* TestObject);

	virtual bool ShouldShowLoadingScreen() const
	{
		return false;
	}
};
