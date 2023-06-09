// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractibleActorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractibleActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GREENONE_API IInteractibleActorInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	virtual void Action(AActor* Interactor) {};
};
