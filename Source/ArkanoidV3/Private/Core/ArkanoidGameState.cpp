#include "Core/ArkanoidGameState.h"
#include "Core/ArkanoidGameMode.h"
#include "Engine/LevelStreaming.h" 
#include "Bonuses/ArkanoidBonus.h"
#include "Actor/ArkanoidBricks.h"
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

bool AArkanoidGameState::DoesPhaseHaveBricks(TSoftObjectPtr<UWorld> PhaseAsset)
{
    if (PhaseAsset.IsNull()) return false;

    UWorld* World = GetWorld();
    if (!World) return false;

    // Имя, которое ищем (например "Sublevel_PhaseB")
    FString TargetName = PhaseAsset.GetAssetName();

    // 1. Перебираем все загруженные стриминг-уровни
    const TArray<ULevelStreaming*>& StreamingLevels = World->GetStreamingLevels();

    for (ULevelStreaming* StreamLevel : StreamingLevels)
    {
        if (!StreamLevel) continue;

        // Проверяем, тот ли это уровень по имени
        FString PkgName = StreamLevel->GetWorldAssetPackageName();
        if (PkgName.Contains(TargetName))
        {
            // 2. Получаем сам объект уровня (ULevel)
            ULevel* LoadedLevel = StreamLevel->GetLoadedLevel();
            
            // Если уровень не загружен — кирпичей точно нет
            if (!LoadedLevel) return false;

            // 3. ПРЯМОЙ ПЕРЕБОР АКТОРОВ В ЭТОМ УРОВНЕ
            // Мы не зависим от BeginPlay, мы смотрим, что лежит в файле прямо сейчас.
            for (AActor* Actor : LoadedLevel->Actors)
            {
                // Проверяем: валиден ли актор, не удаляется ли он, и является ли он Кирпичом
                if (Actor && !Actor->IsPendingKillPending() && Actor->IsA(AArkanoidBricks::StaticClass()))
                {
                    // Нашли хотя бы один кирпич!
                    // Значит, в этот уровень можно переключаться.
                    return true; 
                }
            }
        }
    }

    // Если ничего не нашли
    return false;
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
