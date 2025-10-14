// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "QuestData.h" 
#include "ObjectiveEventRouter.h"
#include "QuestSubsystem.generated.h"

/**
 * 퀘스트를 관리하는 전역 시스템
 현재 퀘스트 상태를 관리하고,
목표 달성 여부를 판정하고,
다음 퀘스트를 이어주는 시스템의 중심 두뇌역할


 */
UCLASS(BlueprintType)
class UESNOWWORLD_API UQuestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	UDataTable* QuestTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
	UDataTable* ObjectiveTable = nullptr;



public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 퀘스트 시작 함수 ( 튜토리얼 같은 것)
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void StartQuest(FName QuestId);

	// 퀘스트 진행 상태 출력 (디버그용)
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void DebugPrintQuest(FName QuestId);

	UFUNCTION(BlueprintPure, Category = "Quest", meta = (WorldContext = "WorldContextObject"))
	static UQuestSubsystem* Get(const UObject* WorldContextObject);

	// 라우터 이벤트를 받는 핸들러 (Dynamic에 필요)
	UFUNCTION() 
		void HandleObjectiveEvent(const FObjectiveEvent& E);
};
