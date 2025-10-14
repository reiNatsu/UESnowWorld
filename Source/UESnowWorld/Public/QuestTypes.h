// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestTypes.generated.h"

UENUM(BlueprintType)
enum class EQuestType : uint8
{
	Linear		UMETA(DisplayName = "Linear"),
	CheckList	UMETA(DisplayName = "CheckList")
};

UENUM(BlueprintType)
enum class EObjectiveType : uint8
{
    TalkToNPC       UMETA(DisplayName = "TalkToNPC"),
    AcquireItem     UMETA(DisplayName = "AcquireItem"),
    KillEnemy       UMETA(DisplayName = "KillEnemy"),
    ReachLocation   UMETA(DisplayName = "ReachLocation"),
    InteractActor   UMETA(DisplayName = "InteractActor"),
    CustomEvent     UMETA(DisplayName = "CustomEvent")
};