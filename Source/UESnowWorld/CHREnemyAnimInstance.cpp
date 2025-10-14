// Fill out your copyright notice in the Description page of Project Settings.


#include "CHREnemyAnimInstance.h"
#include "CHREnemy.h"
#include "AI/EnemyTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void UCHREnemyAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
    Owner = Cast<ACHREnemy>(TryGetPawnOwner());
    if (Owner) Move = Owner->GetCharacterMovement();
}

void UCHREnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    // ── 캐시 ─────────────────────────────────────────────
    if (!Owner)
    {
        Owner = Cast<ACHREnemy>(TryGetPawnOwner());
        if (!Owner) return;
    }
    if (!Move)
    {
        Move = Owner->GetCharacterMovement();
    }

    // ── 속도(수평) & 공중 여부 ───────────────────────────
    FVector V = Owner->GetVelocity();
    V.Z = 0.f;                 // 수평 성분만
    Speed = V.Size();        // cm/s
    bIsInAir = (Move ? Move->IsFalling() : false);

    // ── 추격 여부: BB의 TargetActor 유무로 판단 ─────────
    bIsChasing = false;
    if (AAIController* AIC = Cast<AAIController>(Owner->GetController()))
    {
        if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
        {
            bIsChasing = (BB->GetValueAsObject(EnemyBB::TargetActor) != nullptr);
        }
    }

    // ── 정규화 속도(0~1): 현재 MaxSpeed 기준 ────────────
    //    (추격 때는 RunSpeed, 순찰 때는 WalkSpeed가 MaxWalkSpeed로 잡혀있다는 가정)
    const float MaxRef = (Move && Move->GetMaxSpeed() > 1.f) ? Move->GetMaxSpeed() : 600.f;
    Speed01 = FMath::Clamp(Speed / MaxRef, 0.f, 1.f);

    // ── ABS 하나(0=Idle, 1=Walk, 2=Run)를 쓸 때 축값도 제공 ─
    //    Patrol(=미추격) → 0~1,  Chase(=추격) → 1~2 로 매핑
    SpeedABS = bIsChasing
        ? FMath::Lerp(1.f, 2.f, Speed01)    // Run 영역
        : FMath::Lerp(0.f, 1.f, Speed01);   // Walk 영역

    // ※ bIsAttacking 은 공격 몽타주 노티파이에서 On/Off 권장
    //Super::NativeUpdateAnimation(DeltaSeconds);

    //if (!Owner) { Owner = Cast<ACHREnemy>(TryGetPawnOwner()); if (!Owner) return; }
    //if (!Move) { Move = Owner->GetCharacterMovement(); }

    //FVector V = Owner->GetVelocity(); V.Z = 0.f;
    //Speed = V.Size();
    //bIsInAir = Move->IsFalling();

    //// 추격 여부: BB에 TargetActor 유무
    //if (AAIController* AIC = Cast<AAIController>(Owner->GetController()))
    //{
    //    if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
    //    {
    //        bIsChasing = BB->GetValueAsObject(EnemyBB::TargetActor) != nullptr;
    //    }
    //}
    //// bIsAttacking 은 공격 몽타주 노티파이로 On/Off 하는 걸 추천
}