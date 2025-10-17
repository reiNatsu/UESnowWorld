// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindPatrolPos.generated.h"

/** NavMesh 내에서 현재 위치 기준 랜덤 도달 가능 지점을 찾아 BB.TargetPosition에 기록 */
UCLASS()
class UESNOWWORLD_API UBTTask_FindPatrolPos : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
    UBTTask_FindPatrolPos();

    UPROPERTY(EditAnywhere, Category = "Search")
    float Radius = 600.f; // 순찰 반경

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
