// Fill out your copyright notice in the Description page of Project Settings.


#include "LightsOut/LOCore/LOHUD.h"
#include "LightsOut/LOCore/LOGameState.h"
#include "LightsOut/LOUI/LOStageClear.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void ALOHUD::BeginPlay()
{
    TObjectPtr<ALOGameState> gameState = GetWorld()->GetGameState<ALOGameState>();
    TObjectPtr<ALOPlayerController> playerControlelr = (ALOPlayerController*)GetOwningPlayerController();

    // GameStateに各イベントを登録
    gameState->OnGameStartEvents.AddUObject(this, &ALOHUD::OnStartGame);
    gameState->OnInputLockChangedEvents.AddUObject(this, &ALOHUD::OnInputLockChanged);
    gameState->OnStageClearEvents.AddUObject(this, &ALOHUD::OnStageClear);

    // Title作成
    if (TitleUIClass)
    {
        TitleUI = (ULOTitle*)UWidgetBlueprintLibrary::Create(GetWorld(), TitleUIClass, GetOwningPlayerController());
        TitleUI->AddToViewport();
        TitleUI->SetVisibility(ESlateVisibility::Visible);
    }

    // Main作成
    if (MainUIClass)
    {
        MainUI = (ULOMainUI*)UWidgetBlueprintLibrary::Create(GetWorld(), MainUIClass, GetOwningPlayerController());
        MainUI->AddToViewport();
        MainUI->SetVisibility(ESlateVisibility::Collapsed);
    }

    // ClearUI作成
    if (StageClearUIClass)
    {
        StageClearUI = (ULOStageClear*)UWidgetBlueprintLibrary::Create(GetWorld(), StageClearUIClass, GetOwningPlayerController());
        StageClearUI->AddToViewport(StageClearZOrder);
        StageClearUI->SetVisibility(ESlateVisibility::Collapsed);
    }

    // InputLock作成
    if (InputLockUIClass)
    {
        InputLockUI = UWidgetBlueprintLibrary::Create(GetWorld(), InputLockUIClass, GetOwningPlayerController());
        InputLockUI->AddToViewport(InputLockZOrder);
        InputLockUI->SetVisibility(ESlateVisibility::Collapsed);
    }

    // PauseMenu
    if (PauseMenuUIClass)
    {
        PauseMenuUI = UWidgetBlueprintLibrary::Create(GetWorld(), PauseMenuUIClass, GetOwningPlayerController());
        PauseMenuUI->AddToViewport(PauseMenuZOrder);
        PauseMenuUI->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void ALOHUD::OnStartGame() const
{
    if (TitleUI)
    {
        TitleUI->PlayFadeOut();
    }

    if (MainUI)
    {
        MainUI->PlayFadeIn();
    }
}

void ALOHUD::ShowPauseMenu() const
{
    if (PauseMenuUI)
    {
        PauseMenuUI->SetVisibility(ESlateVisibility::Visible);
    }
}

void ALOHUD::OnInputLockChanged(bool IsLock) const
{
    if (InputLockUI)
    {
        InputLockUI->SetVisibility(IsLock ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

void ALOHUD::OnStageClear() const
{
    if (StageClearUI)
    {
        StageClearUI->PlayStageClearAnimation([]
        {
            UE_LOG(LogTemp, Log, TEXT("StageClearAnimation Completed"));
        });
    }
}