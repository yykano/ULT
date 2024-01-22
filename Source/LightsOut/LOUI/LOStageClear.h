// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LOStageClear.generated.h"

/**
 * 
 */
UCLASS()
class LIGHTSOUT_API ULOStageClear : public UUserWidget
{
	GENERATED_BODY()

protected:

    // アニメーション再生完了時にOnPlayAnimationFinishedを実行する必要がある
    UFUNCTION(BlueprintImplementableEvent, Category="LO", meta = (BlueprintProtected))
    void PlayClearAnimation();

    // アニメーション再生完了イベント
    UFUNCTION(BlueprintCallable, Category="LO", meta = (BlueprintProtected))
    void OnClearAnimationFinished()
    {
        AnimationFinishEvent.ExecuteIfBound();
        AnimationFinishEvent.Unbind();
    }

public:

    FSimpleDelegate AnimationFinishEvent;
    void PlayStageClearAnimation(TFunction<void(void)>&& Callback)
    {
        AnimationFinishEvent.BindLambda(Callback);
        PlayClearAnimation();
    }
};
