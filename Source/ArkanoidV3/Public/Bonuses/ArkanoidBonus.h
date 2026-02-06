// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArkanoidBonus.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class AArkanoidPaddle;

/**
 * @class AArkanoidBonus
 * @brief Базовый класс для всех выпадающих предметов.
 * Реализует падение вниз и уничтожение при подборе.
 */


UCLASS()
class ARKANOIDV3_API AArkanoidBonus : public AActor
{
	GENERATED_BODY()

public:
	
	AArkanoidBonus();

protected:

	virtual void BeginPlay() override;

public:
	
	virtual void Tick(float DeltaTime) override;

	// --- КОМПОНЕНТЫ ---

	/** Коллизия для детекта пересечения с кареткой */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arkanoid | Components")
	TObjectPtr<UBoxComponent> CollisionBox;

	/** Визуальная модель бонуса (капсула, куб и т.д.) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arkanoid | Components")
	TObjectPtr<UStaticMeshComponent> BonusMesh;

	// --- НАСТРОЙКИ ---

	/** Скорость падения бонуса (юнитов в секунду) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Bonus Stats")
	float BonusFallingSpeed = 400.0f;

protected:
	/** * @brief Обработчик события пересечения (Overlap).
	 * Проверяет, коснулся ли бонус игрока или зоны смерти.
	 */
	UFUNCTION()
	virtual void OnBonusOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
	 * @brief Виртуальная функция активации эффекта.
	 * Должна быть переопределена в дочерних классах (Speed, Size и т.д.).
	 */
	virtual void ActivateBonusEffect(AArkanoidPaddle* PlayerPawn);
};
