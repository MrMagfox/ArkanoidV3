// Fill out your copyright notice in the Description page of Project Settings.


#include "Bonuses/ArkanoidBonus_PaddleSize.h"
#include "Pawn/ArkanoidPaddle.h"

void AArkanoidBonus_PaddleSize::ActivateBonusEffect(AArkanoidPaddle* PlayerPawn)
{
	if (PlayerPawn)
	{
		// Вызываем функцию изменения размера у каретки
		PlayerPawn->ApplyPaddleSizeBuff(ScaleMultiplier, Duration);

		if (GEngine)
		{
			FString Msg = FString::Printf(TEXT("Paddle Size Changed! %s"), *ScaleMultiplier.ToString());
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, Msg);
		}
	}
}
