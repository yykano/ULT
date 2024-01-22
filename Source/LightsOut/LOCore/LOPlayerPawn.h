// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "LOPlayerPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class LIGHTSOUT_API ALOPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ALOPlayerPawn();
    virtual void BeginPlay() override;

    // ステージのサイズを元に、カメラが映す範囲を設定する
    void SetStageSizeAndCameraOrthoWidth(float StageSize);

    // カメラ描画範囲の余白
    UPROPERTY(EditAnywhere, Category = "LO")
    float ViewportMargin;

protected:

    UPROPERTY(EditAnywhere, Category = "LO")
    TObjectPtr<USceneComponent> DefaultSceneRoot;

    UPROPERTY(EditAnywhere, Category = "LO")
    TObjectPtr<USpringArmComponent> CameraArm;

    UPROPERTY(EditAnywhere, Category = "LO")
    TObjectPtr<UCameraComponent> Camera;

private:

    // 現在のステージサイズ
    float StageSize;

    // Viewportのサイズが変更されたときのコールバック
    void OnViewportResized(FViewport* Viewport, uint32 _) const;

    // 実際にカメラに対してサイズの設定を行う
    void SetCameraOrthoWidthByStageSize(FIntPoint ViewportSize) const;
};
