// Fill out your copyright notice in the Description page of Project Settings.

#include "LOGameState.h"
#include "LightsOut/LightsOut.h"
#include "LOPlayerPawn.h"
#include "LOPlayerController.h"
#include "LightsOut/LOActor/LOPanel.h"
#include "LightsOut/LOStruct/LOPanelRow.h"
#include "LightsOut/LOUI/LOMainUI.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerState.h"
#include "LOHUD.h"

void ALOGameState::StartGame()
{
    // InputLock初期化
    SetInputLock(true);

    // 初回実行処理キュー
    TSharedRef<FLOActionQueue> actionQueue = MakeShared<FLOActionQueue>();

    // ステージ作成
    actionQueue->Enqueue([this, actionQueue]
    {
        if (OnGameStartEvents.IsBound())
        {
            OnGameStartEvents.Broadcast();
        }

        StageCreate(CurrentStageLevel, [actionQueue] { actionQueue->NextAction(); });
    });

    // ステージ初期化
    actionQueue->Enqueue([this, actionQueue]
    {
        StageReset([actionQueue] { actionQueue->NextAction(); });
    });

    // InputLock解除
    actionQueue->Enqueue([this, actionQueue]
    {
        SetInputLock(false);
        actionQueue->NextAction();
    });

    // 初回実行
    actionQueue->ExecuteFirstAction();
}

// ステージを生成する
void ALOGameState::StageCreate(int Level, TFunction<void(void)>&& Callback)
{
    CurrentStageLevel = Level;
    StepCount = 0;

    UWorld* world = GetWorld();

    // パネルBPが設定されていない
    if (!PanelClass)
    {
        UE_LOG(LogTemp, Error, TEXT("PanelClass not valid"));
        return;
    }

    // PlayerPawn（カメラの制御と、各パネルの座標として使用する）
    APlayerState* pState = PlayerArray[0];
    ALOPlayerPawn* pawn = (ALOPlayerPawn*)pState->GetPawn();
    check(pawn);

    // カメラサイズの設定
    const float stageSize = (PanelSize * GetStageSize()) + (PanelInterval + (GetStageSize() - 1));
    pawn->SetStageSizeAndCameraOrthoWidth(stageSize);

    // パネル配列の初期化
    Panels.Reset();

    // Pawnの座標をパネルの生成位置とする
    FTransform pawnTransform = pawn->GetTransform();

    // 全パネルの中心がPawnの座標となるように、全体的に位置をずらす
    const float locationOffset = (PanelSize + PanelInterval) * ((GetStageSize() - 1) / 2.0);

    // Z軸（縦軸)
    for (int col = 0; col < GetStageSize(); col++)
    {
        Panels.Add(FLOPanelRow());

        // X軸（横軸）
        for (int row = 0; row < GetStageSize(); row++)
        {
            FTransform panelTransform = pawnTransform;
            FVector location = panelTransform.GetLocation();
            location.X += (PanelSize + PanelInterval) * row - locationOffset;
            location.Z += (PanelSize + PanelInterval) * col - locationOffset;
            panelTransform.SetLocation(location);

            ALOPanel* panel = (ALOPanel*)world->SpawnActor(PanelClass, &panelTransform);
            Panels[col].Panels.Add(panel);
            panel->SetPos(FIntPoint(row, col));
            panel->InitFlip(false);
            panel->InitFade(false);
            panel->OnClickEvent.BindLambda([this](FIntPoint pos)
            {
                FlipPanel(GetPanel(pos), false);
            });
        }
    }

    // ステージ情報作成
    InitialFlips.Reset();
    while (InitialFlips.Num() < GetStageSize())
    {
        // 乱数
        const auto x = FMath::RandRange(0, GetStageSize() - 1);
        const auto y = FMath::RandRange(0, GetStageSize() - 1);
        auto pos = FIntPoint(x, y);

        // 同じパネルを二度反転させない
        if (!InitialFlips.Contains(pos))
        {
            InitialFlips.AddUnique(pos);
        }
    }

    // 順番にフェードさせていく
    int count = 0;
    float fadeDelay = PanelFadeTime / GetPanelNum();
    for (FLOPanelRow row : Panels)
    {
        for (TObjectPtr<ALOPanel> panel : row.Panels)
        {
            count++;

            // 最初の要素は遅延なしで実行
            if (count == 1)
            {
                panel->PlayFade(true);
                continue;
            }

            // フェードを開始する時間
            float fadeTime = count * fadeDelay;

            // それぞれのパネルを指定時間後にフェードさせる
            FTimerHandle fadeHandle;
            world->GetTimerManager().SetTimer(fadeHandle, [this, panel]()
            {
                panel->PlayFade(true);

            }, fadeTime, false);
        }
    }

    // すべてのフェードが完了するタイミングで完了コールバックを実行
    FTimerHandle lastHandle;
    world->GetTimerManager().SetTimer(lastHandle, [this, Callback]()
    {
        if (Callback)
            Callback();

    }, PanelFadeTime, false);
}

// ステージを初期状態に戻す
void ALOGameState::StageReset(TFunction<void(void)>&& Callback)
{
    StepCount = 0;
    ALOGameState::StageReset_Impl(0, MoveTemp(Callback));
}

// ResetStageから実行される再帰関数（直接実行することはない）
void ALOGameState::StageReset_Impl(const int Count, TFunction<void(void)>&& Callback)
{
    // 完了コールバック
    if (Count >= InitialFlips.Num())
    {
        if (Callback)
            Callback();

        return;
    }

    // ワールド取得
    UWorld* world = GetWorld();

    // 初回
    if (Count <= 0)
    {
        // すでに実行中なら停止させる
        if (StageResetTimerHandle.IsValid())
        {
            world->GetTimerManager().ClearTimer(StageResetTimerHandle);
        }

        // すべてのパネルをOFFに
        for (FLOPanelRow row : Panels)
        {
            for (TObjectPtr<ALOPanel> panel : row.Panels)
            {
                panel->InitFlip(false);
            }
        }
    }

    // パネルの取得
    FIntPoint pos = InitialFlips[Count];
    TObjectPtr<ALOPanel> panel = GetPanel(pos);

    // フリップ実行
    FlipPanel(panel, true, [this, Count, Callback] () mutable
    {
        // 次のフリップまでの待機時間
        float delayTime = StageResetTime - (CurrentStageLevel * StageResetSpeed);

        // 即時実行（SetTimerの時間指定が0以下になるとタイマーは実行されない）
        if (delayTime <= 0)
        {
            StageReset_Impl(Count + 1, MoveTemp(Callback));
            return;
        }

        // 数秒待機（レベルごとに早くなる）
        FTimerHandle timerHandle;
        GetWorld()->GetTimerManager().SetTimer(timerHandle, [this, Count, Callback]() mutable
        {
            StageReset_Impl(Count + 1, MoveTemp(Callback));

        }, delayTime, false);
    });
}

// ステージを破棄する
void ALOGameState::StageDestroy(TFunction<void(void)>&& Callback)
{
    UWorld* world = GetWorld();

    // 順番にフェードさせていく
    int count = 0;
    float fadeDelay = PanelFadeTime / GetPanelNum();
    for (FLOPanelRow row : Panels)
    {
        for (TObjectPtr<ALOPanel> panel : row.Panels)
        {
            count++;

            // 最初の要素は遅延なしで実行
            if (count == 1)
            {
                panel->PlayFade(false);
                continue;
            }

            // フェードを開始する時間
            float fadeTime = count * fadeDelay;

            // それぞれのパネルを指定時間後にフェードさせる
            FTimerHandle fadeHandle;
            world->GetTimerManager().SetTimer(fadeHandle, [this, panel]()
            {
                panel->PlayFade(false, [panel]
                {
                    panel->Destroy();
                });

            }, fadeTime, false);
        }
    }

    // すべてのフェードが完了するタイミングで次ステージの作成
    FTimerHandle lastHandle;
    world->GetTimerManager().SetTimer(lastHandle, [this, Callback]()
    {
        if (Callback)
            Callback();

    }, PanelFadeTime, false);
}

// パネルをフリップする
void ALOGameState::FlipPanel(TObjectPtr<ALOPanel> Panel, bool IsSystemFlip, TFunction<void(void)>&& Callback)
{
    // 入力禁止 && システムによる操作ではない
    if (bIsInputLock && !IsSystemFlip)
    {
        return;
    }

    // プレイヤーによるフリップの場合はステップ数加算
    if (!IsSystemFlip)
    {
        StepCount++;
    }

    // 中心
    Panel->PlayFlip(Callback);
    FIntPoint pos = Panel->GetPos();

    // 左
    if (pos.X > 0)
    {
        FIntPoint leftPos = pos;
        leftPos.X -= 1;
        GetPanel(leftPos)->PlayFlip();
    }

    // 右
    if (pos.X + 1 < GetStageSize())
    {
        FIntPoint rightPos = pos;
        rightPos.X += 1;
        GetPanel(rightPos)->PlayFlip();
    }

    // 下
    if (pos.Y > 0)
    {
        FIntPoint topPos = pos;
        topPos.Y -= 1;
        GetPanel(topPos)->PlayFlip();
    }

    // 下
    if (pos.Y + 1 < GetStageSize())
    {
        FIntPoint topPos = pos;
        topPos.Y += 1;
        GetPanel(topPos)->PlayFlip();
    }

    // ユーザーによるフリップ実行であればクリアチェック
    if (!IsSystemFlip)
    {
        // すべてのパネルの状態をチェック
        bool isAllLightsOut = true;
        for (FLOPanelRow row : Panels)
        {
            for (TObjectPtr<ALOPanel> panel : row.Panels)
            {
                isAllLightsOut &= !panel->GetIsLightOn();
            }
        }

        // すべてOFFならクリア
        if (isAllLightsOut)
        {
            // ステージが再構築されるまで入力させない
            SetInputLock(true);

            // パネルのフリップ演出が完了するのを待ってから完了処理
            TSharedRef<FDelegateHandle> clearDelegateHandle = MakeShared<FDelegateHandle>();
            *clearDelegateHandle = Panel->OnFlipedEvent.AddLambda([this, Panel, clearDelegateHandle]()
            {
                StageClear();
                Panel->OnFlipedEvent.Remove(*clearDelegateHandle);
            });
        }
    }
}

// クリア時
void ALOGameState::StageClear()
{
    // クリアイベント実行（待機は不要）
    if (OnStageClearEvents.IsBound())
        OnStageClearEvents.Broadcast();

    // 初回実行処理キュー
     TSharedRef<FLOActionQueue> actionQueue = MakeShared<FLOActionQueue>();

    // ステージ作成
    actionQueue->Enqueue([this, actionQueue]
    {
        StageDestroy([actionQueue] {actionQueue->NextAction(); });
    });

    // ステージ作成
    actionQueue->Enqueue([this, actionQueue]
    {
        StageCreate(CurrentStageLevel + 1, [actionQueue] {actionQueue->NextAction(); });
    });

    // ステージ初期化
    actionQueue->Enqueue([this, actionQueue]
    {
        StageReset([actionQueue] {actionQueue->NextAction(); });
    });

    // InputLock解除
    actionQueue->Enqueue([this, actionQueue]
    {
        SetInputLock(false);
    });

    // 初回実行
    actionQueue->ExecuteFirstAction();
}