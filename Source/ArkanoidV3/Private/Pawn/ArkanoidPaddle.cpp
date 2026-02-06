#include "Pawn/ArkanoidPaddle.h"
#include "Actor/ArkanoidBall.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Core/ArkanoidGameState.h"            
#include "Bonuses/ArkanoidBonus_PaddleSize.h"
#include "Bonuses/ArkanoidBonus_Invert.h"
#include "Bonuses/ArkanoidBonus_PaddleSpeed.h"

AArkanoidPaddle::AArkanoidPaddle()
{
    USceneComponent* DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DummyRoot"));
    RootComponent = DummyRoot;

    PaddleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PaddleMesh"));
    PaddleMesh->SetupAttachment(RootComponent);
    PaddleMesh->SetCollisionProfileName(TEXT("Pawn"));

    BallSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("BallSpawnPoint"));
    BallSpawnPoint->SetupAttachment(RootComponent);
    BallSpawnPoint->SetRelativeLocation(FVector(50.0f, 0.0f, 0.0f));

    bIsBallLaunched = false;
    MovementLimit = 450.0f; 
    MoveSpeed = 1000.0f;
}

void AArkanoidPaddle::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
       if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
       {
          if (DefaultMappingContext)
          {
             Subsystem->AddMappingContext(DefaultMappingContext, 0);
          }
       }
    }
	// Запоминаем скорость, которую настроил в Блюпринте как "Нормальную"
	CachedBaseMoveSpeed = MoveSpeed;
    RespawnBall();
}

void AArkanoidPaddle::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AArkanoidPaddle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
       if (MoveAction)
          EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AArkanoidPaddle::Move);
       
       if (LaunchAction)
          EnhancedInputComponent->BindAction(LaunchAction, ETriggerEvent::Started, this, &AArkanoidPaddle::LaunchBall);
    }
}

void AArkanoidPaddle::Move(const FInputActionValue& Value)
{
	float AxisValue = Value.Get<float>();

	if (Controller && (AxisValue != 0.0f))
	{
		// --- ЛОГИКА ИНВЕРСИИ ---
		float DirectionMultiplier = bIsControlInverted ? -1.0f : 1.0f;
        
		// ВАЖНО: Добавляем * DirectionMultiplier в формулу!
		float MoveOffset = AxisValue * MoveSpeed * DirectionMultiplier * GetWorld()->GetDeltaSeconds();
        
		FVector CurrentLoc = GetActorLocation();
		float NewY = CurrentLoc.Y + MoveOffset;

		// CLAMP
		NewY = FMath::Clamp(NewY, -MovementLimit, MovementLimit);

		FVector NewLocation = FVector(CurrentLoc.X, NewY, CurrentLoc.Z);
		SetActorLocation(NewLocation);
	}
}

void AArkanoidPaddle::LaunchBall()
{
    if (!bIsBallLaunched && CurrentBall)
    {
       CurrentBall->Launch(GetActorForwardVector());
       bIsBallLaunched = true;
    }
}

void AArkanoidPaddle::RespawnBall()
{
    if (CurrentBall)
    {
       CurrentBall->Destroy();
       CurrentBall = nullptr;
    }

    if (BallClass)
    {
       FActorSpawnParameters SpawnParams;
       SpawnParams.Owner = this;
       SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

       // Спавним в точке SpawnPoint
       CurrentBall = GetWorld()->SpawnActor<AArkanoidBall>(BallClass, BallSpawnPoint->GetComponentTransform(), SpawnParams);

       if (CurrentBall)
       {
          // ВАЖНО: Используем KeepWorldScale, чтобы мяч не наследовал масштаб родителей вообще
          CurrentBall->AttachToComponent(BallSpawnPoint, FAttachmentTransformRules::KeepWorldTransform);
          
          // Сбрасываем локальную позицию и поворот мяча в 0 (относительно точки спавна), но масштаб оставляем (1,1,1)
          CurrentBall->SetActorRelativeLocation(FVector::ZeroVector);
          CurrentBall->SetActorRelativeRotation(FRotator::ZeroRotator);
       }
    }

    bIsBallLaunched = false;
}



void AArkanoidPaddle::ApplyPaddleSizeBuff(FVector ScaleMultiplier, float DurationSec)
{
    GetWorldTimerManager().ClearTimer(TimerHandle_SizeBuff);

    // Берем масштаб именно МЕША, а не актора
    if (CachedBaseScale.IsZero())
    {
       CachedBaseScale = PaddleMesh->GetRelativeScale3D();
    }

    FVector NewScale = CachedBaseScale * ScaleMultiplier;

    // Применяем масштаб только к МЕШУ
    PaddleMesh->SetRelativeScale3D(NewScale);

    UE_LOG(LogTemp, Warning, TEXT("PADDLE BONUS: New Mesh Scale %s"), *NewScale.ToString());

    GetWorldTimerManager().SetTimer(TimerHandle_SizeBuff, this, &AArkanoidPaddle::ResetPaddleScale, DurationSec, false);
    
    if (AArkanoidGameState* GS = GetWorld()->GetGameState<AArkanoidGameState>())
    {
       GS->StartBonusTimer(AArkanoidBonus_PaddleSize::StaticClass(), DurationSec);
    }
}

void AArkanoidPaddle::ResetPaddleScale()
{
	if (!CachedBaseScale.IsZero())
	{
		// Возвращаем масштаб МЕША
		PaddleMesh->SetRelativeScale3D(CachedBaseScale);
	}

	CachedBaseScale = FVector::ZeroVector;
    
	UE_LOG(LogTemp, Warning, TEXT("PADDLE BONUS END: Size reset."));
}

void AArkanoidPaddle::ApplyInvertControl(float DurationSec)
{
	// 1. Сбрасываем старый таймер (продлеваем время)
	GetWorldTimerManager().ClearTimer(TimerHandle_Invert);

	// 2. Включаем режим "Наоборот"
	bIsControlInverted = true;

	UE_LOG(LogTemp, Warning, TEXT("BONUS: Controls INVERTED!"));

	// 3. Запускаем таймер отключения
	GetWorldTimerManager().SetTimer(TimerHandle_Invert, this, &AArkanoidPaddle::ResetInvertControl, DurationSec, false);

	// 4. Отправляем в UI
	if (AArkanoidGameState* GS = GetWorld()->GetGameState<AArkanoidGameState>())
	{
		// Используем класс бонуса (мы создадим его ниже)
		// Если компилятор ругается, пока можно закомментировать строку с GS, 
		// а раскомментировать после создания класса ArkanoidBonus_Invert
		GS->StartBonusTimer(AArkanoidBonus_Invert::StaticClass(), DurationSec);
	}
}

void AArkanoidPaddle::ResetInvertControl()
{
	bIsControlInverted = false;
	UE_LOG(LogTemp, Warning, TEXT("BONUS END: Controls Normal."));
}

void AArkanoidPaddle::ApplyPaddleSpeedBuff(float SpeedMultiplier, float Duration)
{
	// Сбрасываем таймер
	GetWorldTimerManager().ClearTimer(TimerHandle_PaddleSpeed);

	// Если вдруг база не записалась (страховка), пишем сейчас
	if (CachedBaseMoveSpeed <= 0.0f)
	{
		CachedBaseMoveSpeed = MoveSpeed;
	}

	// Считаем новую скорость от БАЗОВОЙ (чтобы бонусы не перемножались грязно)
	float NewSpeed = CachedBaseMoveSpeed * SpeedMultiplier;
	MoveSpeed = NewSpeed;

	UE_LOG(LogTemp, Warning, TEXT("PADDLE SPEED: New Speed: %f"), MoveSpeed);

	// Таймер на возврат
	GetWorldTimerManager().SetTimer(TimerHandle_PaddleSpeed, this, &AArkanoidPaddle::ResetPaddleSpeed, Duration, false);

	// UI Таймер
	if (AArkanoidGameState* GS = GetWorld()->GetGameState<AArkanoidGameState>())
	{
		// Передаем класс бонуса скорости (создадим его ниже)
		GS->StartBonusTimer(AArkanoidBonus_PaddleSpeed::StaticClass(), Duration);
	}
}

void AArkanoidPaddle::ResetPaddleSpeed()
{
	// Возвращаем как было
	if (CachedBaseMoveSpeed > 0.0f)
	{
		MoveSpeed = CachedBaseMoveSpeed;
	}
    
	UE_LOG(LogTemp, Warning, TEXT("PADDLE SPEED END: Reset to %f"), MoveSpeed);
}

