// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "ArkanoidCamera.generated.h"

UCLASS()
class ARKANOIDV3_API AArkanoidCamera : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AArkanoidCamera();

protected:
	// Вызов при создании актора в редакторе или при спавне в игре
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	// Вызывается каждый кадр 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arkanoid | Components")
	TObjectPtr<USceneComponent> SceneRoot;
	// Камера
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Arkanoid | Components")
	TObjectPtr<UCameraComponent> CameraComp;

	// Параметры для настройки прямо в эдиторе
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Camera Settings", meta = (MakeEditWidget = true))
	FVector CameraPosition;
	// Поворот камеры
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arkanoid | Camera Settings")
	FRotator CameraRotation;
};
