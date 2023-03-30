// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_QuickMoveLOrR.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_QuickMoveLOrR::UBTT_QuickMoveLOrR()
{
	bNotifyTick = true;
	MoveTime = 1.f;
	Speed = 600.f;
	bCreateNodeInstance = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTT_QuickMoveLOrR::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CurrentTime = MoveTime;
	// Optimized and secured
	ActivateRotateOnMovement(OwnerComp, false);
	DirectionValue = (UKismetMathLibrary::RandomBool() ? (-1.f) : (1.f));
	return EBTNodeResult::InProgress;
}

void UBTT_QuickMoveLOrR::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	CurrentTime -= DeltaSeconds;

	if (APawn* Pawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		const FVector TargetDirection = Pawn->GetActorRightVector() * DirectionValue;
		Pawn->GetMovementComponent()->AddInputVector(TargetDirection);
		if (const AActor* PlayerRef = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetRef.SelectedKeyName)))
		{
			Pawn->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(Pawn->GetActorLocation(), PlayerRef->GetActorLocation()));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Fail du cast target player dans QuickMoveLOrR. Aborted"));
			ActivateRotateOnMovement(OwnerComp, true);
			FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
		}
	}
	if (CurrentTime <= 0.f)
	{

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UBTT_QuickMoveLOrR::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	ActivateRotateOnMovement(OwnerComp, true);
}

void UBTT_QuickMoveLOrR::ActivateRotateOnMovement(UBehaviorTreeComponent& OwnerComp, bool activ)
{
	if (APawn* SelfPawnRef = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		if (UCharacterMovementComponent* MovementComp = Cast<UCharacterMovementComponent>(SelfPawnRef->GetMovementComponent()))
		{
			MovementComp->bOrientRotationToMovement = activ;
		}
	}
}

