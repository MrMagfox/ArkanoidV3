// Fill out your copyright notice in the Description page of Project Settings.


#include "ArkanoidV3/Public/Actor/ArkanoidCamera.h"


// Sets default values
AArkanoidCamera::AArkanoidCamera()
{
	
	PrimaryActorTick.bCanEverTick = false;
	// Инициализация компонентов
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;
	// Камера 
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SceneRoot);

	// Дефолтные значения: Высота 2000, смотрит вниз (-90 градусов по Y)
	// В UE5: X - Forward, Y - Right, Z - Up.
	// Чтобы смотреть сверху вниз на плоскость XY:
	CameraPosition = FVector(0.0f, 0.0f, 2000.0f);
	CameraRotation = FRotator(-90.0f, -90.0f, 0.0f);
}

void AArkanoidCamera::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (CameraComp)
	{
		CameraComp->SetRelativeLocation(CameraPosition);
		CameraComp->SetRelativeRotation(CameraRotation);
	}
	
}



