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
    // ������ ���׷����͸� ������ �� ȣ��
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void ReduceProgress(float Amount); // 0~1 ������ ����(Ȯ������ ����)
};
