// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "CustomCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_NONE	UMETA(Hidden),
	//TODO: Add Custom movement mode
	CMOVE_MAX	UMETA(Hidden),
};

UENUM()
enum EJumpState
{
	JS_None,
	JS_Vertical,
	JS_Horizontal,
};

/**
 * 
 */
UCLASS()
class GREENONE_API UCustomCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	class FSavedMove_CustomCharacter : public FSavedMove_Character
	{
	public:
		FSavedMove_CustomCharacter();

		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;

		//Walk Speed Update
		uint8 bSavedRequestMaxWalkSpeedChange : 1;
	};

	UPROPERTY(Transient)
	class AGreenOneCharacter* GreenOneCharacter;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE AGreenOneCharacter* GetOwnerCharacter() const { return GreenOneCharacter; }

protected:
	virtual void InitializeComponent() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;
	
public:
	UCustomCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;
	
#pragma region Jump/Falling
	//Attributes//
private:
	int32 MaxJump = 2;

	EJumpState InJumpState = JS_None;

	UPROPERTY(EditAnywhere, Category = "Custom|Jump/Falling|Vertical", DisplayName = "Editer la rapidité du jump vertical")
	bool bManualVerticalVelocity;
	/** Default value of vertical jump is the same that jump velocity */
	UPROPERTY(EditAnywhere, Category = "Custom|Jump/Falling|Vertical", DisplayName = "Force d'impulsion du jump vertical", meta = (ForceUnits = "cm/s",  EditCondition="bManualVerticalVelocity"))
	float VerticalJumpVelocity = 1000.f;
	float VelocityTemp;
	UPROPERTY(EditAnywhere, Category = "Custom|Jump/Falling|Vertical", DisplayName = "Hauteur max", meta = (ForceUnits = "cm/s"))
	float MaxVerticalHeight = 800.f;
	bool bVerticalJump;
	UPROPERTY(EditAnywhere, Category = "Custom|Jump/Falling|Vertical", meta = (ClampMin = 0.0f, ClampMax = 1.f))
	float SafeZone = 1.f;
	UPROPERTY(EditAnywhere, Category = "Custom|Jump/Falling|Vertical", DisplayName = "Type de curve de jump")
	TEnumAsByte<EEasingFunc::Type> VerticalJumpCurve = EEasingFunc::CircularOut;
	UPROPERTY(EditAnywhere, Category = "Custom|Jump/Falling|Vertical", DisplayName = "Activer la détection du plafond")
	bool bActiveCheckRoof = false;

	UPROPERTY(EditAnywhere, Category = "Custom|Jump/Falling|Horizontal", DisplayName = "Editer la rapidité du jump horizontal")
	bool bManualHorizontalVelocity = false;
	/** Default value of horizontal jump is the same that jump velocity */
	UPROPERTY(EditAnywhere, Category = "Custom|Jump/Falling|Horizontal", meta = (ForceUnits = "cm/s", EditCondition="bManualHorizontalVelocity"), DisplayName = "Rapidité du jump horizontal")
	float HorizontalJumpVelocity = 700.f;
	bool bHorizontalJump;
	
	UPROPERTY(EditAnywhere, Category = "Custom|Jump/Falling|Horizontal", meta = (ForceUnits = "cm/s"), DisplayName = "Distance du jump horizontal")
	float MaxDistanceHorizontalJump = 350.f;
	float DistanceHorizontalJump;

	FVector2D HorizontalJumpDirection = FVector2D::ZeroVector;
	float TargetDistance = 0;

	FVector CurrentLocation;
	FVector TargetJumpLocation;

	float JumpTime = 0;

public:
	/** Commun a tous les jumps **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Custom|Jump/Falling", DisplayName = "Velocité général", meta = (ForceUnits = "cm/s"))
	float JumpVelocity = 700.f;

	UPROPERTY(EditAnywhere, Category = "Custom|Jump/Falling", DisplayName = "Jump Gravité")
	float CustomGravityScale = 1.75f;

	//Functions//
private:
	virtual bool CheckFall(const FFindFloorResult& OldFloor, const FHitResult& Hit, const FVector& Delta, const FVector& OldLocation, float remainingTime, float timeTick, int32 Iterations, bool bMustJump) override;
	bool VerticalJump();
	bool HorizontalJump();
	void ExecHorizontalJump();
	void ExecVerticalJump(const float DeltaTime);

public:
	UFUNCTION(BlueprintCallable)
	virtual bool DoJump(bool bReplayingMoves) override;
	UFUNCTION(BlueprintCallable)
	bool DoHorizontalJump() const;
	UFUNCTION(BlueprintCallable)
	EJumpState GetCurrentJumpState() const;
	UFUNCTION(BlueprintCallable)
	void SetHorizontalJumpDirection(FVector2D& NewDirection);

#pragma endregion 
};
