// Fill out your copyright notice in the Description page of Project Settings.

#include "BTT_QuickMoveLOrR.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "FlyingAICharacter.h"
#include "GreenOne/AI/Melee/MeleeAICharacter.h"



UBTT_QuickMoveLOrR::UBTT_QuickMoveLOrR()
{
	bNotifyTick = true;
	MoveTime = 1.f;
	bCreateNodeInstance = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTT_QuickMoveLOrR::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	
	CurrentTime = MoveTime;
	DirectionValue = (UKismetMathLibrary::RandomBool() ? (-1.f) : (1.f));
	OtherDirection = (UKismetMathLibrary::RandomBool() ? (-1.f) : (1.f));
	IsHorizontal = UKismetMathLibrary::RandomBool();
	SetDash(OwnerComp);
	return EBTNodeResult::InProgress;
}

void UBTT_QuickMoveLOrR::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	CurrentTime -= DeltaSeconds;
	
	if (APawn* Pawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		FVector TargetDirection;
		switch (Deplacement)
		{
		case EDimension::Horizontal:
			TargetDirection = Pawn->GetActorRightVector() * DirectionValue;
			SetFlyingRotation(Pawn, FVector2D(DirectionValue, 0));
			break;
		case EDimension::Vertical:
			TargetDirection = Pawn->GetActorUpVector() * DirectionValue;
			break;
		case EDimension::Diagonal:
			TargetDirection = Pawn->GetActorRightVector() * DirectionValue;
			TargetDirection += Pawn->GetActorUpVector() * OtherDirection;
			SetFlyingRotation(Pawn, FVector2D(DirectionValue, 0));
			break;
		case EDimension::Alternate:
			TargetDirection = (IsHorizontal ? (Pawn->GetActorRightVector() * DirectionValue) : (Pawn->GetActorUpVector())) * DirectionValue;
			if(IsHorizontal)
			{
				SetFlyingRotation(Pawn, FVector2D(DirectionValue, 0));
			}
			break;
		}
		if (const AActor* PlayerRef = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(TargetRef.SelectedKeyName)))
		{
			const FVector Upvec = UKismetMathLibrary::RotateAngleAxis(PlayerRef->GetActorUpVector(), -25.f, PlayerRef->GetActorRightVector());
			const float dotPro = UKismetMathLibrary::Dot_VectorVector(Pawn->GetActorForwardVector(), Upvec);
			if (abs(dotPro) >= PercentAlignment)
			{
				
				TargetDirection = Pawn->GetActorUpVector() * ((dotPro < 0) ? (-1) : (1));
			}
			Pawn->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(Pawn->GetActorLocation(), PlayerRef->GetActorLocation()));
			Pawn->GetMovementComponent()->AddInputVector(TargetDirection);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Fail du cast target player dans QuickMoveLOrR. Aborted"));
			FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);
		}
	}
	if (CurrentTime <= 0.f)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UBTT_QuickMoveLOrR::SetFlyingRotation(APawn* RefOwner, FVector2D Axis)
{
	if (AFlyingAICharacter* Oui = Cast<AFlyingAICharacter>(RefOwner))
	{
		Oui->SetRotationAxis(Axis);
	}
}

void UBTT_QuickMoveLOrR::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if(AMeleeAICharacter* PlayerRef = Cast<AMeleeAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		PlayerRef->CanLDash = false;
		PlayerRef->CanRDash = false;
		//PlayerRef->Can_Fighting = false;
		//PlayerRef->CanM_Fighting = false;
	}

}

void UBTT_QuickMoveLOrR::SetDash(UBehaviorTreeComponent& OwnerComp)
{
	if(AMeleeAICharacter* PlayerRef = Cast<AMeleeAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		if(DirectionValue == -1)
		{
			//UE_LOG(LogTemp, Warning, TEXT("gauche"));
			PlayerRef->CanLDash = true;
			PlayerRef->CanRDash = false; 
		}
		else if(DirectionValue == 1)
		{
			//UE_LOG(LogTemp, Warning, TEXT("droite"));
			PlayerRef->CanRDash = true;
			PlayerRef->CanLDash = false; 
			//PlayerRef->CanLDash = true; 
		}
	}
}
/*void UBTT_QuickMoveLOrR::SetFight(UBehaviorTreeComponent& OwnerComp)
{
	if(AMeleeAICharacter* PlayerRef = Cast<AMeleeAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		if(DirectionValue == -1)
		{
			UE_LOG(LogTemp, Warning, TEXT("Combo_gauche"));
			PlayerRef->CanLDash = true;
			PlayerRef->CanRDash = false; 
		}
		else if(DirectionValue == 1)
		{
			UE_LOG(LogTemp, Warning, TEXT("Combo_droite"));
			PlayerRef->CanRDash = true;
			PlayerRef->CanLDash = false; 
			//PlayerRef->CanLDash = true; 
		}
	}
}*/	
/*if(AMeleeAICharacter* PlayerRef = Cast<AMeleeAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
{
	
}*/

	
	//if(CanDash)
	//{
		//EStates EState = EStates::LeftDas;
		//enum States { LeftDas };
		//PlayerRef->PlayAnimMontage(LeftDash, 1, NAME_None);
	//}




