// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LOTitle.generated.h"

/**
 * 
 */
UCLASS()
class LIGHTSOUT_API ULOTitle : public UUserWidget
{
	GENERATED_BODY()

protected:

    UFUNCTION(BlueprintCallable, Category = "LO", meta = (BlueprintProtected))
    void StartGame();

public:

    // フェードアウト(AnimationFinishedを実行する必要がある)
    UFUNCTION(BlueprintImplementableEvent, Category = "LO")
    void PlayFadeOut();
};
