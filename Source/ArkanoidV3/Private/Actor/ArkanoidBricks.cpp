#include "Actor/ArkanoidBricks.h"
#include "Actor/ArkanoidBall.h"
#include "Core/ArkanoidGameState.h"
#include "Bonuses/ArkanoidBonus.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Engine/World.h"
#include "Core/ArkanoidGameMode.h" 

AArkanoidBricks::AArkanoidBricks()
{
    PrimaryActorTick.bCanEverTick = false;

    BrickMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BrickMesh"));
    RootComponent = BrickMesh;

    BrickMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    BrickMesh->SetNotifyRigidBodyCollision(true); 
}

void AArkanoidBricks::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    UpdateVisuals();
}

void AArkanoidBricks::BeginPlay()
{
    Super::BeginPlay();

    // --- ЗАЩИТА ОТ БАГОВ КОЛЛИЗИИ ---
    if (BrickMesh)
    {
        // Принудительно ставим настройки при старте уровня
        BrickMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        BrickMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
        
        // Переподписываемся на удар
        BrickMesh->OnComponentHit.RemoveDynamic(this, &AArkanoidBricks::OnBrickHit);
        BrickMesh->OnComponentHit.AddDynamic(this, &AArkanoidBricks::OnBrickHit);
    }

    // 2. РЕГИСТРАЦИЯ В GAMEMODE (ОБНОВЛЕНО)
    // Передаем 'this', чтобы добавить именно ЭТОТ кирпич в массив
    if (AArkanoidGameMode* GM = GetWorld()->GetAuthGameMode<AArkanoidGameMode>())
    {
        GM->RegisterBrick(this);
    }
    
    UpdateVisuals();
}

void AArkanoidBricks::OnBrickHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && OtherActor->IsA(AArkanoidBall::StaticClass()))
    {
        HandleBrickHit();
    }
}

void AArkanoidBricks::HandleBrickHit()
{
    HitsToDestroy--;

    if (HitsToDestroy <= 0)
    {
        // 1. Начисляем очки
        if (AArkanoidGameState* GS = GetWorld()->GetGameState<AArkanoidGameState>())
        {
            GS->AddScore(PointsValue);
        }
        
        // 2. СООБЩАЕМ ГЕЙММОДУ О СМЕРТИ (ОБНОВЛЕНО)
        // Передаем 'this' для удаления из массива
        if (AArkanoidGameMode* GM = GetWorld()->GetAuthGameMode<AArkanoidGameMode>())
        {
            GM->NotifyBrickDestroyed(this);
        }

        // 3. ЛОГИКА СПАВНА БОНУСА 
        if (PossibleBonuses.Num() > 0)
        {
            const float RandomRoll = FMath::RandRange(0.0f, 100.0f);

            if (RandomRoll <= BonusDropChancePercentage)
            {
                const int32 SelectionIndex = FMath::RandRange(0, PossibleBonuses.Num() - 1);
                TSubclassOf<AArkanoidBonus> SelectedBonusClass = PossibleBonuses[SelectionIndex];

                if (SelectedBonusClass)
                {
                    FActorSpawnParameters SpawnParams;
                    SpawnParams.Owner = this; 
                    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

                    GetWorld()->SpawnActor<AArkanoidBonus>(
                        SelectedBonusClass,
                        GetActorLocation(),
                        GetActorRotation(),
                        SpawnParams
                    );
                }
            }
        }

        // 4. Уничтожаем кирпич
        Destroy();
    }
    else
    {
        // Меняем цвет, если еще жив
        UpdateVisuals();
    }
}

void AArkanoidBricks::UpdateVisuals()
{
    if (!BrickMesh || HealthMaterials.IsEmpty()) return;

    int32 MaterialIndex = FMath::Clamp(HitsToDestroy - 1, 0, HealthMaterials.Num() - 1);

    if (HealthMaterials.IsValidIndex(MaterialIndex) && HealthMaterials[MaterialIndex])
    {
        BrickMesh->SetMaterial(0, HealthMaterials[MaterialIndex]);
    }
}