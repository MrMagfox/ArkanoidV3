#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ArkanoidGameState.generated.h"

// Forward declaration
class AArkanoidBonus;

USTRUCT()
struct FActiveBonusEntry
{
    GENERATED_BODY()
    UPROPERTY() TSubclassOf<AArkanoidBonus> BonusClass;
    UPROPERTY() float EndTimeStamp = 0.0f;
};

USTRUCT(BlueprintType)
struct FBonusUIData
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly) TSubclassOf<AArkanoidBonus> BonusClass;
    UPROPERTY(BlueprintReadOnly) float TimeRemaining = 0.0f;
    UPROPERTY(BlueprintReadOnly) bool bIsActive = false; 
};

UCLASS()
class ARKANOIDV3_API AArkanoidGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    AArkanoidGameState();

    // --- ИГРОВАЯ СТАТИСТИКА ---
    void AddScore(int32 Amount);
    void SetLives(int32 NewLives);

    UFUNCTION(BlueprintPure, Category = "Arkanoid | GameProgress")
    int32 GetCurrentScore() const { return CurrentScore; }

    UFUNCTION(BlueprintPure, Category = "Arkanoid | GameProgress")
    int32 GetCurrentLives() const { return CurrentLives; }

    // --- СИСТЕМА ТАЙМЕРА БОНУСОВ ---
    void StartBonusTimer(TSubclassOf<AArkanoidBonus> BonusClass, float Duration);
    UFUNCTION(BlueprintPure, Category = "Arkanoid | UI")
    TArray<FBonusUIData> GetActiveBonusList();
    void ClearAllBonuses();

    // --- НОВАЯ СИСТЕМА ФАЗ (ASSET REFERENCE) ---

    /**
     * @brief Включает уровень по ссылке на ассет и выключает другой (если он является фазой).
     */
    UFUNCTION(BlueprintCallable, Category = "Arkanoid | Phase System")
    void SwitchToLevelByAsset(TSoftObjectPtr<UWorld> LevelAsset);

    /** * @brief Настраивает таймер и запоминает уровни для переключения.
     * @param LevelA Ассет первой фазы.
     * @param LevelB Ассет второй фазы.
     * @param Interval Интервал переключения.
     */
    UFUNCTION(BlueprintCallable, Category = "Arkanoid | Phase System")
    void SetupAndStartAutoSwitcher(TSoftObjectPtr<UWorld> LevelA, TSoftObjectPtr<UWorld> LevelB, float Interval);

    /** Останавливает таймер */
    UFUNCTION(BlueprintCallable, Category = "Arkanoid | Phase System")
    void StopAutoPhaseSwitching();

    // Храним ссылки на выбранные уровни
    UPROPERTY(BlueprintReadOnly, Category = "Arkanoid | Phase System")
    TSoftObjectPtr<UWorld> PhaseAsset_A;

    UPROPERTY(BlueprintReadOnly, Category = "Arkanoid | Phase System")
    TSoftObjectPtr<UWorld> PhaseAsset_B;

    // Храним ссылку на ТЕКУЩИЙ активный уровень
    UPROPERTY(BlueprintReadOnly, Category = "Arkanoid | Phase System")
    TSoftObjectPtr<UWorld> CurrentActiveAsset;

    // Возвращает true, если в указанной фазе есть живые кирпичи. 
    UFUNCTION(BlueprintPure, Category = "Arkanoid | Phase System")
    bool DoesPhaseHaveBricks(TSoftObjectPtr<UWorld> PhaseAsset);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arkanoid | Stats")
    int32 CurrentScore;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arkanoid | Stats")
    int32 CurrentLives;

    TArray<FActiveBonusEntry> ActiveBonusesList;

private:
    FTimerHandle TimerHandle_PhaseSwitch;
    void OnPhaseTimerTick();
};