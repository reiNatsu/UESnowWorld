// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindPatrolPos.generated.h"

/** NavMesh ������ ���� ��ġ ���� ���� ���� ���� ������ ã�� BB.TargetPosition�� ��� */
UCLASS()
class UESNOWWORLD_API UBTTask_FindPatrolPos : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
    UBTTask_FindPatrolPos();

    UPROPERTY(EditAnywhere, Category = "Search")
    float Radius = 600.f; // ���� �ݰ�

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
