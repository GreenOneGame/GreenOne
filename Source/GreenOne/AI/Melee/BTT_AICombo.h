// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_AICombo.generated.h"

/**
 * 
 */
UCLASS()
class GREENONE_API UBTT_AICombo : public UBTTaskNode
{
	GENERATED_BODY()


public:

	UBTT_AICombo();

#pragma region Task
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
	void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult);
#pragma endregion	

#pragma region Fight
	int FightMStatus;
	int FightStatus = 0;
	void SetFight(UBehaviorTreeComponent& OwnerComp);
	void SetMoveFight(UBehaviorTreeComponent& OwnerComp);
	void Check(UBehaviorTreeComponent& OwnerComp);
	void HitCheck(UBehaviorTreeComponent& OwnerComp);
#pragma endregion	













	
};


