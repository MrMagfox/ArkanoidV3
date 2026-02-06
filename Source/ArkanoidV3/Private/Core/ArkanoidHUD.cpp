


#include "ArkanoidV3/Public/Core/ArkanoidHUD.h"
#include "Blueprint/UserWidget.h"

void AArkanoidHUD::BeginPlay()
{
	Super::BeginPlay();

	// Проверяем, выбран ли класс виджета в блюпринте
	if (GameWidgetClass)
	{
		// Создаем виджет
		GameWidget = CreateWidget<UUserWidget>(GetWorld(), GameWidgetClass);

		if (GameWidget)
		{
			// Добавляем на экран
			GameWidget->AddToViewport();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ArkanoidHUD: GameWidgetClass is not selected!"));
	}
}