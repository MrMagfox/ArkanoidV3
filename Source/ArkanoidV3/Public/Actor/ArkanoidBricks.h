#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArkanoidBricks.generated.h"

// Forward declaration компонентов и классов
class UStaticMeshComponent;
class UPrimitiveComponent;
class UMaterialInterface;
class AArkanoidBonus; 

UCLASS()
class ARKANOIDV3_API AArkanoidBricks : public AActor
{
    GENERATED_BODY()

public:
    AArkanoidBricks();

protected:
    virtual void BeginPlay() override;

    /** * @brief Вызывается движком при любом изменении свойств актора в редакторе.
     */
    virtual void OnConstruction(const FTransform& Transform) override;

    UFUNCTION()
    void OnBrickHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
    // --- КОМПОНЕНТЫ ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arkanoid | Components")
    TObjectPtr<UStaticMeshComponent> BrickMesh;

    // --- ВИЗУАЛИЗАЦИЯ ---
    /** * Массив материалов.
     * Index 0 = 1 HP (Default)
     * Index 1 = 2 HP
     * ...
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Visuals")
    TArray<TObjectPtr<UMaterialInterface>> HealthMaterials;

    // --- ПАРАМЕТРЫ СТАТИСТИКИ ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Stats", meta = (ClampMin = "1", ClampMax = "5"))
    int32 HitsToDestroy = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Stats")
    int32 PointsValue = 10;

    // --- НАСТРОЙКИ ЛУТА (DROPS) ---
    // Перенесли в PUBLIC, чтобы работало BlueprintReadWrite

    /** * @brief Массив классов бонусов, которые могут выпасть из этого кирпича.
     * Заполни его в Blueprint.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Loot")
    TArray<TSubclassOf<AArkanoidBonus>> PossibleBonuses;

    /** Шанс выпадения бонуса в процентах (0-100) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Loot", meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float BonusDropChancePercentage = 25.0f;

private:
    // --- ВНУТРЕННЯЯ ЛОГИКА ---
    // Эти функции Blueprint-у видеть не обязательно
    void HandleBrickHit();
    
    /** Обновляет материал. */
    void UpdateVisuals();
};