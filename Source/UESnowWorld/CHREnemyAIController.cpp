// Fill out your copyright notice in the Description page of Project Settings.


#include "CHREnemyAIController.h"
#include "CHREnemy.h"
#include "AI/EnemyTypes.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

ACHREnemyAIController::ACHREnemyAIController()
{
    bAttachToPawn = true;
}

static bool ProjectToNav(UWorld* World, const FVector& In, FVector& Out)
{
    if (UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World))
    {
        FNavLocation NL;
        if (Nav->ProjectPointToNavigation(In, NL, FVector(200, 200, 400)))
        {
            Out = NL.Location;
            return true;
        }
    }
    Out = In;
    return false;
}

void ACHREnemyAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ACHREnemyAIController::OnPossess"));
    if (!BehaviorTreeAsset)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("!BehaviorTreeAsset"));
        return;
    }
    // 1) 블랙보드 초기화
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, TEXT("00"));
    UBlackboardComponent* BBComp = nullptr;
    UseBlackboard(BehaviorTreeAsset->BlackboardAsset, BBComp);
    BB = BBComp;

    // 2) Pawn 캐스트
    ACHREnemy* Enemy = Cast<ACHREnemy>(InPawn);
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, Enemy->GetName());
    // 3) 기본 키 세팅 (Enemy가 있으면 그 값을 사용)
    if (BB)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, TEXT("11"));
        // PatrolMode/DoPatrol은 Pawn 설정을 따름(없으면 기본값)
        uint8 Mode = Enemy ? static_cast<uint8>(Enemy->PatrolMode) : 0;
        bool  bDo = Enemy ? Enemy->bDoPatrol : true;

        //BB->SetValueAsEnum(EnemyBB::PatrolMode, Mode);
        BB->SetValueAsInt(EnemyBB::PatrolMode, static_cast<int32>(Mode));
        BB->SetValueAsBool(EnemyBB::DoPatrol, bDo);

        // Home/Chase 관련 BB 초기값
        const FVector Home = Enemy ? Enemy->GetActorLocation() : (InPawn ? InPawn->GetActorLocation() : FVector::ZeroVector);
        BB->SetValueAsVector(EnemyBB::HomeLocation, Home);   // 집(스폰) 위치
        BB->SetValueAsBool(EnemyBB::AtHome, true);           // 시작은 집 안
        BB->SetValueAsFloat(EnemyBB::LastSeenTime, -100000.f);
        BB->SetValueAsBool(EnemyBB::ShouldChase, false);
        BB->SetValueAsBool(EnemyBB::NeedReturnHome, false);
        // ===========================================


        if (Enemy && Enemy->PatrolMode == EPatrolMode::LinePatrol)
        {
            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, TEXT("11-2"));
            Enemy->bGoingOut = true;

            FVector Target = Enemy->GetActorLocation()
                + Enemy->GetActorForwardVector().GetSafeNormal()
                * Enemy->LinePatrolDistance;

            ProjectToNav(GetWorld(), Target, Target);
            BB->SetValueAsVector(EnemyBB::PatrolTarget, Target);
        }
        else if (Enemy && Enemy->PatrolMode == EPatrolMode::Patrol)
        {
            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Magenta, TEXT("11-1"));
            BB->SetValueAsVector(EnemyBB::PatrolTarget, Enemy->GetActorLocation());
            BB->SetValueAsVector(EnemyBB::TargetPosition, Enemy->GetActorLocation());
        }
        else
        {
            BB->ClearValue(EnemyBB::PatrolTarget);
        }
    }

    // 4) 비헤이비어 트리 실행
    RunBehaviorTree(BehaviorTreeAsset);

    /*if (GEngine) 
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("RunBehaviorTree called"));*/

    //if (BehaviorTreeAsset)
    //{
    //    UBlackboardComponent* BBComp = nullptr;

    //    UseBlackboard(BehaviorTreeAsset->BlackboardAsset, BBComp);
    //    BB = BBComp;
    //   /* GEngine->AddOnScreenDebugMessage(
    //        -1,
    //        5.f,
    //        FColor::Purple,
    //        TEXT("ACHREnemyAIController::OnPossess()CPP")
    //    );*/

    //    RunBehaviorTree(BehaviorTreeAsset);
    //    //BB = GetBlackboardComponent();
    //}
}

