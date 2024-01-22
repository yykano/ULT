// Fill out your copyright notice in the Description page of Project Settings.


#include "LOPanel.h"
#include "Components/StaticMeshComponent.h"
#include "PaperSpriteComponent.h"

// Sets default values
ALOPanel::ALOPanel()
{
    PrimaryActorTick.bCanEverTick = false;
    bIsLightOn = false;

    SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("SpriteComponent"));
    RootComponent = SpriteComponent;
}