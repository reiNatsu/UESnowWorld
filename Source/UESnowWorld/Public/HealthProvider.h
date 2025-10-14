// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HealthProvider.generated.h"


// UINTERFACE: 블루프린트에서 붙일 수 있게 Blueprintable 권장
UINTERFACE(Blueprintable)
class UHealthProvider : public UInterface
{
	GENERATED_BODY()

};

// 실제 인터페이스(네이티브)
class UESNOWWORLD_API IHealthProvider
{
    GENERATED_BODY()

public:
    // 0.0 ~ 1.0 (HP 비율) 반환
    // BlueprintNativeEvent: BP/CPP 둘 다 구현 가능
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Health")
    float GetHpRatio() const;
};