#pragma once

#include "CoreMinimal.h"
#include "Bonuses/ArkanoidBonus.h"
#include "ArkanoidBonus_PaddleSpeed.generated.h"

/**
 * @brief Бонус изменения скорости самой каретки.
 */
UCLASS()
class ARKANOIDV3_API AArkanoidBonus_PaddleSpeed : public AArkanoidBonus
{
	GENERATED_BODY()
	
public:
	/** Множитель скорости. 
	 * > 1.0 (например 1.5) = Ускорение.
	 * < 1.0 (например 0.5) = Замедление (дебафф).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Bonus Logic")
	float SpeedMultiplier = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Bonus Logic")
	float Duration = 10.0f;

protected:
	virtual void ActivateBonusEffect(class AArkanoidPaddle* PlayerPawn) override;
};