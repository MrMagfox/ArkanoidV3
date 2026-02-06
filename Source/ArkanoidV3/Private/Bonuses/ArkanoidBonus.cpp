// Fill out your copyright notice in the Description page of Project Settings.


#include "Bonuses/ArkanoidBonus.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Pawn/ArkanoidPaddle.h"
#include "Actor/ArkanoidKillZone.h"


AArkanoidBonus::AArkanoidBonus()
{
	
	PrimaryActorTick.bCanEverTick = true; // Включаем Tick для движения

	// Создаем коллизию
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;
	CollisionBox->SetCollisionProfileName(TEXT("Trigger")); // Реагирует на пересечения
	CollisionBox->SetBoxExtent(FVector(32.f, 32.f, 32.f));

	// Создаем меш
	BonusMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BonusMesh"));
	BonusMesh->SetupAttachment(RootComponent);
	BonusMesh->SetCollisionProfileName(TEXT("NoCollision")); // Меш не влияет на физику
}


void AArkanoidBonus::BeginPlay()
{
	Super::BeginPlay();
	// Подписка на событие
	if (CollisionBox)
	{
		CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AArkanoidBonus::OnBonusOverlap);
	}
}


void AArkanoidBonus::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Логика падения: смещаем объект по оси X назад (к игроку)
	FVector FallDirection = FVector(-1.0f, 0.0f, 0.0f);
	FVector DeltaLocation = FallDirection * BonusFallingSpeed * DeltaTime;
    
	AddActorLocalOffset(DeltaLocation, true);
}

void AArkanoidBonus::OnBonusOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// ДЕБАГ: Проверяем, с кем столкнулись
	if (OtherActor)
	{
		FString Message = FString::Printf(TEXT("Bonus touched: %s"), *OtherActor->GetName());
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, Message);
	}
	
	// Если коснулись ИГРОКА
	if (OtherActor && OtherActor->IsA(AArkanoidPaddle::StaticClass()))
	{
		if (AArkanoidPaddle* Paddle = Cast<AArkanoidPaddle>(OtherActor))
		{
			ActivateBonusEffect(Paddle);
            
			// Уничтожаем бонус после активации
			Destroy();
		}
	}
	// Если упали в KILLZONE (пролетели мимо)
	else if (OtherActor && OtherActor->IsA(AArkanoidKillZone::StaticClass()))
	{
		Destroy();
	}
}

void AArkanoidBonus::ActivateBonusEffect(AArkanoidPaddle* PlayerPawn)
{
	// Базовая реализация пуста. Здесь будет логика наследников.
	UE_LOG(LogTemp, Warning, TEXT("Base Bonus Effect Triggered (Do nothing)"));
}

