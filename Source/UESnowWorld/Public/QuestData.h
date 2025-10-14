// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "QuestTypes.h"
#include "QuestData.generated.h"


USTRUCT(BlueprintType)
struct FQuestRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName QuestId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EQuestType Type = EQuestType::Linear;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Title;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 RoundId = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bAutoStart = false;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName NextQuestId;
};

USTRUCT(BlueprintType)
struct FObjectiveRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName QuestId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 StepIndex = 0;   // Linear ´Ü°è

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 GroupId = 0;     // Checklist ¹­À½

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName ObjectiveId;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EObjectiveType Type = EObjectiveType::TalkToNPC;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName TargetKey;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 TargetCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bOptional = false;
};