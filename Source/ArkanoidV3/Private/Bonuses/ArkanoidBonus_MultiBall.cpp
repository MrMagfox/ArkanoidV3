// Fill out your copyright notice in the Description page of Project Settings.


#include "Bonuses/ArkanoidBonus_MultiBall.h"
#include "Actor/ArkanoidBall.h"
#include "Pawn/ArkanoidPaddle.h"
#include "GameFramework/ProjectileMovementComponent.h"

void AArkanoidBonus_MultiBall::ActivateBonusEffect(AArkanoidPaddle* PlayerPawn)
{
    if (!PlayerPawn) return;

    // 1. Получаем оригинал, чтобы знать, где спавнить
    AArkanoidBall* OriginalBall = PlayerPawn->GetCurrentBall();
    if (!OriginalBall) return; // Если мяча нет, ничего не делаем

    // Получаем текущую скорость и позицию оригинала
    FVector OriginLoc = OriginalBall->GetActorLocation();
    FVector OriginVel = OriginalBall->GetVelocity();
    
    // Если мяч вдруг стоит на месте (залип), дадим ему пинок вверх
    if (OriginVel.IsZero())
    {
        OriginVel = FVector(1000.f, 0.f, 0.f);
    }

    // Цикл спавна клонов
    for (int32 i = 0; i < ExtraBallsCount; i++)
    {
        FActorSpawnParameters SpawnParams; // Параметры спавна
        SpawnParams.Owner = PlayerPawn; // Владелец - наша платформа
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // Алгоритм обработки коллизий

        // Спавним новый мяч в той же точке
        AArkanoidBall* NewBall = GetWorld()->SpawnActor<AArkanoidBall>(OriginalBall->GetClass(), OriginLoc, FRotator::ZeroRotator, SpawnParams);

        if (NewBall)
        {
            // Вычисляем новый вектор скорости (поворот вектора)
            // Если i=0 (первый клон) -> поворачиваем на +Angle
            // Если i=1 (второй клон) -> поворачиваем на -Angle
            // Для более чем 2 шаров можно усложнить формулу, но для 2 работает идеально.
            
            float AngleModifier = (i % 2 == 0) ? SpreadAngle : -SpreadAngle;
            
            // Поворачиваем вектор скорости вокруг оси Z (UpVector)
            FVector NewVelocity = OriginVel.RotateAngleAxis(AngleModifier, FVector::UpVector);

            // Применяем скорость к новому мячу
            if (UProjectileMovementComponent* PMC = NewBall->FindComponentByClass<UProjectileMovementComponent>())
            {
                PMC->Velocity = NewVelocity;
            }
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("MULTI-BALL ACTIVATED! Spawned %d balls."), ExtraBallsCount);
}