// Fill out your copyright notice in the Description page of Project Settings.


#include "CHRPlayerAimInstance.h"
#include "CHRPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"



void UCHRPlayerAimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// �� AnimInstance�� �پ��ִ� Pawn�� �����ͼ� �÷��̾�� ĳ����
	OwnerCharacter = Cast<ACHRPlayer>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		CharacterMovement = OwnerCharacter->GetCharacterMovement();
	}
}

void UCHRPlayerAimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// ������/�����Ʈ ĳ�� ����
	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<ACHRPlayer>(TryGetPawnOwner());
		if (!OwnerCharacter) return;
	}
	if (!CharacterMovement)
	{
		CharacterMovement = OwnerCharacter->GetCharacterMovement();
	}

	// ===== �ӵ�(����) ��� =====
	FVector Vel = OwnerCharacter->GetVelocity();
	VerticalSpeed = Vel.Z;   
	Vel.Z = 0.f;            // ���� ���(�޸���/�ȱ� �ӵ� �Ǻ� ����)
	Speed = Vel.Size();     // cm/s ����

	// ===== �Է�/�̵� ���� ���� ��� =====
	// ��Ʈ�ѷ� Yaw ���� ��/�� ���͸� ���ؼ�,
	// ���� �̵� ����(�ӵ� ����ȭ)�� �����Ͽ� -1~1 ������ ��´�.
	ForwardInput = 0.f;
	SideInput = 0.f;

	if (OwnerCharacter->Controller && !Vel.IsNearlyZero())
	{
		// ��Ʈ�ѷ� Yaw ���� ��
		const FRotator YawRot(0.f, OwnerCharacter->Controller->GetControlRotation().Yaw, 0.f);
		const FVector ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
		const FVector RightDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

		// ����ȭ�� �̵� ����(-1~+1)
		const FVector MoveDir = Vel.GetSafeNormal();

		// �⺻ ������(-1~+1)
		float Fwd = FVector::DotProduct(ForwardDir, MoveDir);
		float Side = FVector::DotProduct(RightDir, MoveDir);

		// ���� �ӵ� / ���� MaxSpeed = 0~1
		// (�ȱ�/�޸��� ���¿� ���� MaxSpeed�� �ٲ�Ƿ� ���⼱ "���� ���� ���" ����)
		const float SpeedRatio = CharacterMovement->GetMaxSpeed() > 1.f
			? (Speed / CharacterMovement->GetMaxSpeed())
			: 0.f;

		// �޸���� 2��(BlendSpace ���� -2~+2�� ����)
		const float RunMul = OwnerCharacter->IsRunning() ? 2.0f : 1.0f;

		// ���� ��: ����(-1~+1) * �ӵ���(0~1) * �޸�����(1 or 2)
		Fwd = Fwd * SpeedRatio * RunMul;
		Side = Side * SpeedRatio * RunMul;

		// BlendSpace �� ������ �°� Ŭ����(-2~+2)
		ForwardInput = FMath::Clamp(Fwd, -2.0f, 2.0f);
		SideInput = FMath::Clamp(Side, -2.0f, 2.0f);
	}

	// --- ���� ���� & ���� ���� ---
	const bool bNowInAir = CharacterMovement->IsFalling();
	bJustBecameInAir = (!bPrevIsInAir && bNowInAir);  // �̹� �����ӿ� ����->����
	bJustLanded = (bPrevIsInAir && !bNowInAir);  // �̹� �����ӿ� ����->����

	bIsInAir = bNowInAir;
	bIsRunning = OwnerCharacter->IsRunning();

	// ���� ������ �񱳸� ���� ���� ����
	bPrevIsInAir = bNowInAir;
}
