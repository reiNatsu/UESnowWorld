// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MissionRuntime.generated.h"

/**
 * USTRUCT�θ� �� ����ü
 * ������ ����(Ȯ�ο�)
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
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) FName CheckType;   // "OnMonsterDead" ���� �̺�Ʈ �±�
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) FString Param;     // �ʿ� �� ���Ϳ�
};