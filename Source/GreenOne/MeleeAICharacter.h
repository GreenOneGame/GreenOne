// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Navigation/CrowdFollowingComponent.h"
#include "CoreMinimal.h"
#include "GreenOne/AI/BaseEnnemy.h"
#include "GameFramework/Character.h"
#include "MeleeAICharacter.generated.h"

UCLASS()
class GREENONE_API AMeleeAICharacter : public ABaseEnnemy
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMeleeAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void AttackMeleeOK();

};
