// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LightsOut/LOStruct/LOPanelRow.h"
#include "LOGameState.generated.h"

class ALOPanel;
class ALOPlayerController;
class ALOHUD;

DECLARE_MULTICAST_DELEGATE_OneParam(InputLockChangedDelegate, const bool);

/**
 * 
 */
UCLASS()
class LIGHTSOUT_API ALOGameState : public AGameStateBase
{
	GENERATED_BODY()

    /* -----------------------------------
     * UE
     * ----------------------------------- */

public:

    // 現在レベルのGetter
    UFUNCTION(BlueprintPure, Category = "LO")
    int GetCurrentLevel() { return CurrentStageLevel; }

    // 現在ステップのGetter
    UFUNCTION(BlueprintPure, Category = "LO")
    int GetStepCount() { return StepCount; }

private:

    // 生成するパネルのBPクラス
    UPROPERTY(EditAnywhere, Category = "LO")
    TSubclassOf<AActor> PanelClass;

    // 生成するパネル同士の間隔
    UPROPERTY(EditAnywhere, Category = "LO")
    float PanelInterval = 5;

    // ステージの生成/破棄にかける時間
    // パネルの枚数が何枚だろうと、この時間で演出が完了する
    UPROPERTY(EditAnywhere, Category = "LO")
    float PanelFadeTime = 1;

    // ステージのリセットにかける時間
    // ステージの構築回数分、演出時間は長くなる
    UPROPERTY(EditAnywhere, Category = "LO")
    float StageResetTime = 0.5f;

    // ステージのレベルが上がるごとにStageResetTimeを短くする時間
    UPROPERTY(EditAnywhere, Category = "LO")
    float StageResetSpeed = 0.05f;

    /* -----------------------------------
     * C++
     * ----------------------------------- */

private:

    // 生成するパネルのサイズ（PixelPerUnitなので100固定）
    const float PanelSize = 100;

    // 入力ロック
    bool bIsInputLock = false;

    // 現在のステージレベル
    int CurrentStageLevel = 1;

    // 現在のステップ数
    int StepCount = 0;

    // 各パネルアクタ
    TArray<FLOPanelRow> Panels;

    // ステージ構築時のフリップ情報
    TArray<FIntPoint> InitialFlips;

    // ステージリセットのタイマーハンドル
    FTimerHandle StageResetTimerHandle;

    // 指定した座標のパネル取得
    inline TObjectPtr<ALOPanel> GetPanel(FIntPoint Pos) const
    {
        return Panels[Pos.Y].Panels[Pos.X];
    }

    // マップのサイズ(1行あたりのパネル数)を取得
    // 4マスから始まり、3レベルごとに1マス増える
    inline int GetStageSize() const
    {
        return floor(CurrentStageLevel / 3) + 4;
    }

    // 全パネル数
    inline int GetPanelNum() const
    {
        return pow(GetStageSize(), 2);
    }

public:

    // ゲーム開始
    void StartGame();

    // ステージリセット
    void StageReset(TFunction<void(void)>&& Callback = nullptr);

    // 入力ロック
    InputLockChangedDelegate OnInputLockChangedEvents;
    void SetInputLock(bool isLock)
    {
        bIsInputLock = isLock;
        if (OnInputLockChangedEvents.IsBound())
        {
            OnInputLockChangedEvents.Broadcast(isLock);
        }
    }

    // ゲーム開始時イベント
    FTSSimpleMulticastDelegate OnGameStartEvents;

    // ステージクリア時イベント
    FTSSimpleMulticastDelegate OnStageClearEvents;

    // ポーズ
    FTSSimpleMulticastDelegate OnPauseEvents;

protected:

    // ステージ作成
    void StageCreate(int Level, TFunction<void(void)>&& Callback = nullptr);

    // ステージ破棄
    void StageDestroy(TFunction<void(void)>&& Callback = nullptr);

    // フリップ
    void FlipPanel(TObjectPtr<ALOPanel> PanelPos, bool IsSystemFlip, TFunction<void(void)>&& Callback = nullptr);

    // ステージクリア
    void StageClear();

private:

    // ステージリセット（再帰関数）
    void StageReset_Impl(const int Count, TFunction<void(void)>&& Callback);
};