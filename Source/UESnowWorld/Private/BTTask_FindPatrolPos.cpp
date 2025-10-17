// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindPatrolPos.h"
#include "AI/EnemyTypes.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
    NodeName = TEXT("Find Random Patrol Position");
}

EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    if (!AIC) return EBTNodeResult::Failed;

    APawn* Pawn = AIC->GetPawn();
    if (!Pawn) return EBTNodeResult::Failed;

    const UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Pawn);
    if (!NavSys) return EBTNodeResult::Failed;

    FNavLocation OutLoc;
    const bool bFound = NavSys->GetRandomReachablePointInRadius(Pawn->GetActorLocation(), Radius, OutLoc);
    if (!bFound) return EBTNodeResult::Failed;

    OwnerComp.GetBlackboardComponent()->SetValueAsVector(EnemyBB::TargetPosition, OutLoc.Location);
    return EBTNodeResult::Succeeded;
}
