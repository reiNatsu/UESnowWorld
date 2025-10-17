// Fill out your copyright notice in the Description page of Project Settings.


#include "CHRPlayerAimInstance.h"
#include "CHRPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"



void UCHRPlayerAimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// 이 AnimInstance가 붙어있는 Pawn을 가져와서 플레이어로 캐스팅
	OwnerCharacter = Cast<ACHRPlayer>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		CharacterMovement = OwnerCharacter->GetCharacterMovement();
	}
}

void UCHRPlayerAimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 소유자/무브먼트 캐시 보정
	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<ACHRPlayer>(TryGetPawnOwner());
		if (!OwnerCharacter) return;
	}
	if (!CharacterMovement)
	{
		CharacterMovement = OwnerCharacter->GetCharacterMovement();
	}

	// ===== 속도(수평) 계산 =====
	FVector Vel = OwnerCharacter->GetVelocity();
	VerticalSpeed = Vel.Z;   
	Vel.Z = 0.f;            // 수평만 고려(달리기/걷기 속도 판별 위함)
	Speed = Vel.Size();     // cm/s 기준

	// ===== 입력/이동 방향 성분 계산 =====
	// 컨트롤러 Yaw 기준 전/우 벡터를 구해서,
	// 현재 이동 방향(속도 정규화)과 내적하여 -1~1 범위를 얻는다.
	ForwardInput = 0.f;
	SideInput = 0.f;

	if (OwnerCharacter->Controller && !Vel.IsNearlyZero())
	{
		// 컨트롤러 Yaw 기준 축
		const FRotator YawRot(0.f, OwnerCharacter->Controller->GetControlRotation().Yaw, 0.f);
		const FVector ForwardDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
		const FVector RightDir = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

		// 정규화된 이동 방향(-1~+1)
		const FVector MoveDir = Vel.GetSafeNormal();

		// 기본 내적값(-1~+1)
		float Fwd = FVector::DotProduct(ForwardDir, MoveDir);
		float Side = FVector::DotProduct(RightDir, MoveDir);

		// 현재 속도 / 현재 MaxSpeed = 0~1
		// (걷기/달리기 상태에 따라 MaxSpeed가 바뀌므로 여기선 "현재 상태 대비" 비율)
		const float SpeedRatio = CharacterMovement->GetMaxSpeed() > 1.f
			? (Speed / CharacterMovement->GetMaxSpeed())
			: 0.f;

		// 달리기면 2배(BlendSpace 범위 -2~+2에 맞춤)
		const float RunMul = OwnerCharacter->IsRunning() ? 2.0f : 1.0f;

		// 최종 값: 방향(-1~+1) * 속도비(0~1) * 달리기배수(1 or 2)
		Fwd = Fwd * SpeedRatio * RunMul;
		Side = Side * SpeedRatio * RunMul;

		// BlendSpace 축 범위에 맞게 클램프(-2~+2)
		ForwardInput = FMath::Clamp(Fwd, -2.0f, 2.0f);
		SideInput = FMath::Clamp(Side, -2.0f, 2.0f);
	}

	// --- 공중 상태 & 엣지 감지 ---
	const bool bNowInAir = CharacterMovement->IsFalling();
	bJustBecameInAir = (!bPrevIsInAir && bNowInAir);  // 이번 프레임에 지면->공중
	bJustLanded = (bPrevIsInAir && !bNowInAir);  // 이번 프레임에 공중->지면

	bIsInAir = bNowInAir;
	bIsRunning = OwnerCharacter->IsRunning();

	// 다음 프레임 비교를 위한 상태 저장
	bPrevIsInAir = bNowInAir;
}
