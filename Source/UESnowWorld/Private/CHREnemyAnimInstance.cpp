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

    // ���� ĳ�� ������������������������������������������������������������������������������������������
    if (!Owner)
    {
        Owner = Cast<ACHREnemy>(TryGetPawnOwner());
        if (!Owner) return;
    }
    if (!Move)
    {
        Move = Owner->GetCharacterMovement();
    }

    // ���� �ӵ�(����) & ���� ���� ������������������������������������������������������
    FVector V = Owner->GetVelocity();
    V.Z = 0.f;                 // ���� ���и�
    Speed = V.Size();        // cm/s
    bIsInAir = (Move ? Move->IsFalling() : false);

    // ���� �߰� ����: BB�� TargetActor ������ �Ǵ� ������������������
    bIsChasing = false;
    if (AAIController* AIC = Cast<AAIController>(Owner->GetController()))
    {
        if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
        {
            bIsChasing = (BB->GetValueAsObject(EnemyBB::TargetActor) != nullptr);
        }
    }

    // ���� ����ȭ �ӵ�(0~1): ���� MaxSpeed ���� ������������������������
    //    (�߰� ���� RunSpeed, ���� ���� WalkSpeed�� MaxWalkSpeed�� �����ִٴ� ����)
    const float MaxRef = (Move && Move->GetMaxSpeed() > 1.f) ? Move->GetMaxSpeed() : 600.f;
    Speed01 = FMath::Clamp(Speed / MaxRef, 0.f, 1.f);

    // ���� ABS �ϳ�(0=Idle, 1=Walk, 2=Run)�� �� �� �ప�� ���� ��
    //    Patrol(=���߰�) �� 0~1,  Chase(=�߰�) �� 1~2 �� ����
    SpeedABS = bIsChasing
        ? FMath::Lerp(1.f, 2.f, Speed01)    // Run ����
        : FMath::Lerp(0.f, 1.f, Speed01);   // Walk ����

    // �� bIsAttacking �� ���� ��Ÿ�� ��Ƽ���̿��� On/Off ����
    //Super::NativeUpdateAnimation(DeltaSeconds);

    //if (!Owner) { Owner = Cast<ACHREnemy>(TryGetPawnOwner()); if (!Owner) return; }
    //if (!Move) { Move = Owner->GetCharacterMovement(); }

    //FVector V = Owner->GetVelocity(); V.Z = 0.f;
    //Speed = V.Size();
    //bIsInAir = Move->IsFalling();

    //// �߰� ����: BB�� TargetActor ����
    //if (AAIController* AIC = Cast<AAIController>(Owner->GetController()))
    //{
    //    if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
    //    {
    //        bIsChasing = BB->GetValueAsObject(EnemyBB::TargetActor) != nullptr;
    //    }
    //}
    //// bIsAttacking �� ���� ��Ÿ�� ��Ƽ���̷� On/Off �ϴ� �� ��õ
}