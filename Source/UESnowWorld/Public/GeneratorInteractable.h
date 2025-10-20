// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GeneratorInteractable.generated.h"
/**
 * 
 */
UINTERFACE(Blueprintable)
class UGeneratorInteractable : public UInterface
{
    GENERATED_BODY()
};

class IGeneratorInteractable
{
    GENERATED_BODY()
    
public:
    // 보스가 제네레이터를 방해할 때 호출
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void ReduceProgress(float Amount); // 0~1 스케일 가정(확실하지 않음)
};
