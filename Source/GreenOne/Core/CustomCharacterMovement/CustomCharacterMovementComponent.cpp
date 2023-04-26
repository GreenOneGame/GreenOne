// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GreenOne/Gameplay/GreenOneCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

UCustomCharacterMovementComponent::UCustomCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GravityScale = CustomGravityScale;
	JumpZVelocity = JumpVelocity;
	MaxDistanceHorizontalJump = JumpZVelocity / 2;

	static ConstructorHelpers::FObjectFinder<USoundBase> DashSoundObject(TEXT("/Game/GreenOne/SFX/MainCharater/S_Dash"));
	if (DashSoundObject.Object != nullptr)
	{
		DashSound = DashSoundObject.Object;
	}

}

void UCustomCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	GreenOneCharacter = Cast<AGreenOneCharacter>(GetOwner());
}

void UCustomCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ExecHorizontalJump();
	ExecVerticalJump(DeltaTime);

#if DASH == 1
    DashTick(DeltaTime);
    CooldownTick(DeltaTime);
	// Reset the DashDirectionVector
	DashDirectionVector = FVector2D::ZeroVector;
#endif

#if DASH == 2
	DashTick(DeltaTime);
	CooldownTick(DeltaTime);
#endif
}

void UCustomCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
}

void UCustomCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
}

void UCustomCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	if (IsMovingOnGround() && !IsFalling())
	{
		InJumpState = JS_None;
		GetOwnerCharacter()->JumpMaxCount = MaxJump;
		GravityScale = CustomGravityScale;
		bVerticalJump = false;
		bHorizontalJump = false;
		HorizontalJumpDirection = FVector2D::ZeroVector;
	}
}

void UCustomCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	Super::PhysCustom(DeltaTime, Iterations);
}

bool UCustomCharacterMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

#pragma region Jump/Falling

bool UCustomCharacterMovementComponent::DoJump(bool bReplayingMoves)
{
	if (CharacterOwner && CharacterOwner->CanJump())
	{
		bool bJumped = false;
		//Check the jumpState
		if (InJumpState == JS_None)
		{
			bJumped = VerticalJump();
		}
		else if (InJumpState == JS_Vertical)
		{
			bJumped = HorizontalJump();
		}

		return bJumped;
	}

	return false;
}

bool UCustomCharacterMovementComponent::CheckFall(const FFindFloorResult& OldFloor, const FHitResult& Hit,
                                                  const FVector& Delta, const FVector& OldLocation, float remainingTime,
                                                  float timeTick, int32 Iterations,
                                                  bool bMustJump)
{
	GetOwnerCharacter()->JumpCurrentCount = 0;
	GetCharacterOwner()->JumpCurrentCountPreJump = 0;
	GetOwnerCharacter()->JumpMaxCount = MaxJump + 1;
	InJumpState = JS_None;
	return Super::CheckFall(OldFloor, Hit, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump);
}

bool UCustomCharacterMovementComponent::VerticalJump()
{
	//Check if the vertical Jump is edit manually

	if (bManualVerticalVelocity)
	{
		VelocityTemp = VerticalJumpVelocity;
	}
	else
	{
		VelocityTemp = JumpZVelocity;
	}

	if (!bConstrainToPlane || FMath::Abs(PlaneConstraintNormal.Z) != 1.f)
	{
		SetMovementMode(MOVE_Falling);
		InJumpState = JS_Vertical;
		Velocity = GetOwnerCharacter()->GetVelocity();
		bVerticalJump = true;
		TargetJumpLocation = GetOwnerCharacter()->GetActorLocation() + FVector::UpVector * MaxVerticalHeight;
		CurrentLocation = GetOwnerCharacter()->GetActorLocation();
		JumpTime = 0;
		return true;
	}
	return false;
}

bool UCustomCharacterMovementComponent::HorizontalJump()
{
	if (bHorizontalJump) return false;

	bVerticalJump = false;
	FVector Direction = GetOwnerCharacter()->GetActorForwardVector().GetSafeNormal2D();
	FVector Target = Direction;

	if (HorizontalJumpDirection != FVector2D::ZeroVector)
	{
		FVector Forward = GetOwnerCharacter()->GetActorForwardVector().GetSafeNormal2D() * HorizontalJumpDirection.Y;
		FVector Right = GetOwnerCharacter()->GetActorRightVector().GetSafeNormal2D() * HorizontalJumpDirection.X;
		Direction = Forward.GetSafeNormal() + Right.GetSafeNormal();
		Direction.Normalize();

		Target = Direction;
	}

	TempRotationCharacter = GetRotationToDirection(Direction);
	
	//Check if the horizontal Jump is edit manually
	if (bManualHorizontalVelocity)
	{
		Target *= HorizontalJumpVelocity;
	}
	else
	{
		Target *= JumpZVelocity;
	}

	if (InJumpState == JS_Vertical && IsFalling())
	{
		DistanceHorizontalJump = MaxDistanceHorizontalJump;
		GravityScale = 0.f;

		TargetJumpLocation = GetOwnerCharacter()->GetActorLocation() + Direction * MaxDistanceHorizontalJump;

		FHitResult ObstacleHit;
		float CapsuleRadius = GetOwnerCharacter()->GetCapsuleComponent()->GetScaledCapsuleRadius();
		float CapsuleHalfHeight = GetOwnerCharacter()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		TArray<AActor*> ActorsIgnores;
		ActorsIgnores.Push(GetOwnerCharacter());

		//Check if the horizontalJump preview hit an obstacle
		bool bObstacleHit = UKismetSystemLibrary::CapsuleTraceSingle(
			GetWorld(), GetOwnerCharacter()->GetActorLocation(),
			TargetJumpLocation, CapsuleRadius, CapsuleHalfHeight,
			UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
			ActorsIgnores, EDrawDebugTrace::ForDuration, ObstacleHit, true, FLinearColor::Red, FLinearColor::Blue, 2);

		if (bObstacleHit)
		{
			TargetJumpLocation = ObstacleHit.ImpactPoint;
			DistanceHorizontalJump = ObstacleHit.Distance;
		}

		CurrentLocation = GetOwnerCharacter()->GetActorLocation();
		GetOwnerCharacter()->LaunchCharacter(Target, true, true);
		DrawDebugCapsule(GetWorld(), TargetJumpLocation, CapsuleHalfHeight, CapsuleRadius, FQuat::Identity,
		                 FColor::Purple, false, 3);

		bHorizontalJump = true;
		UGameplayStatics::PlaySound2D(GetWorld(), DashSound);
		return true;
	}

	return false;
}

void UCustomCharacterMovementComponent::ExecVerticalJump(const float DeltaTime) {
	if (!bVerticalJump) return;

	JumpTime += DeltaTime;

	const float CurveDeltaTime = (VelocityTemp / MaxVerticalHeight) * JumpTime;

	if (bActiveCheckRoof)
	{
		FHitResult VerticalJumpHitResult;
		const bool bVerticalJumpHit = GetWorld()->LineTraceSingleByChannel(
			VerticalJumpHitResult, GetOwnerCharacter()->GetActorLocation(),
			GetOwnerCharacter()->GetActorLocation() + FVector::UpVector * 3, ECC_Visibility);
		if (bVerticalJumpHit)
		{
			DrawDebugSphere(GetWorld(), VerticalJumpHitResult.ImpactPoint, 8, 10, FColor::Red, false, 2);
			TargetJumpLocation.Z = VerticalJumpHitResult.ImpactPoint.Z;
		}
	}
	
	float NewZVelocity = UKismetMathLibrary::Ease(VerticalJumpVelocity, 0, CurveDeltaTime, VerticalJumpCurve);
	Velocity.Z = NewZVelocity;
	if (CurveDeltaTime > SafeZone)
	{
		bVerticalJump = false;
		//Velocity.Z = 0.f;
		return;
	}
	const float NewZLocation = UKismetMathLibrary::Ease(CurrentLocation.Z, TargetJumpLocation.Z, CurveDeltaTime, VerticalJumpCurve);
	GetOwnerCharacter()->SetActorLocation(FVector(GetOwnerCharacter()->GetActorLocation().X, GetOwnerCharacter()->GetActorLocation().Y, NewZLocation), true);
}

void UCustomCharacterMovementComponent::ExecHorizontalJump()
{
	if (!bHorizontalJump) return;

	TargetDistance += FVector::Distance(CurrentLocation, GetOwnerCharacter()->GetActorLocation());

	
	if (TargetDistance == 0.0f && !BlockCheckHandle.IsValid())
	{
		GetWorld()->GetTimerManager().SetTimer(BlockCheckHandle, [&]()
		{
			GetOwnerCharacter()->LaunchCharacter(FVector::BackwardVector*10.f, false, false);
		},DelayToBlockCheck,false);
	}
	else if(TargetDistance > 0.0f)
	{
		BlockCheckHandle.Invalidate();
	}
	
	
	if (TargetDistance > DistanceHorizontalJump)
	{
		bHorizontalJump = false;
		GravityScale = CustomGravityScale;
		HorizontalJumpDirection = FVector2D::ZeroVector;
		TargetDistance = 0;
		InJumpState = JS_None;
	}
	CurrentLocation = GetActorLocation();
	GetOwnerCharacter()->SetActorRotation(TempRotationCharacter);
}

FRotator UCustomCharacterMovementComponent::GetRotationToDirection(FVector Direction)
{
	return FRotator( 0.f, Direction.Rotation().Yaw, 0.f);
}

/**
 * Return if Character can jump horizontally
 */
bool UCustomCharacterMovementComponent::DoHorizontalJump() const
{
	return bHorizontalJump;
}

EJumpState UCustomCharacterMovementComponent::GetCurrentJumpState() const
{
	return InJumpState;
}

void UCustomCharacterMovementComponent::SetHorizontalJumpDirection(FVector2D& NewDirection)
{
	HorizontalJumpDirection = NewDirection;
}

#pragma endregion

#if DASH == 1
#pragma region Dash

void UCustomCharacterMovementComponent::Dash()
{
	if ( IsFalling() && GetCurrentJumpState() == JS_Vertical)
	{
		GetOwnerCharacter()->Jump();
	}
	
	// Securite
	if (GC == nullptr) { return; }
	if (GC->GetCharacterMovement()->IsFalling()) { return; }
	if (bDashOnCooldown || bIsDashing) { return; }
	// 
	
	BeforeRotationCharacter = GC->GetActorRotation();
	GC->GetCharacterMovement()->SetMovementMode(MOVE_Custom, CMOVE_DASH);
	StartDashLocation = GC->GetActorLocation();

	FVector DirectionVector = FVector::ZeroVector;
	
	// Récupération de la direction du joueur
	FVector Direction = GC->GetActorForwardVector().GetSafeNormal2D();
	
	if ( DashDirectionVector != FVector2D::ZeroVector )
	{
		FVector Forward = GC->GetActorForwardVector().GetSafeNormal2D() * DashDirectionVector.Y;
		FVector Right = GC->GetActorRightVector().GetSafeNormal2D() * DashDirectionVector.X;
		Direction = Forward + Right;
		Direction.Normalize();
	}

	TempRotationCharacter = GetRotationToDirection(Direction);

	Direction = FVector(Direction.X, Direction.Y, 0.f);

	if (Direction == FVector::ZeroVector)
	{
		DirectionVector = GC->GetActorForwardVector();
	}
	else
	{
		DirectionVector = Direction;
	}
	
	FHitResult ObstacleHit;
	float CapsuleRadius = GC->GetCapsuleComponent()->GetScaledCapsuleRadius();
	float CapsuleHalfHeight = GC->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	TArray<AActor*> ActorsIgnores;
	ActorsIgnores.Push(GC);

	TargetDashLocation = StartDashLocation + DirectionVector * DashDistance;

	bool bObstacleHit = UKismetSystemLibrary::CapsuleTraceSingle(
	GetWorld(), GetOwnerCharacter()->GetActorLocation(),
	TargetDashLocation, CapsuleRadius, CapsuleHalfHeight,
	UEngineTypes::ConvertToTraceType(ECC_Visibility), false,
	ActorsIgnores, EDrawDebugTrace::ForDuration, ObstacleHit, true,
	FLinearColor::Red, FLinearColor::Blue, 2);
	
	// Si le dash est en collision avec un objet, on reduit la distance du dash
	if (bObstacleHit)
	{
		TargetDashLocation = StartDashLocation + (DirectionVector * (ObstacleHit.Distance - 50.f));
	}
	
	DashTime = (DashDistance / DashSpeed) * 1000;

	Velocity = (DirectionVector * DashSpeed);
	
	CurrentDashAlpha = 0.f;
	bIsDashing = true;
	UGameplayStatics::PlaySound2D(GetWorld(), DashSound);
}

void UCustomCharacterMovementComponent::CancelDash()
{
	if (bIsDashing)
	{
		bIsDashing = false;
		bDashOnCooldown = true;
		CurrentDashCooldown = DashCooldown;
		DashDirectionVector = FVector2D::ZeroVector;
		GC->SetActorLocation(TempTargetLocation);
		GC->SetActorRotation(BeforeRotationCharacter);
		GC->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

void UCustomCharacterMovementComponent::DashTick(float DeltaTime)
{
	// Securite
	if (!bIsDashing || bDashOnCooldown) { return; }
	if (GC == nullptr) { return; }
	//

	CurrentDashAlpha += (DeltaTime * 1000) / (DashTime);

	if (CurrentDashAlpha >= 1)
	{
		CurrentDashAlpha = 1;
		CurrentDashCooldown = DashCooldown;
		bIsDashing = false;
		bDashOnCooldown = true;
		GC->SetActorRotation(BeforeRotationCharacter);
		GC->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		DashDirectionVector = FVector2D::ZeroVector;
	}

	TempTargetLocation = UKismetMathLibrary::VLerp(StartDashLocation, TargetDashLocation, CurrentDashAlpha);
	GC->SetActorLocation(TempTargetLocation);
	GC->SetActorRotation(TempRotationCharacter);
}

void UCustomCharacterMovementComponent::CooldownTick(float DeltaTime)
{
	if (!bDashOnCooldown) { return; }
	CurrentDashCooldown -= DeltaTime;

	if (CurrentDashCooldown <= 0.f)
	{
		CurrentDashCooldown = 0.f;
		bDashOnCooldown = false;
		DashDirectionVector = FVector2D::ZeroVector;
	}
}

#pragma endregion

#endif

#if DASH == 2

#pragma region Dash V2

void UCustomCharacterMovementComponent::Dash()
{
	if ( IsFalling() && GetCurrentJumpState() == JS_Vertical)
	{
		GetOwnerCharacter()->Jump();
	}
	
	// Securite
	if (GC == nullptr) { return; }
	if (GC->GetCharacterMovement()->IsFalling()) { return; }
	if (bDashOnCooldown || bIsDashing) { return; }
	
	SetupDashValues();
	
}

void UCustomCharacterMovementComponent::CancelDash()
{
	if (bIsDashing)
	{
		ResetDashValues();
	}
}

// 
void UCustomCharacterMovementComponent::CooldownTick(float DeltaTime)
{
	if (!bDashOnCooldown) { return; }
	CurrentDashCooldown -= DeltaTime;

	if (CurrentDashCooldown <= 0.f)
	{
		CurrentDashCooldown = 0.f;
		bDashOnCooldown = false;
	}
}

// Utiliser pour placer le player pendant le Dash
void UCustomCharacterMovementComponent::DashTick(float DeltaTime)
{
	//Returning if the player is not dashing or if there is only one point in the dashPoints array
	if (!bIsDashing) return;
	
	if (DashPoints.Num() <= 1)
	{
		bIsDashing = false;
		return;
	}

	//Increase the Time elapsed while Dashing
	CurrentDashCooldown += DeltaTime;

	//Calculate the Distance between the current point and the next one, and the duration of the segment using this distance relative to the total distance
	float DeltaDist = FVector::Distance(DashPoints[Iterator - 1], DashPoints[Iterator]);
	float SegmentDuration = DeltaDist * DashTime / DashDistance;
	//Then calculate the alpha to lerp between the current point and the next one and set the new location of the player
	float LerpAlpha = FMath::Clamp(CurrentDashCooldown / SegmentDuration, 0.0f, 1.0f);
	FVector NewLocation = FMath::Lerp(DashPoints[Iterator - 1], DashPoints[Iterator], LerpAlpha);
	GetOwner()->SetActorLocation(NewLocation);
	
	if (LerpAlpha >= 1.f)
	{
		//When the Lerp is finished, increase the iterator and reset the time elapsed
		Iterator++;
		if (Iterator >= DashPoints.Num())
		{
			bIsDashing = false;
			// OnDashEnd();
			return;
		}
		CurrentDashCooldown = 0.f;
	}
}

// Called when Dash button is pressed
void UCustomCharacterMovementComponent::SetupDashValues()
{
	// if the dash is on cooldown, return
	if (!bIsDashing || CurrentDashCooldown < DashCooldown) return;
	MAXIT = 20;

	/*
	// Getting Movement direction of the player and the forward of the Camera
	FVector2d Axis = Cast<AGreenOneCharacter>(GetOwner())->MovementAxisValue.GetSafeNormal();
	// UE_LOG(LogTemp, Warning, TEXT("Axis: %s"), *Axis.ToString());
	
	FVector Forward = GetOwner()->FindComponentByClass<UCameraComponent>()->GetForwardVector().GetSafeNormal2D();
	
	//If the player is not moving, the dash direction is the forward of the player, otherwise it's some scary math shit
	if (Axis.IsNearlyZero())
		DashDirection = GetOwner()->GetActorForwardVector().GetSafeNormal2D();
	else
		DashDirection = Forward * Axis.Y + FVector::CrossProduct(FVector::UpVector, Forward) * Axis.X;

	*/

	
	//Setting the first Begin End and Pause point values
	FVector Begin = GetOwner()->GetActorLocation();
	FVector End = Begin + DashDirection * DashDistance;
	NewPausePoint = Begin;
	DashDistanceLeft = DashDistance;

	//Resetting all dashPoints and adding the location of the player as the first one
	DashPoints.Empty();
	DashPoints.Add(Begin);

	//Storing the velocity vector size before the Dash, then calling the trace
	// FVector Velocity = Cast<ADarkestCanopyCharacter>(GetOwner())->GetCharacterMovement()->Velocity;
	InitialSpeedSize = FVector{Velocity.X, Velocity.Y, 0}.Size();

	CollisionTrace(Begin, End);
}

void UCustomCharacterMovementComponent::ResetDashValues()
{
	if (bIsDashing)
	{
		//Setting the velocity to 0 to prevent moving while Dashing
		Cast<AGreenOneCharacter>(GetOwner())->GetCharacterMovement()->Velocity = FVector::ZeroVector;
		for (int i = 0; i < DashPoints.Num() - 1; i++)
		{
			//If a point is too close to the next one, remove it
			if (FVector::Distance(DashPoints[i], DashPoints[i + 1]) < 3.f)
			{
				DashPoints.RemoveAt(i);
				i--;
			}
		}

		bIsDashing = false;
		bDashOnCooldown = true;
		CurrentDashCooldown = DashCooldown;
		Iterator = 1;
		DashDirectionVector = FVector2D::ZeroVector;
		GC->SetActorLocation(TempTargetLocation);
		GC->SetActorRotation(BeforeRotationCharacter);
		GC->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

void UCustomCharacterMovementComponent::CollisionTrace(FVector Begin, FVector End)
{
	//INFINITE LOOP SECURITY
	MAXIT --;
	if (MAXIT < 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Max Iterations Reached"));
		Dash();
		return;
	}

	//Capsule Trace using Capsule scale values of the Player
	FHitResult HitResult;
	bool Hit = UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), Begin, End, Capsule->GetScaledCapsuleRadius(), Capsule->GetScaledCapsuleHalfHeight(), UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
		false, ToIgnore, DrawLines ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, HitResult, true, FLinearColor::Green, FLinearColor::Red, 12.f);

	if (Hit)
	{
		//If Hit and the hit is one of the class to skip, add it to the ignore list and recall the trace
		for (TSubclassOf<AActor> Class : ClassToSkip)
		{
			if (Class != nullptr && HitResult.GetActor() != nullptr && HitResult.GetActor()->IsA(Class))
			{
				ToIgnore.Add(HitResult.GetActor());
				CollisionTrace(Begin,End);
				return;
			}
			
		}
		//If the hit is not one of the class to skip, call the AngleTrace function
		AngleTrace(HitResult);
	}	
	else
	{
		DashPoints.Add(HitResult.TraceEnd);
		// Dash();
	}
}

void UCustomCharacterMovementComponent::AngleTrace(FHitResult &HitResult)
{
	//Projecting the normal of the hit on the dash direction using a plane normal made by DashDirection rotated by 90 degrees on the Z axis
	FVector VertProj = FVector::VectorPlaneProject(HitResult.Normal, DashDirection.RotateAngleAxis(90.f, FVector::UpVector));
	//Calculating the angle between the projected normal and the dash direction
	float VertAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(VertProj.GetSafeNormal(), DashDirection.GetSafeNormal()))) - 90.f;

	//Same thing but with the horizontal plane
	FVector HoriProj = FVector::VectorPlaneProject(HitResult.Normal, FVector::UpVector);
	float HoriAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(HoriProj.GetSafeNormal(), DashDirection.GetSafeNormal()))) - 90.f;

	//Getting the factor of the cross product between the dash direction and the normal of the hit for horizontal angle calculation (need to slide left or right)
	float Factor = DashDirection.X * HitResult.Normal.Y - DashDirection.Y * HitResult.Normal.X;

	//Check if the Hit Object is Climbable
	bool Climbable = CanClimbUP(HitResult);
	
	//If the slide on the hit object is impossible, add the hit location to the dashPoints and call the Dash function
	if (!Climbable && HoriAngle > MaxHorizontalAngle && (HitResult.ImpactNormal.Z < 0 || (VertAngle > MaxVerticalAngle && HitResult.ImpactNormal.Z >= 0)))
	{
		UE_LOG(LogTemp, Warning, TEXT("No Angle Trace"));
		DashPoints.Add(HitResult.Location);
		
		// Dash();
		return;
	}
	
	//Storing the hit location in a temp variable and decrease the total distance left by the distance between the last pause point and the hit location
	FVector TempPoint = HitResult.Location;
	DashDistanceLeft -= FVector::Distance(NewPausePoint, TempPoint);
	if (DashDistanceLeft > 1.f)
	{
		FVector ToPlayer = (NewPausePoint - HitResult.Location).GetSafeNormal();
		//Putting the stored Location into the Newpausepoint and the dashpoints array
		FVector Begin, End;
		NewPausePoint = TempPoint;
		DashPoints.Add(NewPausePoint);
		
		//If the vertical angle is not too big and directed upwards (normal.z beign bigger than 0), create a new begin and end point by rotating the dash direction by the vertical angle
		if (VertAngle < MaxVerticalAngle && HitResult.ImpactNormal.Z > 0)
		{
			//Slighly lifting up the Begin to prevent multiple same detection
			Begin = FVector(NewPausePoint.X, NewPausePoint.Y, NewPausePoint.Z + 3.f);
			FVector Axis = DashDirection.RotateAngleAxis(90.f, FVector::UpVector).GetSafeNormal2D();
			//Creating a new ending position using the dash direction rotated by the vertical angle
			End = Begin + DashDirection.RotateAngleAxis(-VertAngle, Axis) * DashDistanceLeft;
		}
		//Then check if the player can climb up the wall
		else if (Climbable)
		{
			Begin = FVector(NewPausePoint.X, NewPausePoint.Y, NewPausePoint.Z + MaxClimbHeight);
			End = Begin + DashDirection * DashDistanceLeft;
		}
		//Else if the horizontal angle is not too big or if the wall is not directed upwards, create a new begin and end point by rotating the dash direction by the horizontal angle
		else if (HoriAngle < MaxHorizontalAngle || (HitResult.ImpactNormal.Z < 0 && HoriAngle < MaxHorizontalAngle))
		{
			//Create a vector pointing from the hit location to the player to slightly move the trace (prevent infinite hit detection)			
			Begin = NewPausePoint + ToPlayer * 3;
			//Creating a new ending position using the dash direction rotated by the horizontale angle
			FRotator Rot = FRotator(0, Factor > 0 ? HoriAngle : -HoriAngle, 0);
			End = Begin + Rot.RotateVector(DashDirection) * DashDistanceLeft;
		}
		//And finally if nothing is checked, add the hit location to the dashPoints and call the Dash function
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Blocked"));
			DashPoints.Add(HitResult.Location);

			// Dash();
			return;
		}

		//Create an updated DashDirection and Retrace recursively
		DashDirection = FVector{End - Begin}.GetSafeNormal2D();
		DrawDebugLine(GetWorld(), Begin, End, FColor::Orange, false, 2.f, 0, 3.f);
		CollisionTrace(Begin, End);
	}
	else
	{
		//If the total distance left is unsignificant, add the end point to the dashPoints and call the Dash function
		DashPoints.Add(HitResult.TraceEnd);
		
		// Dash();
	}
}

bool UCustomCharacterMovementComponent::CanClimbUP(FHitResult &OldHitResult)
{
	//Creating a new position vector lifted by the max climb height
	FVector HitPosLifted = FVector{OldHitResult.Location.X, OldHitResult.Location.Y, OldHitResult.Location.Z + MaxClimbHeight};
	FHitResult HitResult;
	//Tracing from the lifted position to the Dashdirection, checking if something else is hit
	bool Hit = UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), HitPosLifted, HitPosLifted + DashDirection * DashDistanceLeft, Capsule->GetScaledCapsuleRadius(), Capsule->GetScaledCapsuleHalfHeight(), UEngineTypes::ConvertToTraceType(ECC_WorldStatic),
		false, ToIgnore, DrawLines ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, HitResult, true, FLinearColor::Green, FLinearColor::Red, 2.f);
	
	//If nothing has hit, or if the Planar position of the hit has changed (not the same object is hit), this means the player can Climb and dash
	if (OldHitResult.Location.Y != HitResult.Location.Y || OldHitResult.Location.X != HitResult.Location.X)
	{
		if (!Hit) return true;

		FVector Route = HitResult.Location - OldHitResult.Location;
		DrawDebugLine(GetWorld(), OldHitResult.Location, HitResult.Location, FColor::Red, false, 2.f, 0, 3.f);
		float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(Route.GetSafeNormal(), DashDirection.GetSafeNormal())));
		if (Angle < MaxVerticalAngle)
		{
			return true;
		}
	}	
	return false;
}

#pragma endregion Dash V2

#endif