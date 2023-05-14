// Fill out your copyright notice in the Description page of Project Settings.

#include "GreenOne/AI/Melee/BTT_AICombo.h"
#include "AIController.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GreenOne/AI/Melee/MeleeAICharacter.h"

UBTT_AICombo::UBTT_AICombo()
{
	bNotifyTick = true;
	bCreateNodeInstance = true;
	bNotifyTaskFinished = true;
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_AICombo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AMeleeAICharacter* AiRef = Cast<AMeleeAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		AiRef->FightMStatus = (UKismetMathLibrary::RandomBool() ? (-1.f) : (1.f));
		AiRef->SetMoveFight();
	}
	return EBTNodeResult::InProgress;
}

void UBTT_AICombo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (OwnerComp.GetAIOwner() != nullptr)
	{
		if (AMeleeAICharacter* AiRef = Cast<AMeleeAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
		{
			if (!AiRef->IsWaiting())
			{
				if(AiRef->IsTouch())
				{
					FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				}
				else
				{ FinishLatentTask(OwnerComp, EBTNodeResult::Failed); }
			}
		}
	}
}

void UBTT_AICombo::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if(AMeleeAICharacter* PlayerRef = Cast<AMeleeAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		UE_LOG(LogTemp, Warning, TEXT("taskfinish"));
		PlayerRef->bIsWaiting = true;
		PlayerRef->EndAnimation();
		PlayerRef->StopMouv();
	}
}
