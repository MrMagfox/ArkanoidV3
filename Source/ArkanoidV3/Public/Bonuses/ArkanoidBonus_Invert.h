#pragma once

#include "CoreMinimal.h"
#include "Bonuses/ArkanoidBonus.h"
#include "ArkanoidBonus_Invert.generated.h"


UCLASS()
class ARKANOIDV3_API AArkanoidBonus_Invert : public AArkanoidBonus
{
	GENERATED_BODY()
	
public:
	/** Время действия дебаффа */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Bonus Logic")
	float Duration = 5.0f; // Пусть действует недолго, это же сложно!

protected:
	virtual void ActivateBonusEffect(class AArkanoidPaddle* PlayerPawn) override;
};