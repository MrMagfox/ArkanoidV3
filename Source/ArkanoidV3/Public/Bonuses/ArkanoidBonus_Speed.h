// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArkanoidBonus.h"
#include "ArkanoidBonus_Speed.generated.h"
/**
 * @class AArkanoidBonus_Speed
 * @brief Бонус, который ускоряет мяч игрока.
 */
UCLASS()
class ARKANOIDV3_API AArkanoidBonus_Speed : public AArkanoidBonus
{
	GENERATED_BODY()

public:
	/** Множитель скорости (1.5 = +50% скорости) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Bonus Logic")
	float SpeedMultiplier = 1.5f;

	/** Время действия эффекта (секунды) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Bonus Logic")
	float EffectDuration = 5.0f;

protected:
	// Переопределяем функцию базового класса
	virtual void ActivateBonusEffect(AArkanoidPaddle* PlayerPawn) override;


};
