// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/ArkanoidKillZone.h"
#include "Components/BoxComponent.h"
#include "Actor/ArkanoidBall.h"
#include "Core/ArkanoidGameMode.h"
#include "Bonuses/ArkanoidBonus.h" 

AArkanoidKillZone::AArkanoidKillZone()
{
	PrimaryActorTick.bCanEverTick = false;

	// Создаем BoxComponent
	ZoneBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ZoneBox"));
	RootComponent = ZoneBox;

	/**
	 * НАСТРОЙКА КОЛЛИЗИИ:
	 * Trigger пропускает лучи, но ловит пересечения.
	 */
	ZoneBox->SetCollisionProfileName(TEXT("Trigger"));
    
	// Визуализация в редакторе
	ZoneBox->SetHiddenInGame(true); 
}

void AArkanoidKillZone::BeginPlay()
{
	Super::BeginPlay();

	if (ZoneBox)
	{
		ZoneBox->OnComponentBeginOverlap.AddDynamic(this, &AArkanoidKillZone::OnOverlapBegin);
	}
}

void AArkanoidKillZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 1. Если упал МЯЧ
	if (OtherActor && OtherActor->IsA(AArkanoidBall::StaticClass()))
	{
		// Сначала сообщаем GameMode (пока мяч еще существует!)
		if (AArkanoidGameMode* GM = GetWorld()->GetAuthGameMode<AArkanoidGameMode>())
		{
			GM->OnBallLost();
		}

		// Теперь спокойно уничтожаем
		OtherActor->Destroy();
	}
	// 2. Если упал БОНУС (чтобы не летел бесконечно вниз)
	else if (OtherActor && OtherActor->IsA(AArkanoidBonus::StaticClass()))
	{
		OtherActor->Destroy();
	}
}