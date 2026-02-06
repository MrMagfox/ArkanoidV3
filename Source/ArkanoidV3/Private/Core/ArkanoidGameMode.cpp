#include "Core/ArkanoidGameMode.h" 
#include "Core/ArkanoidHUD.h"
#include "Pawn/ArkanoidPaddle.h"
#include "Actor/ArkanoidCamera.h" 
#include "Core/ArkanoidGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Actor/ArkanoidBall.h" 
#include "Core/ArkanoidGameInstance.h" 
#include "Actor/ArkanoidBricks.h" 

AArkanoidGameMode::AArkanoidGameMode()
{
    DefaultPawnClass = AArkanoidPaddle::StaticClass();
    GameStateClass = AArkanoidGameState::StaticClass();
    HUDClass = AArkanoidHUD::StaticClass();
}

void AArkanoidGameMode::StartPlay()
{
    Super::StartPlay();

    if (AArkanoidGameState* GS = GetGameState<AArkanoidGameState>())
    {
       GS->SetLives(MaxLives);
    }

    AActor* FoundCamera = UGameplayStatics::GetActorOfClass(GetWorld(), GameCameraClass);
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC && FoundCamera)
    {
       PC->SetViewTargetWithBlend(FoundCamera, 0.0f);
    }

    //ActiveBricks.Empty();
}

void AArkanoidGameMode::OnBallLost()
{
    TArray<AActor*> ActiveBalls;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AArkanoidBall::StaticClass(), ActiveBalls);

    if (ActiveBalls.Num() > 1) return; 

    if (AArkanoidGameState* GS = GetGameState<AArkanoidGameState>())
    {
       GS->ClearAllBonuses();
       int32 CurrentLives = GS->GetCurrentLives();
       CurrentLives--;
       GS->SetLives(CurrentLives);

       if (CurrentLives > 0) RespawnBallForPlayer();
       else UE_LOG(LogTemp, Error, TEXT("GAME OVER!"));
    }
}

void AArkanoidGameMode::RespawnBallForPlayer()
{
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
       if (AArkanoidPaddle* Paddle = Cast<AArkanoidPaddle>(PC->GetPawn()))
       {
          Paddle->RespawnBall();
       }
    }
}

void AArkanoidGameMode::RegisterBrick(AActor* BrickActor)
{
    if (BrickActor) ActiveBricks.AddUnique(BrickActor);
}

bool AArkanoidGameMode::HasBricksInPhase(FName PhaseName)
{
    FString SearchString = PhaseName.ToString();

    for (AActor* Brick : ActiveBricks)
    {
       if (!Brick) continue;
       ULevel* ActorLevel = Brick->GetLevel();
       if (ActorLevel)
       {
          // Получаем имя пакета уровня и ищем в нем наше имя
          FString LevelPackageName = ActorLevel->GetOuter()->GetName();
          if (LevelPackageName.Contains(SearchString)) return true; 
       }
    }
    return false;
}

void AArkanoidGameMode::NotifyBrickDestroyed(AActor* BrickActor)
{
    if (BrickActor)
    {
       ActiveBricks.Remove(BrickActor);

       // 1. ПОЛНАЯ ПОБЕДА
       if (ActiveBricks.Num() <= 0)
       {
          UE_LOG(LogTemp, Warning, TEXT("VICTORY! All bricks destroyed."));
          
          if (UArkanoidGameInstance* GI = Cast<UArkanoidGameInstance>(GetGameInstance()))
          {
             GI->CompleteLevel(CurrentLevelIndex);
          }

          TArray<AActor*> ActiveBalls;
          UGameplayStatics::GetAllActorsOfClass(GetWorld(), AArkanoidBall::StaticClass(), ActiveBalls);
          for (AActor* BallActor : ActiveBalls) { BallActor->SetActorTickEnabled(false); }

          GetWorldTimerManager().SetTimer(TimerHandle_WinDelay, this, &AArkanoidGameMode::GoToMainMenu, 1.0f, false);
          return;
       }

       // 2. УМНАЯ ПРОВЕРКА ФАЗ (ОБНОВЛЕНО ПОД АССЕТЫ)
       if (AArkanoidGameState* GS = GetGameState<AArkanoidGameState>())
       {
          // Получаем полные имена пакетов из ассетов, которые мы настроили в Setup
          FString NameA = GS->PhaseAsset_A.GetLongPackageName();
          FString NameB = GS->PhaseAsset_B.GetLongPackageName();

          // Проверяем наличие кирпичей, используя полные пути как имена
          bool bHasA = HasBricksInPhase(FName(*NameA));
          bool bHasB = HasBricksInPhase(FName(*NameB));

          // Сценарий 1: Фаза А пуста, но Б еще жива
          if (!bHasA && bHasB)
          {
             // Если мы НЕ на уровне Б -> переключаемся
             if (GS->CurrentActiveAsset.GetLongPackageName() != NameB)
             {
                 UE_LOG(LogTemp, Warning, TEXT("Phase A Cleared! Locking to Phase B."));
                 GS->StopAutoPhaseSwitching();
                 GS->SwitchToLevelByAsset(GS->PhaseAsset_B);
             }
          }
          // Сценарий 2: Фаза Б пуста, но А еще жива
          else if (!bHasB && bHasA)
          {
             // Если мы НЕ на уровне А -> переключаемся
             if (GS->CurrentActiveAsset.GetLongPackageName() != NameA)
             {
                 UE_LOG(LogTemp, Warning, TEXT("Phase B Cleared! Locking to Phase A."));
                 GS->StopAutoPhaseSwitching();
                 GS->SwitchToLevelByAsset(GS->PhaseAsset_A);
             }
          }
       }
    }
}

void AArkanoidGameMode::GoToMainMenu()
{
    UGameplayStatics::OpenLevel(this, FName("MainMenu"));
}

void AArkanoidGameMode::RecountBricks() {}