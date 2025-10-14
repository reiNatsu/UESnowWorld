// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAvatarAnimInstance.h"
#include "PlayerAvatar.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UPlayerAvatarAnimInstance::UPlayerAvatarAnimInstance()
{
}
void UPlayerAvatarAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
    if (OwnerCharacter)
    {
        MoveComp = OwnerCharacter->GetCharacterMovement();
    }
}
void UPlayerAvatarAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (!OwnerCharacter)
    {
        OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
    }

    if (!OwnerCharacter) return;

    const FVector Velocity = OwnerCharacter->GetVelocity();
    const FVector Lateral = FVector(Velocity.X, Velocity.Y, 0.f);
    Speed = Lateral.Size();

    // AnimInstance�� �����ϴ� ���� ���(����: Pawn�� ȸ��)
    Direction = CalculateDirection(Velocity, OwnerCharacter->GetActorRotation());

    if (!MoveComp)
    {
        MoveComp = OwnerCharacter->GetCharacterMovement();
    }

    bIsInAir = (MoveComp ? MoveComp->IsFalling() : false);
   /* if (MoveComp)
    {
        bIsInAir = MoveComp->IsFalling();
        bHasAcceleration = !MoveComp->GetCurrentAcceleration().IsNearlyZero();
    }*/
}
void UPlayerAvatarAnimInstance::OnStateAnimationEnds()
{
    switch (State)
    {
    case EPlayerState::Attack:
    case EPlayerState::Hit:
    case EPlayerState::Vault:
    case EPlayerState::Climb:
    case EPlayerState::Jump:
        State = EPlayerState::Locomotion;
        break;
    case EPlayerState::Die:
        // ���� ���� ���� (���Ѵٸ� ���⼭�� ����)
        break;
    default:
        break;
    }
}
