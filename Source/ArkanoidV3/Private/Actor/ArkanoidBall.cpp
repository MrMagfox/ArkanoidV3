/**
 * @file ArkanoidBall.cpp
 * @brief Реализация логики поведения мяча.
 */

#include "Actor/ArkanoidBall.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"
#include "Core/ArkanoidGameState.h"      
#include "Bonuses/ArkanoidBonus_Speed.h" 
#include "Bonuses/ArkanoidBonus_Size.h"

// --- ДОБАВЬ ЭТОТ ИНКЛЮД, ЧТОБЫ ВИДЕТЬ КЛАСС PADDLE ---
#include "Pawn/ArkanoidPaddle.h" 
// -----------------------------------------------------

AArkanoidBall::AArkanoidBall()
{
    PrimaryActorTick.bCanEverTick = false;

    BallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BallMesh"));
    RootComponent = BallMesh;
    BallMesh->SetCollisionProfileName(TEXT("BlockAllDynamic")); 
    BallMesh->SetSimulatePhysics(false);
    BallMesh->SetEnableGravity(false);
    BallMesh->SetUseCCD(true);
    BallMesh->bTraceComplexOnMove = true;

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->bShouldBounce = true;
    ProjectileMovement->Bounciness = 1.0f; 
    ProjectileMovement->Friction = 0.0f;   
    ProjectileMovement->ProjectileGravityScale = 0.0f; 
    ProjectileMovement->InitialSpeed = 0.0f;           
    ProjectileMovement->MaxSpeed = MaxSpeed;           
    ProjectileMovement->bSweepCollision = true;
    ProjectileMovement->bConstrainToPlane = true;
    ProjectileMovement->SetPlaneConstraintNormal(FVector(0.0f, 0.0f, 1.0f)); 
}

void AArkanoidBall::BeginPlay()
{
    Super::BeginPlay();
    
    if (ProjectileMovement)
    {
       ProjectileMovement->OnProjectileBounce.AddDynamic(this, &AArkanoidBall::OnBounce);
    }
}

void AArkanoidBall::Launch(FVector Direction)
{
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    if (ProjectileMovement)
    {
       ProjectileMovement->Velocity = Direction.GetSafeNormal() * MinSpeed;
       ProjectileMovement->Activate();
    }
}

// Функция обработки отскока мяча от объектов
void AArkanoidBall::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
    if (!ProjectileMovement) return;

    float CurrentSpeed = ProjectileMovement->Velocity.Size();
    float NewSpeed = FMath::Clamp(CurrentSpeed * SpeedMultiplierOnBounce, MinSpeed, MaxSpeed);
    
    bool bIsCurveApplied = false;
    AActor* HitActor = ImpactResult.GetActor();

    // ПРОВЕРКА НА КАРЕТКУ
    if (HitActor && HitActor->IsA(AArkanoidPaddle::StaticClass()))
    {
        // 1. Кастим к нашему классу, чтобы достать RealVelocity
        AArkanoidPaddle* Paddle = Cast<AArkanoidPaddle>(HitActor);
        
        // Берем НАШУ рассчитанную скорость
        FVector PaddleVel = Paddle ? Paddle->GetRealVelocity() : FVector::ZeroVector;

        // Проверяем скорость (порог 10.0f достаточно, чтобы отсеять шум)
        if (PaddleVel.SizeSquared() > 100.0f) 
        {
            // === ЛОГИКА ПОДКРУТКИ (FRICTION) ===
            // Если каретка едет вправо -> мяч получает пинок вправо.
            
            float FrictionFactor = 0.6f; // Сила подкрутки (0.5 = 50% от скорости каретки)

            FVector BallDir = ProjectileMovement->Velocity;
            
            // Добавляем скорость каретки к поперечной скорости мяча (Y)
            BallDir.Y += PaddleVel.Y * FrictionFactor;
            
            // Важно: Гарантируем, что мяч летит ВПЕРЕД (по X), чтобы не застрял внутри каретки
            BallDir.X = FMath::Abs(BallDir.X); 

            // Применяем
            ProjectileMovement->Velocity = BallDir.GetSafeNormal() * NewSpeed;
            
            bIsCurveApplied = true;
            UE_LOG(LogTemp, Warning, TEXT(">>> Moving Hit! Added Speed: %f"), PaddleVel.Y);
        }
        else
        {
            // === КАРЕТКА СТОИТ (или движется очень медленно) ===
            // Используем логику "Куда попал":
            // Удар в центр -> Прямо. Удар в край -> В бок.
            
            FVector PaddleOrigin = HitActor->GetActorLocation();
            FVector HitPoint = ImpactResult.ImpactPoint;
            float OffsetY = HitPoint.Y - PaddleOrigin.Y;
            
            // Коэффициент угла от края (чем больше, тем сильнее угол при ударе краем стоячей каретки)
            float AngleStrength = 2.0f; 

            FVector BallDir = ProjectileMovement->Velocity;
            BallDir.Y += OffsetY * AngleStrength; // Смещаем угол
            BallDir.X = FMath::Abs(BallDir.X);    // Всегда вперед

            ProjectileMovement->Velocity = BallDir.GetSafeNormal() * NewSpeed;
        }
    }
    else
    {
        // СТЕНЫ / КИРПИЧИ
        ProjectileMovement->Velocity = ProjectileMovement->Velocity.GetSafeNormal() * NewSpeed;
    }

    // ОТРИСОВКА (Зеленый = Движение, Красный = Стоя)
    if (bShowTrajectoryDebug && GetWorld())
    {
        FVector Start = ImpactResult.ImpactPoint;
        FVector End = Start + (ProjectileMovement->Velocity.GetSafeNormal() * 300.0f);
        DrawDebugLine(GetWorld(), Start, End, bIsCurveApplied ? FColor::Green : FColor::Red, false, 2.0f, 0, 2.0f);
    }
}

// ... Остальные функции (ApplySpeedBuff, ResetSpeed, SizeBuff, ResetSize) без изменений ...

void AArkanoidBall::ApplySpeedBuff(float SpeedMultiplier, float BuffDurationInSeconds)
{
    if (!ProjectileMovement) return;

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

    float CurrentVelocityMag = ProjectileMovement->Velocity.Size();
    float TargetSpeed = CurrentVelocityMag * SpeedMultiplier;
    TargetSpeed = FMath::Clamp(TargetSpeed, MinSpeed, MaxSpeed);

    ProjectileMovement->Velocity = ProjectileMovement->Velocity.GetSafeNormal() * TargetSpeed;

    if (GEngine)
    {
       FString EndMsg = FString::Printf(TEXT("BALL RESULT: NewMax=%f, NewRealSpeed=%f"), MaxSpeed, ProjectileMovement->Velocity.Size());
       GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Magenta, EndMsg);
    }

    GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpeedBuff, this, &AArkanoidBall::ResetSpeedToNormal, BuffDurationInSeconds, false);
    
    if (AArkanoidGameState* GS = GetWorld()->GetGameState<AArkanoidGameState>())
    {
       GS->StartBonusTimer(AArkanoidBonus_Speed::StaticClass(), BuffDurationInSeconds);
    }
}

void AArkanoidBall::ResetSpeedToNormal()
{
    if (!ProjectileMovement) return;

    MaxSpeed = CachedBaseMaxSpeed;
    MinSpeed = CachedBaseMinSpeed;
    ProjectileMovement->MaxSpeed = MaxSpeed;

    float CurrentVelocityMag = ProjectileMovement->Velocity.Size();
    
    if (CurrentVelocityMag > MaxSpeed)
    {
       ProjectileMovement->Velocity = ProjectileMovement->Velocity.GetSafeNormal() * MaxSpeed;
    }
    else if (CurrentVelocityMag < MinSpeed)
    {
       ProjectileMovement->Velocity = ProjectileMovement->Velocity.GetSafeNormal() * MinSpeed;
    }

    CachedBaseMaxSpeed = 0.0f;
    CachedBaseMinSpeed = 0.0f;
    UE_LOG(LogTemp, Warning, TEXT("BONUS END: Speed reset to normal."));
}

void AArkanoidBall::ApplySizeBuff(float SizeMultiplier, float BuffDuration)
{
    GetWorld()->GetTimerManager().ClearTimer(TimerHandle_SizeBuff);

    if (CachedBaseScale.IsZero())
    {
       CachedBaseScale = GetActorScale3D();
    }

    FVector NewScale = CachedBaseScale * SizeMultiplier;
    SetActorScale3D(NewScale);

    UE_LOG(LogTemp, Warning, TEXT("BONUS: Size x%f! New Scale: %s"), SizeMultiplier, *NewScale.ToString());

    GetWorld()->GetTimerManager().SetTimer(TimerHandle_SizeBuff, this, &AArkanoidBall::ResetSizeToNormal, BuffDuration, false);

    if (AArkanoidGameState* GS = GetWorld()->GetGameState<AArkanoidGameState>())
    {
       GS->StartBonusTimer(AArkanoidBonus_Size::StaticClass(), BuffDuration);
    }
}

void AArkanoidBall::ResetSizeToNormal()
{
    if (!CachedBaseScale.IsZero())
    {
       SetActorScale3D(CachedBaseScale);
    }
    CachedBaseScale = FVector::ZeroVector;
    UE_LOG(LogTemp, Warning, TEXT("BONUS END: Size reset."));
}