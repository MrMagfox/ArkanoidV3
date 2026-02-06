/**
 * @file ArkanoidBall.cpp
 * @brief Реализация логики поведения мяча.
 * * В данном файле настроена физика полета, отскоки и фиксация в одной плоскости (XY).
 */

#include "ArkanoidV3/Public/Actor/ArkanoidBall.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"
#include "Core/ArkanoidGameState.h"      
#include "Bonuses/ArkanoidBonus_Speed.h" 
#include "Bonuses/ArkanoidBonus_Size.h"

/**
 * Конструктор мяча. 
 * Здесь настраиваются компоненты, которые будут созданы автоматически при спавне актора.
 */
AArkanoidBall::AArkanoidBall()
{
	// Отключаем стандартный Tick, так как все движение обрабатывается внутри ProjectileMovementComponent
	PrimaryActorTick.bCanEverTick = false;

	// --- ИНИЦИАЛИЗАЦИЯ МЕША (ВИЗУАЛ + КОЛЛИЗИЯ) ---
	BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
	RootComponent = BallMesh;

	// Настройка профиля коллизии. 'Projectile' или 'BlockAllDynamic' подходят лучше всего.
	BallMesh->SetCollisionProfileName(TEXT("BlockAllDynamic")); 
	
	// ВАЖНО: Отключаем симуляцию физики движка (PhysX/Chaos), чтобы ProjectileMovement полностью контролировал движение.
	BallMesh->SetSimulatePhysics(false);
	BallMesh->SetEnableGravity(false);

	/**
	 * ПРЕДОТВРАЩЕНИЕ ПРОЛЕТА СКВОЗЬ ОБЪЕКТЫ:
	 * bUseCCD (Continuous Collision Detection) — заставляет движок проверять столкновения 
	 * не только в точках А и Б, но и на всем пути следования между кадрами.
	 */
	BallMesh->SetUseCCD(true);
	BallMesh->bTraceComplexOnMove = true;

	// --- НАСТРОЙКА КОМПОНЕНТА ДВИЖЕНИЯ ---
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	
	// Настройка отскоков
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 1.0f; // Эластичность 1.0 = мяч не теряет энергию при ударе
	ProjectileMovement->Friction = 0.0f;   // Трение 0 = мяч не застревает на поверхностях
	
	// Прочие параметры движения
	ProjectileMovement->ProjectileGravityScale = 0.0f; // Никакой гравитации
	ProjectileMovement->InitialSpeed = 0.0f;           // Ждем команды Launch
	ProjectileMovement->MaxSpeed = MaxSpeed;           // Ограничение из заголовочного файла

	/**
	 * СИСТЕМА SWEEP:
	 * Обязательно включаем, чтобы ProjectileMovement проверял столкновения при перемещении.
	 */
	ProjectileMovement->bSweepCollision = true;

	/**
	 * ПЛОСКОЕ ДВИЖЕНИЕ (2D в 3D):
	 * Блокируем ось Z, чтобы мяч никогда не подпрыгивал вверх и не падал вниз.
	 */
	ProjectileMovement->bConstrainToPlane = true;
	ProjectileMovement->SetPlaneConstraintNormal(FVector(0.0f, 0.0f, 1.0f)); // Блокируем вектор (0,0,1)
}

/**
 * Вызывается при запуске игры.
 */
void AArkanoidBall::BeginPlay()
{
	Super::BeginPlay();
	
	// Подписываемся на делегат отскока, чтобы изменять скорость или играть звуки
	if (ProjectileMovement)
	{
		ProjectileMovement->OnProjectileBounce.AddDynamic(this, &AArkanoidBall::OnBounce);
	}
}

/**
 * Запуск мяча. Вызывается из каретки (Paddle).
 * @param Direction Направление, куда полетит мяч.
 */
void AArkanoidBall::Launch(FVector Direction)
{
	// Отрываем мяч от иерархии каретки, чтобы он двигался независимо в мировом пространстве
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	if (ProjectileMovement)
	{
		// Устанавливаем вектор скорости (Направление * Скорость)
		ProjectileMovement->Velocity = Direction.GetSafeNormal() * MinSpeed;
		
		// Активируем компонент, если он был деактивирован
		ProjectileMovement->Activate();
	}
}

/**
 * Обработка события отскока.
 */
void AArkanoidBall::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (!ProjectileMovement) return;

	// Измеряем текущую скорость мяча
	float CurrentSpeed = ProjectileMovement->Velocity.Size();
	
	// Увеличиваем скорость (SpeedMultiplierOnBounce) и зажимаем в пределах [MinSpeed, MaxSpeed]
	float NewSpeed = FMath::Clamp(CurrentSpeed * SpeedMultiplierOnBounce, MinSpeed, MaxSpeed);

	// Сохраняем направление после отскока и применяем новую скорость
	ProjectileMovement->Velocity = ProjectileMovement->Velocity.GetSafeNormal() * NewSpeed;
}

void AArkanoidBall::ApplySpeedBuff(float SpeedMultiplier, float BuffDurationInSeconds)
{
	if (!ProjectileMovement) return;

	// ДЕБАГ: Выводим старые значения
	if (GEngine) 
	{
		FString StartMsg = FString::Printf(TEXT("BALL START: OldMax=%f, Multiplier=%f"), MaxSpeed, SpeedMultiplier);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, StartMsg);
	}

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SpeedBuff);

	if (CachedBaseMaxSpeed <= 0.0f)
	{
		CachedBaseMaxSpeed = MaxSpeed;
		CachedBaseMinSpeed = MinSpeed;
	}

	float NewMaxSpeed = CachedBaseMaxSpeed * SpeedMultiplier;
	float NewMinSpeed = CachedBaseMinSpeed * SpeedMultiplier;

	MaxSpeed = NewMaxSpeed;
	MinSpeed = NewMinSpeed;
	ProjectileMovement->MaxSpeed = NewMaxSpeed;

	// Расчет новой скорости
	float CurrentVelocityMag = ProjectileMovement->Velocity.Size();
	float TargetSpeed = CurrentVelocityMag * SpeedMultiplier;
	TargetSpeed = FMath::Clamp(TargetSpeed, MinSpeed, MaxSpeed);

	ProjectileMovement->Velocity = ProjectileMovement->Velocity.GetSafeNormal() * TargetSpeed;

	// ДЕБАГ: Выводим результат
	if (GEngine)
	{
		FString EndMsg = FString::Printf(TEXT("BALL RESULT: NewMax=%f, NewRealSpeed=%f"), MaxSpeed, ProjectileMovement->Velocity.Size());
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, EndMsg);
	}

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpeedBuff, this, &AArkanoidBall::ResetSpeedToNormal, BuffDurationInSeconds, false);
	// Сообщаем GameState, что активирован бонус СКОРОСТИ
	if (AArkanoidGameState* GS = GetWorld()->GetGameState<AArkanoidGameState>())
	{
		GS->StartBonusTimer(AArkanoidBonus_Speed::StaticClass(), BuffDurationInSeconds);
	}
}

void AArkanoidBall::ResetSpeedToNormal()
{
	if (!ProjectileMovement) return;

	// 1. Возвращаем базовые настройки переменным
	MaxSpeed = CachedBaseMaxSpeed;
	MinSpeed = CachedBaseMinSpeed;

	// 2. Возвращаем настройки компоненту
	ProjectileMovement->MaxSpeed = MaxSpeed;

	// 3. Корректируем текущую скорость мяча, если она вышла за пределы вернувшихся рамок
	float CurrentVelocityMag = ProjectileMovement->Velocity.Size();
    
	// Если мяч летит слишком быстро (после ускорения) -> замедляем
	if (CurrentVelocityMag > MaxSpeed)
	{
		ProjectileMovement->Velocity = ProjectileMovement->Velocity.GetSafeNormal() * MaxSpeed;
	}
	// Если мяч летит слишком медленно (после замедления) -> ускоряем
	else if (CurrentVelocityMag < MinSpeed)
	{
		ProjectileMovement->Velocity = ProjectileMovement->Velocity.GetSafeNormal() * MinSpeed;
	}

	// Сбрасываем кэш (флаг того, что бафф закончился)
	CachedBaseMaxSpeed = 0.0f;
	CachedBaseMinSpeed = 0.0f;

	UE_LOG(LogTemp, Warning, TEXT("BONUS END: Speed reset to normal."));
}

void AArkanoidBall::ApplySizeBuff(float SizeMultiplier, float BuffDuration)
{
	// Сбрасываем таймер (если бонус взят повторно)
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SizeBuff);

	// Кэшируем исходный размер (ТОЛЬКО если это первый активный бонус размера)
	if (CachedBaseScale.IsZero())
	{
		CachedBaseScale = GetActorScale3D();
	}

	// Вычисляем новый размер (Исходный * Множитель)
	// Важно умножать именно ИСХОДНЫЙ, чтобы бонусы не перемножались в бесконечность (2*2*2...)
	FVector NewScale = CachedBaseScale * SizeMultiplier;

	// Применяем масштаб
	SetActorScale3D(NewScale);

	UE_LOG(LogTemp, Warning, TEXT("BONUS: Size x%f! New Scale: %s"), SizeMultiplier, *NewScale.ToString());

	//Запускаем таймер возврата
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_SizeBuff, this, &AArkanoidBall::ResetSizeToNormal, BuffDuration, false);

	// Сообщаем GameState, что активирован бонус РАЗМЕРА МЯЧА
	if (AArkanoidGameState* GS = GetWorld()->GetGameState<AArkanoidGameState>())
	{
		GS->StartBonusTimer(AArkanoidBonus_Size::StaticClass(), BuffDuration);
	}
}

void AArkanoidBall::ResetSizeToNormal()
{
	// Возвращаем как было
	if (!CachedBaseScale.IsZero())
	{
		SetActorScale3D(CachedBaseScale);
	}

	// Сбрасываем кэш
	CachedBaseScale = FVector::ZeroVector;

	UE_LOG(LogTemp, Warning, TEXT("BONUS END: Size reset."));
}
