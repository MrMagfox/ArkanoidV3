#include "Core/ArkanoidGameInstance.h"
#include "Core/ArkanoidSaveGame.h"
#include "Kismet/GameplayStatics.h"

void UArkanoidGameInstance::Init()
{
	Super::Init();
	LoadProgress();
}

void UArkanoidGameInstance::CompleteLevel(int32 LevelIndex)
{
	// Если мы прошли уровень 1, то открываем уровень 2.
	// Если прошли уровень 5, но у нас уже открыт 10-й (перепроходим), ничего не меняем.
    
	int32 NextLevelIndex = LevelIndex + 1;

	if (NextLevelIndex > CurrentMaxLevel)
	{
		CurrentMaxLevel = NextLevelIndex;
		SaveProgress();
        
		UE_LOG(LogTemp, Warning, TEXT("LEVEL UNLOCKED: %d"), CurrentMaxLevel);
	}
	// --- ДОБАВЬ ЭТО ДЛЯ ПРОВЕРКИ ---
	UE_LOG(LogTemp, Error, TEXT("GameInstance: CompleteLevel called for Level: %d. Current Max is: %d"), LevelIndex, CurrentMaxLevel);
	// -------------------------------
}

bool UArkanoidGameInstance::IsLevelUnlocked(int32 LevelIndex)
{
	return LevelIndex <= CurrentMaxLevel;
}

void UArkanoidGameInstance::SaveProgress()
{
	// Создаем объект сохранения
	if (UArkanoidSaveGame* SaveInst = Cast<UArkanoidSaveGame>(UGameplayStatics::CreateSaveGameObject(UArkanoidSaveGame::StaticClass())))
	{
		SaveInst->MaxUnlockedLevelIndex = CurrentMaxLevel;
		UGameplayStatics::SaveGameToSlot(SaveInst, SaveSlotName, 0);
	}
}

void UArkanoidGameInstance::LoadProgress()
{
	// Проверяем, есть ли сохранение на диске
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		if (UArkanoidSaveGame* LoadedSave = Cast<UArkanoidSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0)))
		{
			CurrentMaxLevel = LoadedSave->MaxUnlockedLevelIndex;
			UE_LOG(LogTemp, Warning, TEXT("SAVE LOADED. Max Level: %d"), CurrentMaxLevel);
		}
	}
	else
	{
		CurrentMaxLevel = 1; // Если сохранения нет, начинаем с 1
	}
}