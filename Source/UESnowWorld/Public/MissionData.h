// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MissionData.generated.h"


UENUM(BlueprintType)
enum class EMissionType : uint8
{
    None    UMETA(DisplayName = "None"),
    KillMonster       UMETA(DisplayName = "몬스터 킬 카운트"),
    DestroyObject UMETA(DisplayName = "오브젝트 파괴 카운트"),
    EndHP UMETA(DisplayName = "종료시 플레이어 HP 체크"),
    ActivateGenerator   UMETA(DisplayName = "발전기 고치기")
    
};

USTRUCT(BlueprintType)
struct FMissionData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MissionID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EMissionType MissionType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 TargetCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName CheckType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Param;
};

//class UESNOWWORLD_API MissionData
//{
//public:
//	MissionData();
//	~MissionData();
//};
