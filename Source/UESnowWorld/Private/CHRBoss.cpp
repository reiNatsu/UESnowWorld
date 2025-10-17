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
		

	// ���� �þ߸� ���� Perception
    Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 4500.f;             // �а�
    SightConfig->LoseSightRadius = 5000.f;
    SightConfig->PeripheralVisionAngleDegrees = 130.f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    Perception->ConfigureSense(*SightConfig);
    Perception->SetDominantSense(SightConfig->GetSenseImplementation());

    // �ӵ� 1���� ����
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

    // ���� ����: �ʿ� �� ���⼭ ���� ����Ʈ�� �̵�
   // RespawnAtRandomPoint(); // ù ������ ���� �����ϰ� ������ Ȱ��ȭ
}

bool ACHRBoss::TryInstantDeathFromBackstab(AActor* Attacker, const FVector& HitFromDir)
{
    if (!Attacker || bIsDead)
        return false;

    const FVector BossForward = GetActorForwardVector();
    const float Dot = FVector::DotProduct(BossForward, -HitFromDir.GetSafeNormal());
    // ex, ���� �� �Ϳ��� ������ Dot�� ������ ����� ŭ
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

            // TODO : ���� ��Ÿ�� paly (BP���� mostage_play�� ó��)
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
    Super::HandleDeath();       // ���� ó�� ����

    // ���� ���� ó�� 
    bIsDead = true;
    FTimerHandle T;
    GetWorldTimerManager().SetTimer(T, this, &ACHRBoss::RespawnAtRandomPoint, 8.f, false);
}

void ACHRBoss::RespawnAtRandomPoint()
{
    // ���� ���� ����Ʈ(Tag = "BossSpawnPoint")�� �ϳ��� �ڷ���Ʈ
    TArray<AActor*> Points;
    UGameplayStatics::GetAllActorsWithTag()
}

void ACHRBoss::ReduceGeneratorProgress(AActor* GeneratorActor)
{
}

void ACHRBoss::EnterSpeedUpMode(float SpeedMul, float DamageMul)
{
}
