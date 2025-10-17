// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"

#include "AI/EnemyTypes.h"
#include "CHREnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTTask_Attack::UBTTask_Attack()
{
    NodeName = TEXT("Attack Target");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return EBTNodeResult::Failed;

    AActor* Target = Cast<AActor>(BB->GetValueAsObject(EnemyBB::TargetActor));
    const bool bInRange = BB->GetValueAsBool(EnemyBB::InAttackRange);
    if (!Target || !bInRange) return EBTNodeResult::Failed;

    AAIController* AIC = OwnerComp.GetAIOwner();
    ACHREnemy* Enemy = AIC ? Cast<ACHREnemy>(AIC->GetPawn()) : nullptr;
    if (!Enemy) return EBTNodeResult::Failed;

    return Enemy->TryAttackTarget(Target) ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}