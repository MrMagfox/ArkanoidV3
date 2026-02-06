#include "Core/ArkanoidSaveGame.h"

UArkanoidSaveGame::UArkanoidSaveGame()
{
	// По умолчанию открыт только 1-й уровень
	MaxUnlockedLevelIndex = 1;
}