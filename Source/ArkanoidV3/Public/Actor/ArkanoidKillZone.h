

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArkanoidKillZone.generated.h"

class UBoxComponent;

UCLASS()
class ARKANOIDV3_API AArkanoidKillZone : public AActor
{
	GENERATED_BODY()

public:
	
	AArkanoidKillZone();

protected:
	
	virtual void BeginPlay() override;

	/**
	* @brief Срабатывает, когда что-то входит в зону триггера.
	*  @param OverlappedComp - Компонент, который вызвал событие перекрытия (зона коробки).
	*  @param OtherActor - Актор, который вошел в зону триггера.
	*  @param OtherComp - Компонент, который вошел в зону триггера.
	*  @param OtherBodyIndex - Индекс тела, который вошел в зону триггера.
	*  @param bFromSweep - Признак того, что событие вызвано с помощью триггера.
	*  @param SweepResult - Результат триггера.
	*/
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	// --- КОМПОНЕНТЫ ---

	/** Коллизия-коробка, определяющая зону уничтожения */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arkanoid | Components")
	TObjectPtr<UBoxComponent> ZoneBox;
};
