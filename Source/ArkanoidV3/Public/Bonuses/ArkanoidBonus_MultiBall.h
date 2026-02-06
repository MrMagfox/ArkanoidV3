// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArkanoidBonus.h"
#include "ArkanoidBonus_MultiBall.generated.h"

UCLASS()
class ARKANOIDV3_API AArkanoidBonus_MultiBall : public AArkanoidBonus
{
	GENERATED_BODY()

public:
	/** Сколько дополнительных шаров создать (обычно 2, чтобы в сумме стало 3) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Bonus Logic")
	int32 ExtraBallsCount = 2;

	/** Угол разлета новых шаров (в градусах). 
	 * Например, 30.0f означает, что шары полетят под углом +/- 30 градусов от основного. 
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Bonus Logic")
	float SpreadAngle = 30.0f;

protected:
	virtual void ActivateBonusEffect(class AArkanoidPaddle* PlayerPawn) override;
};
