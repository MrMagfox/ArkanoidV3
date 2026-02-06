

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ArkanoidHUD.generated.h"


UCLASS()
class ARKANOIDV3_API AArkanoidHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	// --- НАСТРОЙКИ ---

	/** Класс виджета, который мы хотим создать (выбираем WBP_GameHUD в блюпринте) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Arkanoid | UI")
	TSubclassOf<UUserWidget> GameWidgetClass;

	// --- СОСТОЯНИЕ ---

	/** Ссылка на созданный виджет, чтобы мы могли к нему обращаться */
	UPROPERTY(BlueprintReadOnly, Category = "Arkanoid | UI")
	TObjectPtr<UUserWidget> GameWidget;
};
