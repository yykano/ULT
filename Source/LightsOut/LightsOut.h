// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LOLog, Log, All);

// デリゲートをキュー処理するためのクラス
struct FLOActionQueue
{
private:

    TArray<TFunction<void(void)>> ActionQueue;
    int QueueIndex;

public:

    void Reset()
    {
        ActionQueue.Reset();
        QueueIndex = 0;
    }

    void Enqueue(TFunction<void(void)>&& Action)
    {
        ActionQueue.Add(Action);
    }

    void NextAction()
    {
        if (QueueIndex >= ActionQueue.Num())
        {
            return;
        }

        TFunction<void(void)> Action = ActionQueue[QueueIndex];
        QueueIndex++;

        if (Action)
        {
            Action();
        }
        else
        {
            NextAction();
        }
    }

    void ExecuteFirstAction()
    {
        QueueIndex = 0;
        NextAction();
    }
};