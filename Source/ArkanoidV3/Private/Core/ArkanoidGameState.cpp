#include "Core/ArkanoidGameState.h"
#include "Engine/LevelStreaming.h" 
#include "Bonuses/ArkanoidBonus.h" 
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AArkanoidGameState::AArkanoidGameState()
{
    CurrentScore = 0;
    CurrentLives = 3;
}

void AArkanoidGameState::AddScore(int32 Amount)
{
    CurrentScore += Amount;
}

void AArkanoidGameState::SetLives(int32 NewLives)
{
    CurrentLives = NewLives;
}

// --- ЛОГИКА ТАЙМЕРА (БОНУСЫ) ---
void AArkanoidGameState::StartBonusTimer(TSubclassOf<AArkanoidBonus> BonusClass, float Duration)
{
    if (!GetWorld()) return;
    float CurrentTime = GetWorld()->GetTimeSeconds();
    float NewEndTime = CurrentTime + Duration;
    bool bFound = false;
    for (FActiveBonusEntry& Entry : ActiveBonusesList) {
        if (Entry.BonusClass == BonusClass) {
            Entry.EndTimeStamp = NewEndTime;
            bFound = true; break;
        }
    }
    if (!bFound) {
        FActiveBonusEntry NewEntry;
        NewEntry.BonusClass = BonusClass;
        NewEntry.EndTimeStamp = NewEndTime;
        ActiveBonusesList.Add(NewEntry);
    }
}

TArray<FBonusUIData> AArkanoidGameState::GetActiveBonusList()
{
    TArray<FBonusUIData> ResultArray;
    UWorld* World = GetWorld();
    if (!World) return ResultArray;
    float CurrentTime = World->GetTimeSeconds();
    for (int32 i = ActiveBonusesList.Num() - 1; i >= 0; i--) {
        FActiveBonusEntry& Entry = ActiveBonusesList[i];
        if (CurrentTime >= Entry.EndTimeStamp) {
            ActiveBonusesList.RemoveAt(i);
        } else {
            FBonusUIData UIData;
            UIData.BonusClass = Entry.BonusClass;
            UIData.TimeRemaining = Entry.EndTimeStamp - CurrentTime;
            UIData.bIsActive = true;
            ResultArray.Add(UIData);
        }
    }
    return ResultArray;
}

void AArkanoidGameState::ClearAllBonuses()
{
    ActiveBonusesList.Empty();
}

// --- СИСТЕМА ФАЗ ПОКА НЕ ИСПОЛЬЗУЕТСЯ!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ---

void AArkanoidGameState::SwitchToLevelByAsset(TSoftObjectPtr<UWorld> LevelAsset)
{
    UWorld* World = GetWorld();
    if (!World) return;

    //  Получаем имена фаз (А и Б), которые настроили
    FString NameA = PhaseAsset_A.GetAssetName();
    FString NameB = PhaseAsset_B.GetAssetName(); 

    //Получаем имя уровня, который надо загрузить
    FString TargetName = LevelAsset.GetAssetName();
    
    // Запоминаем текущий
    CurrentActiveAsset = LevelAsset;

    // --- ПРОСТОЙ ЦИКЛ ПО ВСЕМ УРОВНЯМ ---
    const TArray<ULevelStreaming*>& StreamingLevels = World->GetStreamingLevels();

    for (ULevelStreaming* Level : StreamingLevels)
    {
        if (!Level) continue;

        FString LevelPkgName = Level->GetWorldAssetPackageName();
        
        // Если это уровень ФАЗА А
        if (LevelPkgName.Contains(NameA))
        {
            // Если цель — А, то ВКЛЮЧАЕМ. Иначе ВЫКЛЮЧАЕМ.
            bool bShow = (TargetName == NameA);
            
            Level->SetShouldBeVisible(bShow);
            Level->SetShouldBeLoaded(true); // Всегда держим загруженным
            
            // Применяем мгновенно
            if (ULevel* LoadedLevel = Level->GetLoadedLevel())
            {
                LoadedLevel->bIsVisible = bShow;
            }
        }
        // Если это уровень ФАЗА Б
        else if (LevelPkgName.Contains(NameB))
        {
            // Если цель — Б, то ВКЛЮЧАЕМ. Иначе ВЫКЛЮЧАЕМ.
            bool bShow = (TargetName == NameB);
            
            Level->SetShouldBeVisible(bShow);
            Level->SetShouldBeLoaded(true); // Всегда держим загруженным
            
            // Применяем мгновенно
            if (ULevel* LoadedLevel = Level->GetLoadedLevel())
            {
                LoadedLevel->bIsVisible = bShow;
            }
        }
    }

    // Обновляем видимость
    World->FlushLevelStreaming(EFlushLevelStreamingType::Visibility);
}

void AArkanoidGameState::SetupAndStartAutoSwitcher(TSoftObjectPtr<UWorld> LevelA, TSoftObjectPtr<UWorld> LevelB, float Interval)
{
    // 1. Сохраняем ассеты
    PhaseAsset_A = LevelA;
    PhaseAsset_B = LevelB;

    if (Interval <= 0.0f) Interval = 5.0f;

    // 2. Сразу включаем Фазу А
    SwitchToLevelByAsset(PhaseAsset_A);

    // 3. Запускаем таймер
    GetWorldTimerManager().SetTimer(TimerHandle_PhaseSwitch, this, &AArkanoidGameState::OnPhaseTimerTick, Interval, true);
    
    UE_LOG(LogTemp, Warning, TEXT("Auto-Switcher STARTED. Level A: %s, Level B: %s"), 
        *PhaseAsset_A.GetAssetName(), *PhaseAsset_B.GetAssetName());
}

void AArkanoidGameState::StopAutoPhaseSwitching()
{
    GetWorldTimerManager().ClearTimer(TimerHandle_PhaseSwitch);
    UE_LOG(LogTemp, Warning, TEXT("Auto-Switcher STOPPED."));
}

void AArkanoidGameState::OnPhaseTimerTick()
{
    // Логика качелей: Сравниваем пути
    if (CurrentActiveAsset.GetLongPackageName() == PhaseAsset_A.GetLongPackageName())
    {
        SwitchToLevelByAsset(PhaseAsset_B);
    }
    else
    {
        SwitchToLevelByAsset(PhaseAsset_A);
    }
}