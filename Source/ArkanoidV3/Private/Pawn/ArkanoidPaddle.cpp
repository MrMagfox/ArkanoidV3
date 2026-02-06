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
    MouseSensitivity = 1.5f; // Дефолтная чувствительность
}

void AArkanoidPaddle::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
       // --- MOUSE UPDATE: Скрываем курсор и захватываем управление ---
       PC->bShowMouseCursor = false;
       PC->SetInputMode(FInputModeGameOnly());
       // -------------------------------------------------------------

       if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
       {
          if (DefaultMappingContext)
          {
             Subsystem->AddMappingContext(DefaultMappingContext, 0);
          }
       }
    }
    CachedBaseMoveSpeed = MoveSpeed;
	LastFrameLocation = GetActorLocation();
	RealVelocity = FVector::ZeroVector;
    RespawnBall();
}

void AArkanoidPaddle::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
	FVector CurrentLocation = GetActorLocation();

	if (DeltaTime > 0.0f)
	{
		// Формула скорости: (Текущая позиция - Старая позиция) / Время
		RealVelocity = (CurrentLocation - LastFrameLocation) / DeltaTime;
	}

	// Запоминаем для следующего кадра
	LastFrameLocation = CurrentLocation;
	// ДЕБАГ: 
	 if (!RealVelocity.IsZero()) UE_LOG(LogTemp, Warning, TEXT("Speed: %s"), *RealVelocity.ToString());
}

void AArkanoidPaddle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
       // Клавиатура (старое)
       if (MoveAction)
          EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AArkanoidPaddle::Move);
       
       // Запуск (старое)
       if (LaunchAction)
          EnhancedInputComponent->BindAction(LaunchAction, ETriggerEvent::Started, this, &AArkanoidPaddle::LaunchBall);

       // --- MOUSE UPDATE: Новая привязка ---
       if (MouseMoveAction)
          EnhancedInputComponent->BindAction(MouseMoveAction, ETriggerEvent::Triggered, this, &AArkanoidPaddle::OnMouseMove);
    }
}

// Движение клавиатурой (зависит от DeltaTime и MoveSpeed)
void AArkanoidPaddle::Move(const FInputActionValue& Value)
{
    float AxisValue = Value.Get<float>();

    if (Controller && (AxisValue != 0.0f))
    {
       float DirectionMultiplier = bIsControlInverted ? -1.0f : 1.0f;
       float MoveOffset = AxisValue * MoveSpeed * DirectionMultiplier * GetWorld()->GetDeltaSeconds();
        
       FVector CurrentLoc = GetActorLocation();
       float NewY = CurrentLoc.Y + MoveOffset;

       NewY = FMath::Clamp(NewY, -MovementLimit, MovementLimit);
       SetActorLocation(FVector(CurrentLoc.X, NewY, CurrentLoc.Z));
    }
}

// --- MOUSE UPDATE: Движение мышью ---
void AArkanoidPaddle::OnMouseMove(const FInputActionValue& Value)
{
    float MouseDelta = Value.Get<float>();

    if (Controller && (MouseDelta != 0.0f))
    {
        // Учет инверсии (если бонус активен)
        float DirectionMultiplier = bIsControlInverted ? -1.0f : 1.0f;

        // Расчет смещения: Дельта (пиксели) * Чувствительность
        // Здесь НЕ умножаем на DeltaTime, так как Input Action Axis уже возвращает дельту за кадр
        float MoveOffset = MouseDelta * MouseSensitivity * DirectionMultiplier;

        FVector CurrentLoc = GetActorLocation();
        float NewY = CurrentLoc.Y + MoveOffset;

        // Ограничение перемещения
        NewY = FMath::Clamp(NewY, -MovementLimit, MovementLimit);

        SetActorLocation(FVector(CurrentLoc.X, NewY, CurrentLoc.Z));
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

       CurrentBall = GetWorld()->SpawnActor<AArkanoidBall>(BallClass, BallSpawnPoint->GetComponentTransform(), SpawnParams);

       if (CurrentBall)
       {
          CurrentBall->AttachToComponent(BallSpawnPoint, FAttachmentTransformRules::KeepWorldTransform);
          CurrentBall->SetActorRelativeLocation(FVector::ZeroVector);
          CurrentBall->SetActorRelativeRotation(FRotator::ZeroRotator);
       }
    }
    bIsBallLaunched = false;
}

// ... Остальные функции бонусов без изменений ...
void AArkanoidPaddle::ApplyPaddleSizeBuff(FVector ScaleMultiplier, float DurationSec)
{
    GetWorldTimerManager().ClearTimer(TimerHandle_SizeBuff);
    if (CachedBaseScale.IsZero()) CachedBaseScale = PaddleMesh->GetRelativeScale3D();
    FVector NewScale = CachedBaseScale * ScaleMultiplier;
    PaddleMesh->SetRelativeScale3D(NewScale);
    UE_LOG(LogTemp, Warning, TEXT("PADDLE BONUS: New Mesh Scale %s"), *NewScale.ToString());
    GetWorldTimerManager().SetTimer(TimerHandle_SizeBuff, this, &AArkanoidPaddle::ResetPaddleScale, DurationSec, false);
    if (AArkanoidGameState* GS = GetWorld()->GetGameState<AArkanoidGameState>())
       GS->StartBonusTimer(AArkanoidBonus_PaddleSize::StaticClass(), DurationSec);
}

void AArkanoidPaddle::ResetPaddleScale()
{
    if (!CachedBaseScale.IsZero()) PaddleMesh->SetRelativeScale3D(CachedBaseScale);
    CachedBaseScale = FVector::ZeroVector;
    UE_LOG(LogTemp, Warning, TEXT("PADDLE BONUS END: Size reset."));
}

void AArkanoidPaddle::ApplyInvertControl(float DurationSec)
{
    GetWorldTimerManager().ClearTimer(TimerHandle_Invert);
    bIsControlInverted = true;
    UE_LOG(LogTemp, Warning, TEXT("BONUS: Controls INVERTED!"));
    GetWorldTimerManager().SetTimer(TimerHandle_Invert, this, &AArkanoidPaddle::ResetInvertControl, DurationSec, false);
    if (AArkanoidGameState* GS = GetWorld()->GetGameState<AArkanoidGameState>())
       GS->StartBonusTimer(AArkanoidBonus_Invert::StaticClass(), DurationSec);
}

void AArkanoidPaddle::ResetInvertControl()
{
    bIsControlInverted = false;
    UE_LOG(LogTemp, Warning, TEXT("BONUS END: Controls Normal."));
}

void AArkanoidPaddle::ApplyPaddleSpeedBuff(float SpeedMultiplier, float Duration)
{
    GetWorldTimerManager().ClearTimer(TimerHandle_PaddleSpeed);
    if (CachedBaseMoveSpeed <= 0.0f) CachedBaseMoveSpeed = MoveSpeed;
    float NewSpeed = CachedBaseMoveSpeed * SpeedMultiplier;
    MoveSpeed = NewSpeed;
    UE_LOG(LogTemp, Warning, TEXT("PADDLE SPEED: New Speed: %f"), MoveSpeed);
    GetWorldTimerManager().SetTimer(TimerHandle_PaddleSpeed, this, &AArkanoidPaddle::ResetPaddleSpeed, Duration, false);
    if (AArkanoidGameState* GS = GetWorld()->GetGameState<AArkanoidGameState>())
       GS->StartBonusTimer(AArkanoidBonus_PaddleSpeed::StaticClass(), Duration);
}

void AArkanoidPaddle::ResetPaddleSpeed()
{
    if (CachedBaseMoveSpeed > 0.0f) MoveSpeed = CachedBaseMoveSpeed;
    UE_LOG(LogTemp, Warning, TEXT("PADDLE SPEED END: Reset to %f"), MoveSpeed);
}