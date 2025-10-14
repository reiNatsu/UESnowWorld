// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HealthProvider.generated.h"


// UINTERFACE: �������Ʈ���� ���� �� �ְ� Blueprintable ����
UINTERFACE(Blueprintable)
class UHealthProvider : public UInterface
{
	GENERATED_BODY()

};

// ���� �������̽�(����Ƽ��)
class UESNOWWORLD_API IHealthProvider
{
    GENERATED_BODY()

public:
    // 0.0 ~ 1.0 (HP ����) ��ȯ
    // BlueprintNativeEvent: BP/CPP �� �� ���� ����
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Health")
    float GetHpRatio() const;
};