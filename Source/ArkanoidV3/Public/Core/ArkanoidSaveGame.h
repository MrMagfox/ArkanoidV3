#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ArkanoidSaveGame.generated.h"

UCLASS()
class ARKANOIDV3_API UArkanoidSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UArkanoidSaveGame();

	/** Номер максимального уровня, который доступен игроку (1, 2, 3...) */
	UPROPERTY(VisibleAnywhere, Category = "SaveData")
	int32 MaxUnlockedLevelIndex;
};