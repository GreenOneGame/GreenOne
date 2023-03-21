// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Dodge.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTT_Dodge::UBTT_Dodge()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTT_Dodge::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(PlayerRef.IsNone())
	{
		UE_LOG(LogTemp,Warning, TEXT("PlayerRef not set in the Dodge Task."));
		return EBTNodeResult::Aborted;
	}
	ControllerRef = Cast<AController>(OwnerComp.GetOwner());
	if (ControllerRef)
	{
		if (UKismetMathLibrary::RandomBool())
		{
			DodgeForce *= -1.f;
		}
	}
	return EBTNodeResult::InProgress;
}

void UBTT_Dodge::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ControllerRef->GetPawn()->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(ControllerRef->GetPawn()->GetActorLocation(), Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(PlayerRef.SelectedKeyName))->GetActorLocation()));
	FVector DodgeDirection = ControllerRef->GetPawn()->GetActorRightVector() * DodgeForce;
	if (ACharacter* AIRef = Cast<ACharacter>(ControllerRef->GetPawn()))
	{
		AIRef->GetMovementComponent()->AddInputVector(DodgeDirection);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Le Owner de la task n'est pas un character est du coup fail l'appel pour le move."));
	}
	const float DotPro = UKismetMathLibrary::Dot_VectorVector(ControllerRef->GetPawn()->GetActorForwardVector(), Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(PlayerRef.SelectedKeyName))->GetActorForwardVector());
	UE_LOG(LogTemp, Warning, TEXT("Dot : %f"), DotPro);
	if ( DotPro >= 0.4f)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
