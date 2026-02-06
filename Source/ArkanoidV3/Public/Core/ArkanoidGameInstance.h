#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ArkanoidGameInstance.generated.h"

UCLASS()
class ARKANOIDV3_API UArkanoidGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	// Вызывается 1 раз при запуске игры
	virtual void Init() override;

	/** * Вызывать, когда игрок прошел уровень.
	 * @param LevelIndex Номер уровня, который только что прошли (например, 1).
	 */
	UFUNCTION(BlueprintCallable, Category = "Arkanoid | Progress")
	void CompleteLevel(int32 LevelIndex);

	/** Проверяет, открыт ли этот уровень */
	UFUNCTION(BlueprintCallable, Category = "Arkanoid | Progress")
	bool IsLevelUnlocked(int32 LevelIndex);

protected:
	UPROPERTY()
	int32 CurrentMaxLevel = 1;

	FString SaveSlotName = TEXT("SaveSlot_01");

	void SaveProgress();
	void LoadProgress();
};