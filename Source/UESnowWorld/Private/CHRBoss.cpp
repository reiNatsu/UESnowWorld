// Fill out your copyright notice in the Description page of Project Settings.


#include "CHRBoss.h"

//#include "BossAbilityComponent.h"
//#include "BossPhaseComponent.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"

ACHRBoss::ACHRBoss()
{
	PrimaryActorTick.bCanEverTick = true;

	AbilityComp = CreateDefaultSubobject<UBossAbilityComponent>(TEXT("AbilityComp"));
	PhaseComp = CreateDefaultSubobject<UBossPhaseComponent>(TEXT("PhaseComp"));
		

	// 넓은 시야를 위한 Perception
    Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 4500.f;             // 넓게
    SightConfig->LoseSightRadius = 5000.f;
    SightConfig->PeripheralVisionAngleDegrees = 130.f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    Perception->ConfigureSense(*SightConfig);
    Perception->SetDominantSense(SightConfig->GetSenseImplementation());

    // 속도 1종류 세팅
    if (UCharacterMovementComponent* Move = GetCharacterMovement())
    {
        Move->MaxWalkSpeed = MoveSpeed;
        Move->bUseControllerDesiredRotation = false;
        Move->bOrientRotationToMovement = true;
    }
}

void ACHRBoss::BeginPlay()
{

    Super::BeginPlay();

    // 스폰 시점: 필요 시 여기서 랜덤 포인트로 이동
   // RespawnAtRandomPoint(); // 첫 스폰도 강제 랜덤하고 싶으면 활성화
}

bool ACHRBoss::TryInstantDeathFromBackstab(AActor* Attacker, const FVector& HitFromDir)
{
    if (!Attacker || bIsDead)
        return false;

    const FVector BossForward = GetActorForwardVector();
    const float Dot = FVector::DotProduct(BossForward, -HitFromDir.GetSafeNormal());
    // ex, 보스 등 귀에서 맞으면 Dot가 음수로 충분히 큼
    if (Dot <= -BackstabDotThreshold)
    {
        HandleDeath();
        return true;
    }

	return false;
}

void ACHRBoss::OnHeadBlocked(const FHitResult& Hit)
{
    if (bIsDead || bIsStunned)
        return;

    if (AActor* Other = Hit.GetActor())
    {
        if (Other->ActorHasTag(TEXT("NoDestroy")))
        {
            bIsStunned = true;
            if (UCharacterMovementComponent* Move = GetCharacterMovement())
            {
                Move->StopMovementImmediately();
            }

            // TODO : 스턴 몽타주 paly (BP에서 mostage_play로 처리)
            GetWorldTimerManager().SetTimerForNextTick([this]()
                {
                    GetWorldTimerManager().SetTimer(FTimerHandle(), this, &ACHRBoss::EndStun, StunDuration, false);
                });
        }
    }
}

void ACHRBoss::EndStun()
{
    bIsStunned = false;
}


void ACHRBoss::HandleDeath()
{
    Super::HandleDeath();       // 공통 처리 먼저

    // 보스 전용 처리 
    bIsDead = true;
    FTimerHandle T;
    GetWorldTimerManager().SetTimer(T, this, &ACHRBoss::RespawnAtRandomPoint, 8.f, false);
}

void ACHRBoss::RespawnAtRandomPoint()
{
    // 랜덤 스폰 포인트(Tag = "BossSpawnPoint")중 하나로 텔레포트
    TArray<AActor*> Points;
    UGameplayStatics::GetAllActorsWithTag()
}

void ACHRBoss::ReduceGeneratorProgress(AActor* GeneratorActor)
{
}

void ACHRBoss::EnterSpeedUpMode(float SpeedMul, float DamageMul)
{
}
