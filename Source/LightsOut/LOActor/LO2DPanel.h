// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LO2DPanel.generated.h"

class UPaperSpriteComponent;

UCLASS()
class LIGHTSOUT_API ALO2DPanel : public AActor
{
	GENERATED_BODY()

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LO")
    TObjectPtr<UPaperSpriteComponent> SpriteComponent;
	
public:	
	// Sets default values for this actor's properties
	ALO2DPanel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
