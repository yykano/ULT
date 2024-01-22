// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LOPanelRow.generated.h"

class ALOPanel;

/**
 * 
 */
USTRUCT()
struct FLOPanelRow
{
    GENERATED_BODY()

public:
    FLOPanelRow();
    TArray<TObjectPtr<ALOPanel>> Panels;
};