// Fill out your copyright notice in the Description page of Project Settings.


#include "CHRBossAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"


ACHRBossAIController::ACHRBossAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	// 기본 넓은 시야
    SightConfig->SightRadius = 2500.f;
    SightConfig->LoseSightRadius = 3000.f;
    SightConfig->PeripheralVisionAngleDegrees = 85.f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

    PerceptionComp->ConfigureSense(*SightConfig);
    PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
    PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &ACHRBossAIController::OnTargetPerceptionUpdated);
}

void ACHRBossAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
    PickNewRoamDestination();
}

void ACHRBossAIController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (CurrentTarget)
        MoveToActor(CurrentTarget, /*AcceptanceRadius=*/150.f, true, true, true, 0, true);  // 타깃 추적
    else
        UpdateRoam(DeltaSeconds);
}

void ACHRBossAIController::ClearTarget()
{
    CurrentTarget = nullptr;
}

void ACHRBossAIController::ConfigureSight(float SightRadius, float LoseSightRadius, float PeripheralVisionAngleDegrees)
{
    SightConfig->SightRadius = SightRadius;
    SightConfig->LoseSightRadius = LoseSightRadius;
    SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngleDegrees;
    PerceptionComp->RequestStimuliListenerUpdate();
}

void ACHRBossAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (Stimulus.WasSuccessfullySensed())
        CurrentTarget = Actor;      // 플레이어를 빠르게 포착
    else
    {
        // 시야에서 사라짐 -> 타깃 클리어
        if (Actor == CurrentTarget)
        {
            CurrentTarget = nullptr;
            PickNewRoamDestination();
        }
    }
}

void ACHRBossAIController::UpdateRoam(float DeltaSeconds)
{
    RoamTimer -= DeltaSeconds;
    if (RoamTimer <= 0.0f)
    {
        PickNewRoamDestination();
        RoamTimer = RepathInterval;
    }
}

void ACHRBossAIController::PickNewRoamDestination()
{
    APawn* p = GetPawn();
    if (!p)
        return;

    if (UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(GetWorld()))
    {
        FNavLocation Out;
        const FVector Origin = p->GetActorLocation();
        if (Nav->GetRandomReachablePointInRadius(Origin, RoamRadius, Out))
        {
            CurrentRoamPoint = Out.Location;
            MoveToLocation(CurrentRoamPoint, /*AcceptanceRadius=*/80.f, true, true, true, false);
        }
    }
}
