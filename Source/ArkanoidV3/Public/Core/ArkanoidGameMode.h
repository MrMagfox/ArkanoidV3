#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ArkanoidGameMode.generated.h"

class AArkanoidCamera;

/**
 * @class AArkanoidGameMode
 * @brief Основные правила игры: управление жизнями, респавн мяча, победа/поражение.
 */
UCLASS()
class ARKANOIDV3_API AArkanoidGameMode : public AGameModeBase
{
    GENERATED_BODY()
    
public:
    AArkanoidGameMode();

    virtual void StartPlay() override;

    /** * @brief Вызывается, когда мяч уничтожен (улетел в KillZone).
     * Отнимает жизнь и запускает логику восстановления.
     */
    UFUNCTION(BlueprintCallable, Category = "Arkanoid | Logic")
    void OnBallLost();

protected:
    // --- НАСТРОЙКИ ---

    /** Камера, на которую переключится вид */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arkanoid | Setup")
    TSubclassOf<AArkanoidCamera> GameCameraClass;

    /** Максимальное количество жизней */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arkanoid | Rules")
    int32 MaxLives = 3;

private:
    /** Внутренняя функция респавна мяча на каретке */
    void RespawnBallForPlayer();

public:
    /** Номер текущего уровня (настраивать в Блюпринте самого уровня override) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Settings")
    int32 CurrentLevelIndex = 1;
    
    /** Вызывается кирпичом при уничтожении. ТЕПЕРЬ ПРИНИМАЕТ ССЫЛКУ НА КИРПИЧ */
    void NotifyBrickDestroyed(AActor* BrickActor);

private:
    // --- ИЗМЕНЕНИЕ: Вместо int32 используем массив ссылок для точности ---
    UPROPERTY()
    TArray<AActor*> ActiveBricks;

    // --- НОВОЕ: Таймер задержки перед меню ---
    FTimerHandle TimerHandle_WinDelay;

    /** Переход в главное меню */
    void GoToMainMenu();

public:
    /** * Пересчитать кирпичи на уровне заново. (Больше не используется, но оставим, чтобы не ломать БП) */
    UFUNCTION(BlueprintCallable, Category = "Arkanoid | Logic")
    void RecountBricks();
    
    /** Кирпич вызывает это при создании. ТЕПЕРЬ ПРИНИМАЕТ ССЫЛКУ НА КИРПИЧ */
    void RegisterBrick(AActor* BrickActor);
    /** Проверяет, остались ли живые кирпичи, принадлежащие указанной фазе */
    bool HasBricksInPhase(FName PhaseName);
};