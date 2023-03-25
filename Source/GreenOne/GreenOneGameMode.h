// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GreenOneGameMode.generated.h"

UCLASS(minimalapi)
class AGreenOneGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGreenOneGameMode();

	UFUNCTION(BlueprintCallable)
	void LoadOneLevel(TSoftObjectPtr<UWorld> LevelToUnload, TSoftObjectPtr<UWorld> Level1Ref);

};



