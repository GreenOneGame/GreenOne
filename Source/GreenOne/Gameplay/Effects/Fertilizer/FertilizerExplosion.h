// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FertilizerBase.h"
#include "FertilizerExplosion.generated.h"

/**
 * 
 */
UCLASS()
class GREENONE_API UFertilizerExplosion : public UFertilizerBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Effect")
	float TimeToAdd = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Effect")
	float AddDamageToExplosion = 20;
	
public:
	UFertilizerExplosion();

	virtual void ApplyEffect(AActor* Actor, AActor* Source) override;
};
