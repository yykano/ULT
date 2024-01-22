// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LOPanel.generated.h"

DECLARE_DELEGATE_OneParam(FClickDelegate, const FIntPoint)

UCLASS()
class LIGHTSOUT_API ALOPanel : public AActor
{
	GENERATED_BODY()

    /* -----------------------------------
    * BP
    * ----------------------------------- */

public:

    // Sprite
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LO")
    TObjectPtr<class UPaperSpriteComponent> SpriteComponent;

    // パネルの座標
    UFUNCTION(BlueprintPure, Category = "LO")
    FIntPoint GetPos() const { return Point; }

    // パネルの状態を取得
    UFUNCTION(BlueprintPure, Category = "LO")
    bool GetIsLightOn() const { return bIsLightOn; }

protected:

    // パネルがクリックされた時のイベント
    UFUNCTION(BlueprintCallable, Category = "LO", meta = (BlueprintProtected))
    void OnClickPanel() const { OnClickEvent.ExecuteIfBound(Point); }

    // On/Off切り替え（完了時にOnFlipedを実行する必要がある）
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "LO")
    void Flip(bool IsLight, bool IsSkip = false);

    // On/OFF切り替え完了イベントを実行
    UFUNCTION(BlueprintCallable, Category = "LO", meta = (BlueprintProtected))
    void OnFliped()
    {
        if (OnFlipedEvent.IsBound())
        {
            OnFlipedEvent.Broadcast();
        }
    }

    // フェード（完了時にOnFadedを実行する必要がある）
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "LO")
    void Fade(bool IsShow, bool IsSkip = false);

    // フェード完了イベントを実行
    UFUNCTION(BlueprintCallable, Category = "LO", meta = (BlueprintProtected))
    void OnFaded()
    {
        if (OnFadedEvent.IsBound())
        {
            OnFadedEvent.Broadcast();
        }
    }

    /* -----------------------------------
    * C++
    * ----------------------------------- */

public:

    // クリック時のコールバック
    FClickDelegate OnClickEvent;

    // フリップ完了時のコールバック
    FSimpleMulticastDelegate OnFlipedEvent;

    // フェード完了時のコールバック
    FSimpleMulticastDelegate OnFadedEvent;

private:

    // 座標
    FIntPoint Point;

    // フリップ状態
    bool bIsLightOn;

public:

	// Sets default values for this actor's properties
	ALOPanel();

    // インデックスの初期化
    inline void SetPos(FIntPoint SetPoint)
    {
        Point = SetPoint;
    }

    // Off設定
    inline void InitFlip(bool isLightOn)
    {
        // すでにOFF
        if (bIsLightOn == isLightOn)
            return;

        bIsLightOn = isLightOn;
        Flip(isLightOn);
    };

    // On/Off切り替え
    FDelegateHandle PlayFlipDelegateHandle;
    inline void PlayFlip(TFunction<void(void)> Callback = nullptr)
    {
        // 重複登録チェック
        if (PlayFlipDelegateHandle.IsValid())
        {
            OnFlipedEvent.Remove(PlayFlipDelegateHandle);
        }

        // デリゲートを登録（1度実行されたら削除）
        if (Callback)
        {
            PlayFlipDelegateHandle = OnFlipedEvent.AddLambda([this, Callback]()
            {
                Callback();
                OnFlipedEvent.Remove(PlayFlipDelegateHandle);
            });
        }

        bIsLightOn = !bIsLightOn;
        Flip(bIsLightOn);
    };

    // 表示状態設定
    inline void InitFade(bool IsShow)
    {
        Fade(IsShow, true);
    }

    // 表示非表示切り替え
    FDelegateHandle PlayFadeDelegateHandle;
    inline void PlayFade(bool IsShow, TFunction<void(void)> Callback = nullptr)
    {
        // 重複登録チェック
        if (PlayFadeDelegateHandle.IsValid())
        {
            OnFadedEvent.Remove(PlayFadeDelegateHandle);
        }

        // デリゲートを登録（1度実行されたら削除）
        if (Callback)
        {
            PlayFadeDelegateHandle = OnFadedEvent.AddLambda([this, Callback]()
            {
                Callback();
                OnFadedEvent.Remove(PlayFadeDelegateHandle);
            });
        }

        Fade(IsShow, false);
    }
};
