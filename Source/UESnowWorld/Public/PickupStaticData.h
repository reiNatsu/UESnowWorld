// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "PickupStaticData.generated.h"

USTRUCT(BlueprintType)
struct FPickupDtaticData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString _name;		// 이름

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> _pockup;		// 어떤 액터일지
};

