// Fill out your copyright notice in the Description page of Project Settings.


#include "CHRBossAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CHRBoss.h"


UCHRBossAnimInstance::UCHRBossAnimInstance() {}

void UCHRBossAnimInstance::NativeInitializeAnimation()
{
    APawn* OwnerPawn = TryGetPawnOwner();
    if (OwnerPawn)
    {
        Boss = Cast<ACHRBoss>(OwnerPawn);
    }
}

void UCHRBossAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    if (!Boss.IsValid())
    {
        APawn* OwnerPawn = TryGetPawnOwner();
        Boss = Cast<ACHRBoss>(OwnerPawn);
        if (!Boss.IsValid()) return;
    }

    // 이동 파라미터
    const ACharacter* Char = Cast<ACharacter>(Boss.Get());
    if (Char && Char->GetCharacterMovement())
    {
        const FVector V = Char->GetVelocity();
        Speed = FVector(V.X, V.Y, 0.f).Size();
        bIsInAir = Char->GetCharacterMovement()->IsFalling();
    }

    // 상태 파라미터
    bIsDead = Boss->IsDead();                // BaseCharacter에 이미 있음
    bIsStunned = Boss->IsStunned();             // CHRBoss에 Getter 하나 노출해줘 (bool bStunned)
    bHasTarget = Boss->HasValidTarget();        // CHRBoss에 Getter 하나 노출해줘 (AI 타깃 유/무)
}

void UCHRBossAnimInstance::PlayStunMontage()
{
    if (StunMontage)
    {
        Montage_Play(StunMontage, 1.f);
    }
}

float UCHRBossAnimInstance::PlayMontage(UAnimMontage* Montage, float InPlayRate)
{
    return (Montage) ? Montage_Play(Montage, InPlayRate) : 0.f;
}