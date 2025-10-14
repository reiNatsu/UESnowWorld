// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionSubsystem.h"

void UMissionSubsystem::StartMission(FName MissionID, EMissionType Type, int32 Target)
{
    ActiveMissionID = MissionID;
    CheckType = Type;
    TargetCount = Target;
    CurrentCount = 0;
    bCompleted = false;

    UE_LOG(LogTemp, Log, TEXT("[Mission] Start %s Type=%d Target=%d"),
        *MissionID.ToString(), (int32)Type, Target);
}

void UMissionSubsystem::AddProgress(EMissionType Type, int32 Amount)
{
    if (bCompleted) return;
    if (Type != CheckType) return;

    CurrentCount = FMath::Clamp(CurrentCount + Amount, 0, TargetCount);
    UE_LOG(LogTemp, Log, TEXT("[Mission] Progress %d/%d"), CurrentCount, TargetCount);

    if (CurrentCount >= TargetCount)
    {
        bCompleted = true;
        UE_LOG(LogTemp, Log, TEXT("[Mission] Completed!"));
        // 여기서 StageClear 브로드캐스트/호출 하고 싶으면 추가
    }
}

