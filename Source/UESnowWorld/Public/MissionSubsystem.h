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
    // �̼� ����(���̺� �� ���� �ּ� ����) - �ʿ� ������ ���� ����
    UFUNCTION(BlueprintCallable, Category = "Mission")
    void StartMission(FName MissionID, EMissionType Type, int32 Target);

    //  ������ �Ϸ� ����, ���� ���� �Լ�(�������Ʈ���� ȣ��)
    UFUNCTION(BlueprintCallable, Category = "Mission")
    void AddProgress(EMissionType Type, int32 Amount = 1);

    // ���� ��ȸ(�ɼ�)
    UFUNCTION(BlueprintPure, Category = "Mission")
    void GetProgress(int32& OutCurrent, int32& OutTarget) const
    {
        OutCurrent = CurrentCount; OutTarget = TargetCount;
    }

    // ����(�������Ʈ������ ���� �� �ְ�)
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