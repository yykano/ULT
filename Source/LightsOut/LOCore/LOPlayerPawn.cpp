// Fill out your copyright notice in the Description page of Project Settings.


#include "LOPlayerPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ALOPlayerPawn::ALOPlayerPawn()
{
    // DefaultSceneRoot
    DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
    RootComponent = DefaultSceneRoot;

    // CameraArm
    CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
    CameraArm->SetupAttachment(RootComponent);

    // Camera
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    Camera->SetupAttachment(CameraArm);

    StageSize = 0;
}

void ALOPlayerPawn::BeginPlay()
{
    FViewport::ViewportResizedEvent.AddUObject(this, &ALOPlayerPawn::OnViewportResized);
}

// Viewportのサイズが変更されたときのコールバック
void ALOPlayerPawn::OnViewportResized(FViewport* viewport, uint32 _) const
{
    ALOPlayerPawn::SetCameraOrthoWidthByStageSize(viewport->GetSizeXY());
}

// ステージのサイズを元に、カメラが映す範囲を設定する
void ALOPlayerPawn::SetStageSizeAndCameraOrthoWidth(float SetStageSize)
{
    StageSize = SetStageSize;

    if (UGameViewportClient* GameViewport = GEngine->GameViewport)
    {
        FIntPoint ScreenSize = GameViewport->Viewport->GetSizeXY();
        ALOPlayerPawn::SetCameraOrthoWidthByStageSize(ScreenSize);
    }
}

// ステージサイズからカメラの範囲を設定する
void ALOPlayerPawn::SetCameraOrthoWidthByStageSize(FIntPoint ViewportSize) const
{
    if (!Camera)
    {
        return;
    }

    if (ViewportSize.X <= 0 || ViewportSize.Y <= 0)
    {
        return;
    }

    float aspectRatio = (float)ViewportSize.X / ViewportSize.Y;
    float orthoWidth = StageSize + ViewportMargin;

    UE_LOG(LogTemp, Log, TEXT("[ALOPlayerPawn::SetCameraOrthoWidthByStageSize] x:%d, y:%d, aspectRatio:%f"),
        ViewportSize.X,
        ViewportSize.Y,
        aspectRatio);

    // 横幅のほうが広い場合は、縦に見切れてしまうので補正する
    if (aspectRatio < 1)
    {
        Camera->OrthoWidth = orthoWidth;
    }
    // 縦幅のほうが広い場合は、OrthoWidthにステージサイズをそのまま設定
    else
    {
        Camera->OrthoWidth = orthoWidth * aspectRatio;
    }
}