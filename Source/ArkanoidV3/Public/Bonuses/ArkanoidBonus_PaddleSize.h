// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArkanoidBonus.h"
#include "ArkanoidBonus_PaddleSize.generated.h"

UCLASS()
class ARKANOIDV3_API AArkanoidBonus_PaddleSize : public AArkanoidBonus
{
	GENERATED_BODY()
	
public:
	/** * Множитель масштаба по осям XYZ.
	 * X = Глубина, Y = Ширина (обычно это главное), Z = Высота.
	 * По умолчанию (1, 2, 1) сделает каретку в 2 раза шире.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Bonus Logic")
	FVector ScaleMultiplier = FVector(1.0f, 2.0f, 1.0f);

	/** Время действия (сек) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Bonus Logic")
	float Duration = 10.0f;

protected:
	virtual void ActivateBonusEffect(class AArkanoidPaddle* PlayerPawn) override;
};