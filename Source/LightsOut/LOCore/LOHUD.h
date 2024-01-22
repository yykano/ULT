// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LightsOut/LOCore/LOGameState.h"
#include "LightsOut/LOUI/LOTitle.h"
#include "LightsOut/LOUI/LOMainUI.h"
#include "LightsOut/LOUI/LOStageClear.h"
#include "LOHUD.generated.h"

/**
 * 
 */
UCLASS()
class LIGHTSOUT_API ALOHUD : public AHUD
{
	GENERATED_BODY()

private:

    // ステージクリアUI 
    UPROPERTY(EditDefaultsOnly, Category = "LO")
    TSubclassOf<ULOTitle> TitleUIClass;

    // メインUI 
    UPROPERTY(EditDefaultsOnly, Category = "LO")
    TSubclassOf<ULOMainUI> MainUIClass;

    // ステージクリアUI 
    UPROPERTY(EditDefaultsOnly, Category = "LO")
    TSubclassOf<ULOStageClear> StageClearUIClass;

    // InputLockUI 
    UPROPERTY(EditDefaultsOnly, Category = "LO")
    TSubclassOf<UUserWidget> InputLockUIClass;

    // PauseMenuUI 
    UPROPERTY(EditDefaultsOnly, Category = "LO")
    TSubclassOf<UUserWidget> PauseMenuUIClass;

public:

    // Title
    TObjectPtr<ULOTitle> TitleUI;

    // メインUI 
    TObjectPtr<ULOMainUI> MainUI;

    // ステージクリアUI 
    TObjectPtr<ULOStageClear> StageClearUI;

    // InputLock
    TObjectPtr<UUserWidget> InputLockUI;

    // PauseMenu
    TObjectPtr<UUserWidget> PauseMenuUI;

public:

    virtual void BeginPlay() override;

    void ShowPauseMenu() const;

private:

    const int InputLockZOrder = 1000;
    const int PauseMenuZOrder = 100;
    const int StageClearZOrder = 1;

    void OnStartGame() const;
    void OnInputLockChanged(bool isLock) const;
    void OnStageClear() const;
};
