// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_ShootOnPlayer.h"
#include "FlyingAiCharacter.h"
#include "AIController.h"

EBTNodeResult::Type UBTT_ShootOnPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AFlyingAICharacter* PawnRef = Cast<AFlyingAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		PawnRef->Shoot();
	}
	return EBTNodeResult::Succeeded;
}
