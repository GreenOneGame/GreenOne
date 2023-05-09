// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractibleHatch.h"

#include "Components/TextRenderComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GreenOne/Core/Instance/GI_GreenOne.h"
#include "Engine/LevelStreaming.h"
#include "GameFramework/PlayerStart.h"
#include "GreenOne/Gameplay/GreenOneCharacter.h"

AInteractibleHatch::AInteractibleHatch()
{
	LevelNameText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("LevelNameText"));
	LevelNameText->SetupAttachment(RootComponent);
}

#if WITH_EDITOR
void AInteractibleHatch::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(AInteractibleHatch, LevelToLoad))
	{
		LevelNameText->SetText(FText::FromString(LevelToLoad.GetAssetName()));
	}
}

#endif // WITH_EDITOR

void AInteractibleHatch::Action(AActor* Interactor)
{
	Super::Action(Interactor);

	PlayerRef = Cast<AGreenOneCharacter>(Interactor);
	if (PlayerRef != nullptr)
	{
		UGI_GreenOne* GameInstanceRef = Cast<UGI_GreenOne>(GetWorld()->GetGameInstance());
		if (GameInstanceRef != nullptr)
		{
			if (!LevelToLoad.IsNull())
			{
				const FName LevelName = FName(*FPackageName::ObjectPathToPackageName(LevelToLoad.ToString()));
				UE_LOG(LogTemp, Warning, TEXT("Map to load %s"), *LevelName.ToString());
				GameInstanceRef->LoadOneLevel(LevelName, this, FName("InteractTpPlayer"), true);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Je sais pas pourquoi t'es vide"));
			}
		}
	}
}

void AInteractibleHatch::InteractTpPlayer()
{
	 if (UGI_GreenOne* GameInstanceRef = Cast<UGI_GreenOne>(GetWorld()->GetGameInstance()))
	 {
		if (!InteractPlayerStartRef.IsNull())
		{
			if (AActor* TargetLocation = Cast<AActor>(InteractPlayerStartRef.LoadSynchronous()))
			{
				PlayerRef->SetActorLocation(TargetLocation->GetActorLocation());
				PlayerRef->GetCharacterMovement()->StopMovementImmediately();
			}
		}
	 }
}