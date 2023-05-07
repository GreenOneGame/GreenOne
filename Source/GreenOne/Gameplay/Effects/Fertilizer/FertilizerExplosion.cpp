// Fill out your copyright notice in the Description page of Project Settings.


#include "FertilizerExplosion.h"

#include "GreenOne/AI/BaseEnnemy.h"

UFertilizerExplosion::UFertilizerExplosion()
{
}

void UFertilizerExplosion::ApplyEffect(AActor* Actor, AActor* Source)
{
	if(!Actor) return;

	if(!IsActorEffectInterface(Actor)) return;
	
	if(ABaseEnnemy* BaseEnnemy = Cast<ABaseEnnemy>(Actor))
	{
		BaseEnnemy->SetDamageZone(AddDamageToExplosion);
	}
	Super::ApplyEffect(Actor);
}
