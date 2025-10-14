// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameModeBase.h"
#include "UESnowWorldGameMode.generated.h"

/*
#include "MissionManager.h"
#include "MissionData.h"
#include "EngineUtils.h"
#include "Engine/DataTable.h"
#include "Algo/RandomShuffle.h"
*/

class AMissionManager; // �� ���� ����(��� include�� cpp����)


UCLASS(minimalapi)
class AUESnowWorldGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AUESnowWorldGameMode();

	// ���� ���� �� �Ŵ��� ã��/���� + �ʱ�ȭ
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mission")
	AMissionManager* MissionManager = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Mission")
	AMissionManager* GetMissionManager() const { return MissionManager; }

	// (����) �ڵ� ���� �� �� Ŭ���� ���� �����ϰ� �ϰ� ������ �ּ� ����
   // UPROPERTY(EditDefaultsOnly, Category="Mission")
   // TSubclassOf<AMissionManager> MissionManagerClass;

};



