// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_Dodge.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class GREENONE_API UBTT_Dodge : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UBTT_Dodge();

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector PlayerRef;
	
	UPROPERTY(EditAnywhere)
	float DodgeForce = 5.f;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Distance de dodge", UIMin = 0, UIMax = 1, ClampMin = 0, ClampMax = 1))
	// Distance de Dodge autour du player.
	// 0 Le dodge ne parcours aucune distance
	// 1 le Drone se retrouve derrière le player.
	float DodgeDistance = 0.4f;

private:

	AController* ControllerRef;
};
