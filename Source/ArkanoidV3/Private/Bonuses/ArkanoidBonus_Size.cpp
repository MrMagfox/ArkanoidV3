// Fill out your copyright notice in the Description page of Project Settings.


#include "Bonuses/ArkanoidBonus_Size.h"
#include "Actor/ArkanoidBall.h"
#include "Pawn/ArkanoidPaddle.h"


void AArkanoidBonus_Size::ActivateBonusEffect(AArkanoidPaddle* PlayerPawn)
{
	if (!PlayerPawn) return; // Если нет игрока, то ничего не делаем
	// Получаем текущуй шарик игрока (если есть)
	AArkanoidBall* CurrentBall = PlayerPawn->GetCurrentBall();
	if (CurrentBall)
	{
		CurrentBall->ApplySizeBuff(SizeMultiplier, EffectDuration);
        
		// Дебаг сообщение
		if (GEngine)
		{
			FString Msg = FString::Printf(TEXT("Size Bonus: Scale x%f"), SizeMultiplier);
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, Msg);
		}
	}
}