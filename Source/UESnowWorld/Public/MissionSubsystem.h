// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MissionData.h"

#include "MissionSubsystem.generated.h"

UCLASS()
class UESNOWWORLD_API UMissionSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // 미션 시작(테이블 안 쓰는 최소 버전) - 필요 없으면 생략 가능
    UFUNCTION(BlueprintCallable, Category = "Mission")
    void StartMission(FName MissionID, EMissionType Type, int32 Target);

    //  발전기 완료 포함, 진행 보고 함수(블루프린트에서 호출)
    UFUNCTION(BlueprintCallable, Category = "Mission")
    void AddProgress(EMissionType Type, int32 Amount = 1);

    // 진행 조회(옵션)
    UFUNCTION(BlueprintPure, Category = "Mission")
    void GetProgress(int32& OutCurrent, int32& OutTarget) const
    {
        OutCurrent = CurrentCount; OutTarget = TargetCount;
    }

    // 상태(블루프린트에서도 읽을 수 있게)
    UPROPERTY(BlueprintReadOnly, Category = "Mission")
    FName ActiveMissionID;

    UPROPERTY(BlueprintReadOnly)
    EMissionType CheckType = EMissionType::None;

    UPROPERTY(BlueprintReadOnly, Category = "Mission")
    int32 TargetCount = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Mission")
    int32 CurrentCount = 0;

    UPROPERTY(BlueprintReadOnly, Category = "Mission")
    bool bCompleted = false;

};