// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MissionRuntime.generated.h"

/**
 * USTRUCT로만 쓸 구조체
 * 에디터 노출(확인용)
 */

USTRUCT(BlueprintType)
struct FMissionRuntime
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) int32 MissionID = 0;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) int32 TargetCount = 0;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) int32 CurrentCount = 0;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) bool  bCompleted = false;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) FString Description;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) FName CheckType;   // "OnMonsterDead" 같은 이벤트 태그
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) FString Param;     // 필요 시 필터용
};