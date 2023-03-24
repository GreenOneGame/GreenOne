// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GreenOne/Gameplay/EntityGame.h"
#include "BaseEnnemy.generated.h"

UCLASS()
class GREENONE_API ABaseEnnemy : public ACharacter, public IEntityGame
{
	GENERATED_BODY()
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTakeDamage);

public:
	// Sets default values for this character's properties
	ABaseEnnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Property")
	float Health = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0), Category = "Custom|Property")
	float Damage = 12.f;

	/**
	 * Return une valeur entre 0 et 1 correspondant au percentage de vie de l'entity
	 */
	UFUNCTION(BlueprintCallable, Category = "Custom|Health")
	float GetPercentHealth();

	UPROPERTY(BlueprintAssignable, Category = "Custom|Damage")
	FOnTakeDamage OnTakeDamage;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Custom|Damage")
	void EntityTakeDamage(float DamageApply);

protected:

	/************************************************************************/
	/* Draw Debug Life bar													*/
	/************************************************************************/

	UPROPERTY(EditAnywhere, Category = "Debug|Life")
	class UWidgetComponent* LifeBarComponent;

	UPROPERTY(EditAnywhere, Category = "Debug|Life")
	bool DrawLifeBar = false;

	UPROPERTY(EditDefaultsOnly, meta = (EditCondition = DrawLifeBar), Category = "Debug|Life")
	TSubclassOf<UUserWidget> LifeBarClass;

private:

	float MaxHealth = 0;

};