// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArkanoidBonus.h"
#include "ArkanoidBonus_Size.generated.h"

UCLASS()
class ARKANOIDV3_API AArkanoidBonus_Size : public AArkanoidBonus
{
	GENERATED_BODY()

public:
	/** * Множитель размера.
	 * > 1.0 : Увеличение (например, 2.0)
	 * < 1.0 : Уменьшение (например, 0.5)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Bonus Logic")
	float SizeMultiplier = 2.0f;

	/** Время действия (сек) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Bonus Logic")
	float EffectDuration = 10.0f;

protected:
	virtual void ActivateBonusEffect(class AArkanoidPaddle* PlayerPawn) override;
};
