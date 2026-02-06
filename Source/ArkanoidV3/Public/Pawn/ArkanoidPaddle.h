#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "ArkanoidPaddle.generated.h"

class UInputMappingContext;
class UInputAction;
class AArkanoidBall;

UCLASS()
class ARKANOIDV3_API AArkanoidPaddle : public APawn
{
    GENERATED_BODY()

public:
    AArkanoidPaddle();

protected:
    virtual void BeginPlay() override;

public: 
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // --- Компоненты ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arkanoid | Components")
    TObjectPtr<UStaticMeshComponent> PaddleMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arkanoid | Components")
    TObjectPtr<USceneComponent> BallSpawnPoint;

    // --- Инпуты ---
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arkanoid | Input")
    TObjectPtr<UInputMappingContext> DefaultMappingContext;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arkanoid | Input")
    TObjectPtr<UInputAction> MoveAction;   // Клавиатура (A/D)

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arkanoid | Input")
    TObjectPtr<UInputAction> LaunchAction; // Пробел/ЛКМ

    // --- MOUSE UPDATE: Новый экшен для мыши ---
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arkanoid | Input")
    TObjectPtr<UInputAction> MouseMoveAction; 

    // --- Настройки ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Stats")
    float MoveSpeed = 1200.0f; // Для клавиатуры

    // --- MOUSE UPDATE: Чувствительность ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Stats")
    float MouseSensitivity = 1.5f; 

    UPROPERTY(EditDefaultsOnly, Category = "Arkanoid | Setup")
    TSubclassOf<AArkanoidBall> BallClass;

private:
    UPROPERTY()
    TObjectPtr<AArkanoidBall> CurrentBall;

    bool bIsBallLaunched;

    void Move(const FInputActionValue& Value);      // Клавиатура
    
    // --- MOUSE UPDATE: Функция мыши ---
    void OnMouseMove(const FInputActionValue& Value); // Мышь

    void LaunchBall();

public:
    UFUNCTION(BlueprintCallable, Category = "Arkanoid | Actions")
    void RespawnBall();
    
    UFUNCTION(BlueprintCallable, Category = "Arkanoid | Access")
    AArkanoidBall* GetCurrentBall() const { return CurrentBall; }
    // Максимальное расстояние движения каретки
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Setup")
    float MovementLimit = 450.0f;

public:
    // ...
    
    /** Реальная скорость каретки (вычисленная вручную) */
    UPROPERTY(BlueprintReadOnly, Category = "Physics")
    FVector RealVelocity;

    // Геттер для мяча
    FVector GetRealVelocity() const { return RealVelocity; }

private:
    /** Позиция в предыдущем кадре */
    FVector LastFrameLocation;

    
    // --- БОНУСЫ (Оставляем как есть) ---
public:
    UFUNCTION(BlueprintCallable, Category = "Arkanoid | Bonus")
    void ApplyPaddleSizeBuff(FVector ScaleMultiplier, float DurationSec);

private:
    FTimerHandle TimerHandle_SizeBuff;
    FVector CachedBaseScale = FVector::ZeroVector;
    void ResetPaddleScale();

public:
    UFUNCTION(BlueprintCallable, Category = "Arkanoid | Bonus")
    void ApplyInvertControl(float DurationSec);

private:
    bool bIsControlInverted = false;
    FTimerHandle TimerHandle_Invert;
    void ResetInvertControl();

public:
    UFUNCTION(BlueprintCallable, Category = "Arkanoid | Bonus")
    void ApplyPaddleSpeedBuff(float SpeedMultiplier, float Duration);

private:
    float CachedBaseMoveSpeed = 0.0f;
    FTimerHandle TimerHandle_PaddleSpeed;
    void ResetPaddleSpeed();
};