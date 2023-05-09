// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SG_GreenOne.generated.h"

/**
 * Save à utiliser dans le jeu.
 */
UCLASS(NotBlueprintable)
class GREENONE_API USG_GreenOne : public USaveGame
{
	GENERATED_BODY()

public:

	USG_GreenOne();

	/**
	 * Position du player sur la carte.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Custom|Player")
	FVector PlayerLocation;

	UPROPERTY(BlueprintReadWrite, Category = "Custom|Player")
	FRotator PlayerRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Custom|World")
	FName MapName;

	UPROPERTY(BlueprintReadWrite, Category = "Custom|Player")
	float PlayerLife;

	/**
	 * Nombre de graine dans le chargeur du player.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Custom|Player")
	int32 NbrSeed;

	UPROPERTY(BlueprintReadWrite, Category = "Custom|World")
	float SunLocation;

	UPROPERTY(BlueprintReadWrite, Category = "Custom|Engine")
	bool bIsFirstTime;
	
};
