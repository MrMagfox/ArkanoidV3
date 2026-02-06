#include "Bonuses/ArkanoidBonus_PaddleSpeed.h"
#include "Pawn/ArkanoidPaddle.h"

void AArkanoidBonus_PaddleSpeed::ActivateBonusEffect(AArkanoidPaddle* PlayerPawn)
{
	if (PlayerPawn)
	{
		PlayerPawn->ApplyPaddleSpeedBuff(SpeedMultiplier, Duration);
        
		if (GEngine)
		{
			FString Msg = (SpeedMultiplier > 1.0f) ? TEXT("NITRO PADDLE!") : TEXT("SLOW PADDLE...");
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, Msg);
		}
	}
}