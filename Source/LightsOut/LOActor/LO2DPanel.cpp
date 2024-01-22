// Fill out your copyright notice in the Description page of Project Settings.


#include "LO2DPanel.h"
#include "PaperSpriteComponent.h"

// Sets default values
ALO2DPanel::ALO2DPanel()
{
    PrimaryActorTick.bCanEverTick = false;

    SpriteComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("SpriteComponent"));
    RootComponent = SpriteComponent;
}

// Called when the game starts or when spawned
void ALO2DPanel::BeginPlay()
{
	Super::BeginPlay();
	
}