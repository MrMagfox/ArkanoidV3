// Fill out your copyright notice in the Description page of Project Settings.


#include "Bonuses/ArkanoidBonus_Speed.h"
#include "Pawn/ArkanoidPaddle.h"
#include "Actor/ArkanoidBall.h"





void AArkanoidBonus_Speed::ActivateBonusEffect(AArkanoidPaddle* PlayerPawn)
{
	Super::ActivateBonusEffect(PlayerPawn);

	if (!PlayerPawn) return;

	AArkanoidBall* CurrentBall = PlayerPawn->GetCurrentBall();

	if (CurrentBall)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, TEXT("Скорость увеличена!"));
        
		CurrentBall->ApplySpeedBuff(SpeedMultiplier, EffectDuration);
	}
	else
	{
		// ОШИБКА: Бонус взят, но мяч не найден
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Ошибка в ArkanoidBonus_Speed: мяч не найден!"));
	}
}
