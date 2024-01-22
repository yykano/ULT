// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LOMainUI.generated.h"

/**
 * 
 */
UCLASS()
class LIGHTSOUT_API ULOMainUI : public UUserWidget
{
	GENERATED_BODY()

    /* -----------------------------------
     * BP
     * ----------------------------------- */
public:

    UFUNCTION(BlueprintCallable, Category = "LO")
    void StageReset();

    UFUNCTION(BlueprintImplementableEvent, Category = "LO")
    void PlayFadeIn();

    UFUNCTION(BlueprintCallable, Category = "LO")
    void Pause() const;
};
