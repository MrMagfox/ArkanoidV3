#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArkanoidBall.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class ARKANOIDV3_API AArkanoidBall : public AActor
{
	GENERATED_BODY()
	
public:	
	AArkanoidBall();

protected:
	virtual void BeginPlay() override;

public:
	// --- Компоненты ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arkanoid | Components")
	TObjectPtr<UStaticMeshComponent> BallMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arkanoid | Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	// --- Параметры ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Physics")
	float SpeedMultiplierOnBounce = 1.05f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Physics")
	float MinSpeed = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Physics")
	float MaxSpeed = 2500.0f;

	// --- функции ---
	UFUNCTION(BlueprintCallable, Category = "Arkanoid | Actions")
	void Launch(FVector Direction);

	UFUNCTION()
	void OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	// --- СИСТЕМА БОНУСОВ ---
	// --- БОНУС: СКОРОСТЬ ---
public:
	/**
	 * @brief Временно ускоряет мяч.
	 * @param SpeedMultiplier Коэффициент ускорения (например, 1.2 = +20% к скорости).
	 * @param BuffDurationInSeconds Время действия бонуса в секундах.
	 */
	UFUNCTION(BlueprintCallable, Category = "Arkanoid | Bunuses")
	void ApplySpeedBuff(float SpeedMultiplier, float BuffDurationInSeconds);

private:
	FTimerHandle TimerHandle_SpeedBuff;

	// Кэш для возврата значений
	float CachedBaseMaxSpeed = 0.0f;
	float CachedBaseMinSpeed = 0.0f; // Добавили кэш минимума

	void ResetSpeedToNormal();

public:
	// --- БОНУС: РАЗМЕР ---

	/**
	 * @brief Временно меняет размер мяча.
	 * @param SizeMultiplier Множитель размера (2.0 = в 2 раза больше, 0.5 = в 2 раза меньше).
	 * @param BuffDuration Время действия.
	 */
	UFUNCTION(BlueprintCallable, Category = "Arkanoid | Bonuses")
	void ApplySizeBuff(float SizeMultiplier, float BuffDuration);

private:
	/** Таймер для размера */
	FTimerHandle TimerHandle_SizeBuff;

	/** Исходный масштаб мяча (FVector), чтобы вернуть как было */
	FVector CachedBaseScale = FVector::ZeroVector;

	/** Вернуть размер в норму */
	void ResetSizeToNormal();
};