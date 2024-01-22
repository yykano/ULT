// Fill out your copyright notice in the Description page of Project Settings.


#include "LOMainUI.h"
#include "LightsOut/LOCore/LOHUD.h"
#include "LightsOut/LOCore/LOGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void ULOMainUI::StageReset()
{
    ALOGameState* gameState = (ALOGameState*)UGameplayStatics::GetGameState(this);
    if (gameState)
    {
        gameState->SetInputLock(true);
        gameState->StageReset([gameState]()
        {
            gameState->SetInputLock(false);
        });
    }
}

void ULOMainUI::Pause() const
{
    ALOHUD* hud = (ALOHUD*)GetOwningPlayer()->GetHUD();
    if (hud)
    {
        hud->ShowPauseMenu();
    }
}