// Fill out your copyright notice in the Description page of Project Settings.


#include "Bonuses/ArkanoidBonus_Invert.h"
#include "Pawn/ArkanoidPaddle.h"

void AArkanoidBonus_Invert::ActivateBonusEffect(AArkanoidPaddle* PlayerPawn)
{
	if (PlayerPawn)
	{
		PlayerPawn->ApplyInvertControl(Duration);
        
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("CONFUSION! Controls Inverted!"));
		}
	}
}

