// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_LoadingScreen.generated.h"

/**
 * 
 */
UCLASS()
class GREENONE_API UW_LoadingScreen : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	void RemoveLoading();

};
