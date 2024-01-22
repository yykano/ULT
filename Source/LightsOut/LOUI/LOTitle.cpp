// Fill out your copyright notice in the Description page of Project Settings.


#include "LOTitle.h"
#include "LightsOut/LightsOut.h"
#include "LightsOut/LOCore/LOGameState.h"

void ULOTitle::StartGame()
{
    TObjectPtr<ALOGameState> gameState = GetWorld()->GetGameState<ALOGameState>();
    if (gameState)
    {
        gameState->StartGame();
    }
}
