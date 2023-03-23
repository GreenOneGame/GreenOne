// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingAICharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GreenOne/Gameplay/EntityGame.h"

// Sets default values
AFlyingAICharacter::AFlyingAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFlyingAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFlyingAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AFlyingAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AFlyingAICharacter::Shoot()
{
	//GetWorld()->GetTimerManager().SetTimer(ShootTimer, this, &AFlyingAICharacter::TimerShoot, ShootRate, true);
	TimerShoot();
}

void AFlyingAICharacter::TimerShoot()
{
	FHitResult Outhit;
	const TArray<AActor*> ActorToIgnore;
	GetWorld()->LineTraceSingleByChannel(Outhit, GetActorLocation() + GetActorForwardVector() * 50, GetActorLocation() + (GetActorForwardVector() * 50000), ECC_Camera);
	UE_LOG(LogTemp, Warning, TEXT("Entity hit : %s"), *Outhit.GetActor()->GetFName().ToString());
	if (Outhit.GetActor()->Implements<UEntityGame>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Il possede bien le Interface"));
		IEntityGame::Execute_EntityTakeDamage(Outhit.GetActor(), Damage);
	}
	

	//UKismetSystemLibrary::LineTraceSingleByProfile(GetWorld(), GetActorLocation(), GetActorLocation() + (GetActorForwardVector() * ShootRange), TEXT("ECC_Camera"), false, ActorToIgnore, EDrawDebugTrace::ForOneFrame, Outhit, true);
	//UE_LOG(LogTemp,Warning, TEXT("Touch : %s"), *Outhit.ToString());
	//if (!ProjectileClass)
	//{
	//	return;
	//}

	//GetWorld()->SpawnActor<AActor>(ProjectileClass, GetActorLocation(), GetActorRotation());
}

