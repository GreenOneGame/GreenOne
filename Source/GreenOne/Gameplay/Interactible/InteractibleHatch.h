// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractibleStaticActor.h"
#include "InteractibleHatch.generated.h"

/**
 * 
 */
UCLASS()
class GREENONE_API AInteractibleHatch : public AInteractibleStaticActor
{
	GENERATED_BODY()

	class ACharacter* PlayerRef;

	UFUNCTION()
	void InteractTpPlayer();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);
#endif
	
public:

	AInteractibleHatch();
	
	UPROPERTY(EditDefaultsOnly, Category = "Component")
	class UTextRenderComponent* LevelNameText;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Custom|Level")
	TSoftObjectPtr<UWorld> LevelToLoad;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Custom|Level")
	TSoftObjectPtr<class APlayerStart> InteractPlayerStartRef;

	virtual void Action(AActor* Interactor) override;
};
