// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Navigation/CrowdFollowingComponent.h"
#include "CoreMinimal.h"
#include "GreenOne/AI/BaseEnnemy.h"
#include "MeleeAICharacter.generated.h"

UCLASS()
class GREENONE_API AMeleeAICharacter : public ABaseEnnemy
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMeleeAICharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(EditAnywhere, Category = "Fight|Melee|Detection", DisplayName = "Distance de Detection devant l'ennemie ")
	FVector Offset = FVector(0, 0, 0);



#pragma region ArmCollison
public:

	UFUNCTION(BlueprintCallable)
	void SetCollision();
	void SetRCollision();
	void SetLCollision();
	void EndCollision();
	void LEndCollision();
	void REndCollision();
	
private:
	
	UPROPERTY(EditAnywhere)
	class UBoxComponent* R_ArmCollider;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* L_ArmCollider;

	UFUNCTION()
	void OnCompHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

#pragma endregion

private:
	
	UPROPERTY(EditDefaultsOnly, meta = (DisplayName = "Damage de l'explosion", ClampMin = 0), Category = "Custom|Explosion")
	float ExploDmg;

#pragma region AnimationValue
public:
	
	void StopMouv();	

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool CanDash() { return !IsDashing; };

	bool IsDashing = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Animations)
	bool CanLDash = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Animations)
	bool CanRDash = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Animations)
	bool CanM_Fighting = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Animations)
	bool CanMR_Fighting = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Animations)
	bool Can_Fighting = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Animations)
	bool CanR_Fighting = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Animations)
	bool CanCombo = false;

#pragma endregion	
	
};
