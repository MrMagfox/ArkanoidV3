// Fill out your copyright notice in the Description page of Project Settings.

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
	// Меш каретки
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arkanoid | Components")
	TObjectPtr<UStaticMeshComponent> PaddleMesh;
	// Точка спавна мяча
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arkanoid | Components")
	TObjectPtr<USceneComponent> BallSpawnPoint;

	// --- Инпуты ---
	//  Добавленные в проект через Enhanced Input System
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arkanoid | Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;
	// Действия 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arkanoid | Input")
	TObjectPtr<UInputAction> MoveAction;
	// Действие запуска мяча
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arkanoid | Input")
	TObjectPtr<UInputAction> LaunchAction;

	// --- Настройки ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Stats")
	float MoveSpeed = 1200.0f;
	// Класс мяча
	UPROPERTY(EditDefaultsOnly, Category = "Arkanoid | Setup")
	TSubclassOf<AArkanoidBall> BallClass;

private:
	UPROPERTY()
	TObjectPtr<AArkanoidBall> CurrentBall;

	bool bIsBallLaunched;

	void Move(const FInputActionValue& Value);
	void LaunchBall();

public:
	// Получить точку спавна мяча
	UFUNCTION(BlueprintCallable, Category = "Arkanoid | Actions")
	void RespawnBall();
	
	/** * @brief Геттер для получения ссылки на текущий мяч.
		 * Нужен для бонусов, чтобы они могли менять параметры мяча.
		 */
	UFUNCTION(BlueprintCallable, Category = "Arkanoid | Access")
	AArkanoidBall* GetCurrentBall() const { return CurrentBall; }

	/** Предел перемещения по оси Y (например, 450.0f) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Setup")
	float MovementLimit = 450.0f;

	// --- БОНУСЫ ---
	// Бафф размера каретки
public:
	
	/**
	 * @brief Временно изменяет масштаб каретки по осям XYZ.
	 * @param ScaleMultiplier Множитель размера (например, Y=2.0 сделает каретку в 2 раза шире).
	 * @param DurationSec Время действия бонуса.
	 */
	UFUNCTION(BlueprintCallable, Category = "Arkanoid | Bonus")
	void ApplyPaddleSizeBuff(FVector ScaleMultiplier, float DurationSec);

private:
	/** Таймер для сброса размера */
	FTimerHandle TimerHandle_SizeBuff;

	/** Исходный размер каретки, чтобы вернуть всё как было */
	FVector CachedBaseScale = FVector::ZeroVector;

	/** Функция возврата к норме */
	void ResetPaddleScale();




public:
	// --- Бонус инверсии управления ---
	/**
	 * @brief Включает инверсию управления.
	 * @param DurationSec Время действия.
	 */
	UFUNCTION(BlueprintCallable, Category = "Arkanoid | Bonus")
	void ApplyInvertControl(float DurationSec);

private:
	/** Флаг: если true, управление наоборот */
	bool bIsControlInverted = false;

	/** Таймер для отключения инверсии */
	FTimerHandle TimerHandle_Invert;

	/** Функция возврата нормального управления */
	void ResetInvertControl();


public:
	// --- Бонус ускорения каретки ---
	/**
	 * @brief Изменяет скорость движения каретки.
	 * @param SpeedMultiplier Множитель (1.5 = быстрее, 0.5 = медленнее).
	 * @param Duration Время действия.
	 */
	UFUNCTION(BlueprintCallable, Category = "Arkanoid | Bonus")
	void ApplyPaddleSpeedBuff(float SpeedMultiplier, float Duration);

private:
	/** Храним базовую скорость, чтобы вернуть её после бонуса */
	float CachedBaseMoveSpeed = 0.0f;

	/** Таймер сброса скорости */
	FTimerHandle TimerHandle_PaddleSpeed;

	/** Возврат к норме */
	void ResetPaddleSpeed();
};
