// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "QuestData.h" 
#include "ObjectiveEventRouter.h"
#include "QuestSubsystem.generated.h"

/**
 * ����Ʈ�� �����ϴ� ���� �ý���
 ���� ����Ʈ ���¸� �����ϰ�,
��ǥ �޼� ���θ� �����ϰ�,
���� ����Ʈ�� �̾��ִ� �ý����� �߽� �γ�����


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

	// ����Ʈ ���� �Լ� ( Ʃ�丮�� ���� ��)
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void StartQuest(FName QuestId);

	// ����Ʈ ���� ���� ��� (����׿�)
	UFUNCTION(BlueprintCallable, Category = "Quest")
	void DebugPrintQuest(FName QuestId);

	UFUNCTION(BlueprintPure, Category = "Quest", meta = (WorldContext = "WorldContextObject"))
	static UQuestSubsystem* Get(const UObject* WorldContextObject);

	// ����� �̺�Ʈ�� �޴� �ڵ鷯 (Dynamic�� �ʿ�)
	UFUNCTION() 
		void HandleObjectiveEvent(const FObjectiveEvent& E);
};
