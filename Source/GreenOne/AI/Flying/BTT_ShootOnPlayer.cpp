// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ShootOnPlayer.h"
#include "FlyingAiCharacter.h"
#include "AIController.h"

EBTNodeResult::Type UBTT_ShootOnPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* ControllerRef = OwnerComp.GetAIOwner();
	if (!ControllerRef)
	{
		return EBTNodeResult::Failed;
	}
	if (!ControllerRef->GetPawn())
	{
		return EBTNodeResult::Failed;
	}
	AFlyingAICharacter* PawnRef = Cast<AFlyingAICharacter>(ControllerRef->GetPawn());
	if (PawnRef)
	{
		PawnRef->Shoot();
	}
	return EBTNodeResult::Succeeded;
}
