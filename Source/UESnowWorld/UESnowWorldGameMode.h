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

class AMissionManager; // ← 전방 선언(헤더 include는 cpp에서)


UCLASS(minimalapi)
class AUESnowWorldGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AUESnowWorldGameMode();

	// 레벨 시작 시 매니저 찾기/스폰 + 초기화
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mission")
	AMissionManager* MissionManager = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Mission")
	AMissionManager* GetMissionManager() const { return MissionManager; }

	// (선택) 자동 스폰 시 쓸 클래스 지정 가능하게 하고 싶으면 주석 해제
   // UPROPERTY(EditDefaultsOnly, Category="Mission")
   // TSubclassOf<AMissionManager> MissionManagerClass;

};



