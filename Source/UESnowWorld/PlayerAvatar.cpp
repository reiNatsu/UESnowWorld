// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAvatar.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "PlayerAvatarAnimInstance.h" // AnimInstance에 상태 전달용

// Sets default values
APlayerAvatar::APlayerAvatar()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 캐릭터는 입력 방향으로 회전 (컨트롤러 yaw는 카메라 회전에만 사용)
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// 이동 컴포넌트 세팅
	auto* MoveComp = GetCharacterMovement();
	MoveComp->bOrientRotationToMovement = true;
	MoveComp->RotationRate = FRotator(0.f, 540.f, 0.f);
	MoveComp->MaxWalkSpeed = WalkSpeed;

	// 스프링암
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = BaseArmLength;
	SpringArm->bUsePawnControlRotation = true;             // 컨트롤러 회전을 따름
	SpringArm->SetRelativeRotation(FRotator(PitchAngle, 0.f, 0.f));
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 12.f;

	// 카메라
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;         // 카메라는 스프링암만 따름
}

// Called when the game starts or when spawned
void APlayerAvatar::BeginPlay()
{
	Super::BeginPlay();

	// 안전차원: 시작 시 속도 보정
	if (auto* MoveComp = GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = WalkSpeed;
	}
}

// Called every frame
void APlayerAvatar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerAvatar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	// ── Axis 바인딩 ──
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerAvatar::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &APlayerAvatar::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APlayerAvatar::Turn);   // Mouse X
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APlayerAvatar::LookUp); // Mouse Y

	// 점프를 쓰고 싶다면:
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ACharacter::StopJumping);

	// Action 바인딩(프로젝트 세팅 > Input에 "Attack" 등록 필요)
	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &APlayerAvatar::Attack);
}

void APlayerAvatar::MoveForward(float Value)
{
	if (Controller && FMath::Abs(Value) > KINDA_SMALL_NUMBER)
	{
		const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
		AddMovementInput(Forward, Value);
	}
}

void APlayerAvatar::MoveRight(float Value)
{
	if (Controller && FMath::Abs(Value) > KINDA_SMALL_NUMBER)
	{
		const FRotator YawRot(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
		AddMovementInput(Right, Value);
	}
}

void APlayerAvatar::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void APlayerAvatar::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void APlayerAvatar::Attack()
{
	if (bIsAttacking || AttackMontage == nullptr)
	{
		return; // 중복 공격 방지 or 미할당
	}

	// 현재 애님 인스턴스
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		if (UAnimInstance* AnimInst = MeshComp->GetAnimInstance())
		{
			// 상태 전환: Attack
			if (UPlayerAvatarAnimInstance* Typed = Cast<UPlayerAvatarAnimInstance>(AnimInst))
			{
				Typed->SetState(EPlayerState::Attack);
			}

			// 몽타주 재생
			const float PlayResult = AnimInst->Montage_Play(AttackMontage, 1.0f);
			if (PlayResult > 0.f)
			{
				bIsAttacking = true;

				// 즉시 멈춤 (관성 제거)
				if (auto* MoveComp = GetCharacterMovement())
				{
					MoveComp->StopMovementImmediately();
					// MoveComp->BrakingFrictionFactor = 0.f; // 필요시 관성 더 줄이기 (선택)
				}

				//전역 이동 입력 차단(선택) 카메라 회전은 유지
				if (bBlockMoveWhileAttacking)
				{
					if (APlayerController* PC = Cast<APlayerController>(GetController()))
					{
						PC->SetIgnoreMoveInput(true);
					}
				}

				// 몽타주 종료 감지
				AnimInst->OnMontageEnded.AddDynamic(this, &APlayerAvatar::HandleMontageEnded);
			}
		}
	}
}

// ───────── 몽타주 종료: Attack + Parkour 공통 복구 ─────────
void APlayerAvatar::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 우리가 재생한 공격 몽타주가 끝났는지 확인
	if (Montage == AttackMontage)
	{
		bIsAttacking = false;

		// 이동 입력 복원(선택)
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->SetIgnoreMoveInput(false);
		}

		if (auto* Move = GetCharacterMovement())
		{
			if (Move->MovementMode == MOVE_None)
			{
				Move->SetMovementMode(MOVE_Walking);
			}
		}
		if (USkeletalMeshComponent* MeshComp = GetMesh())
		{
			if (UPlayerAvatarAnimInstance* Typed = Cast<UPlayerAvatarAnimInstance>(MeshComp->GetAnimInstance()))
			{
				Typed->SetState(EPlayerState::Locomotion);
			}
		}
		return;
	}
	if (Montage == VaultMontage || Montage == ClimbMontage)
	{
		bIsInParkour = false;

		if (auto* Move = GetCharacterMovement())
		{
			if (bZeroGravityWhileClimb) Move->GravityScale = SavedGravityScale;
			if (Move->MovementMode == MOVE_None) Move->SetMovementMode(MOVE_Walking);
		}
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			PC->SetIgnoreMoveInput(false);
		}

		// 루트모션이 짧아 절벽 위까지 못 갔을 때를 대비해 스냅 보정
		if (bHasPendingStandLocation)
		{
			const float Dist = FVector::Dist2D(GetActorLocation(), PendingStandLocation);
			// 10~30cm 이상 차이나면 보정(값은 취향/프로젝트별 조정)
			if (Dist > 20.f)
			{
				// 충돌 무시 텔레포트(걸림 방지). 충돌 고려하려면 Sweep 이동 로직으로 대체.
				SetActorLocation(PendingStandLocation, false, nullptr, ETeleportType::TeleportPhysics);
			}
			bHasPendingStandLocation = false;
		}

		// AnimInstance 상태 복귀
		if (USkeletalMeshComponent* MeshComp = GetMesh())
		{
			if (UPlayerAvatarAnimInstance* Typed = Cast<UPlayerAvatarAnimInstance>(MeshComp->GetAnimInstance()))
			{
				Typed->SetState(EPlayerState::Locomotion);
			}
		}
		return;
	}
}

void APlayerAvatar::OnJumpPressed()
{
	if (bIsAttacking || bIsInParkour)
		return;

	// 파쿠르 우선 시도
	if (TryVault() || TryClimb())
	{
		return;
	}

	// 실패 시 일반 점프
	Jump();
}

bool APlayerAvatar::TryVault()
{
	// 무릎 높이에서는 맞고, 머리 높이에서는 안 맞으면 → 낮은 장애물 = Vault 후보 (확실함)
	FHitResult KneeHit, HeadHit;
	const bool bKneeHit = TraceForwardAtHeight(KneeHeight, ParkourTraceDistance, KneeHit);
	const bool bHeadHit = TraceForwardAtHeight(HeadHeight, ParkourTraceDistance, HeadHit);

	if (!bKneeHit || bHeadHit)
		return false;

	// 장애물 높이 근사: 머리-무릎 차이로 판단(간단 판정)
	const float ObstacleHeight = HeadHeight - KneeHeight; // 단순 근사(프로젝트에 맞게 조정 가능)
	if (ObstacleHeight < MinVaultHeight || ObstacleHeight > MaxVaultHeight)
		return false;

	FVector LandLoc;
	if (!FindVaultLandingLocation(KneeHit, LandLoc))
		return false;

	StartVault(LandLoc);
	return true;
}


void APlayerAvatar::StartVault(const FVector& LandLocation)
{
	if (!VaultMontage) return;

	if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
	{
		// 상태/입력/속도 정지
		if (UPlayerAvatarAnimInstance* Typed = Cast<UPlayerAvatarAnimInstance>(AnimInst))
		{
			Typed->SetState(EPlayerState::Vault);
		}

		bIsInParkour = true;
		if (auto* Move = GetCharacterMovement())
		{
			Move->StopMovementImmediately();
		}
		if (bBlockMoveWhileParkour)
		{
			if (APlayerController* PC = Cast<APlayerController>(GetController()))
			{
				PC->SetIgnoreMoveInput(true);
			}
		}

		// 바라보는 방향을 착지 방향으로 미세 정렬(선택)
		const FVector Dir = (LandLocation - GetActorLocation());
		if (!Dir.IsNearlyZero())
		{
			const FRotator YawOnly(0.f, Dir.Rotation().Yaw, 0.f);
			SetActorRotation(YawOnly);
		}

		AnimInst->OnMontageEnded.RemoveDynamic(this, &APlayerAvatar::HandleMontageEnded);
		const float Play = AnimInst->Montage_Play(VaultMontage, 1.f);
		if (Play > 0.f)
		{
			AnimInst->OnMontageEnded.AddDynamic(this, &APlayerAvatar::HandleMontageEnded);
		}
		else
		{
			// 실패 시 복구
			bIsInParkour = false;
			if (APlayerController* PC = Cast<APlayerController>(GetController()))
			{
				PC->SetIgnoreMoveInput(false);
			}
		}
	}
}


bool APlayerAvatar::TryClimb()
{
	FHitResult KneeHit, HeadHit;
	const bool bKneeHit = TraceForwardAtHeight(KneeHeight, ParkourTraceDistance, KneeHit);
	const bool bHeadHit = TraceForwardAtHeight(HeadHeight, ParkourTraceDistance, HeadHit);
	if (!(bKneeHit && bHeadHit)) return false;

	FVector LedgeLoc, LedgeNormal;
	if (!FindClimbLedgeLocation(KneeHit, LedgeLoc, LedgeNormal)) return false;

	const float Height = (LedgeLoc.Z - GetActorLocation().Z);
	if (Height < MinClimbHeight || Height > MaxClimbHeight) return false;

	// 벽 위(플랫폼) 스탠드 지점도 찾아 둠
	FVector TopStandLoc;
	bHasPendingStandLocation = FindClimbTopStandLocation(KneeHit, TopStandLoc);
	if (bHasPendingStandLocation) PendingStandLocation = TopStandLoc;

	StartClimb(LedgeLoc, LedgeNormal);
	return true;
	//// 무릎/머리 높이 모두 맞으면 → 높은 벽
	//FHitResult KneeHit, HeadHit;
	//const bool bKneeHit = TraceForwardAtHeight(KneeHeight, ParkourTraceDistance, KneeHit);
	//const bool bHeadHit = TraceForwardAtHeight(HeadHeight, ParkourTraceDistance, HeadHit);

	//if (!(bKneeHit && bHeadHit))
	//	return false;

	//FVector LedgeLoc, LedgeNormal;
	//if (!FindClimbLedgeLocation(KneeHit, LedgeLoc, LedgeNormal))
	//	return false;

	//// ledge 높이 대략 판정(간단 근사)
	//const float Height = (LedgeLoc.Z - GetActorLocation().Z);
	//if (Height < MinClimbHeight || Height > MaxClimbHeight)
	//	return false;

	//StartClimb(LedgeLoc, LedgeNormal);
	//return true;
}

void APlayerAvatar::StartClimb(const FVector& LedgeLocation, const FVector& LedgeNormal)
{
	if (!ClimbMontage) return;

	if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
	{
		if (UPlayerAvatarAnimInstance* Typed = Cast<UPlayerAvatarAnimInstance>(AnimInst))
		{
			Typed->SetState(EPlayerState::Climb);
		}

		bIsInParkour = true;

		// 벽을 정면으로 바라보게 회전 정렬
		const FRotator FaceWallYaw(0.f, (-LedgeNormal).Rotation().Yaw, 0.f);
		SetActorRotation(FaceWallYaw);

		if (auto* Move = GetCharacterMovement())
		{
			Move->StopMovementImmediately();

			if (bZeroGravityWhileClimb)
			{
				SavedGravityScale = Move->GravityScale;
				Move->GravityScale = 0.f; // 루트모션 위로 이동 시 안정
			}
		}

		if (bBlockMoveWhileParkour)
		{
			if (APlayerController* PC = Cast<APlayerController>(GetController()))
			{
				PC->SetIgnoreMoveInput(true);
			}
		}

		AnimInst->OnMontageEnded.RemoveDynamic(this, &APlayerAvatar::HandleMontageEnded);
		const float Play = AnimInst->Montage_Play(ClimbMontage, 1.f);
		if (Play > 0.f)
		{
			AnimInst->OnMontageEnded.AddDynamic(this, &APlayerAvatar::HandleMontageEnded);
		}
		else
		{
			// 실패 시 복구
			bIsInParkour = false;
			if (auto* Move = GetCharacterMovement())
			{
				if (bZeroGravityWhileClimb) Move->GravityScale = SavedGravityScale;
			}
			if (APlayerController* PC = Cast<APlayerController>(GetController()))
			{
				PC->SetIgnoreMoveInput(false);
			}
		}
	}
}

// ───────── 전방 라인트레이스(높이별) ─────────
bool APlayerAvatar::TraceForwardAtHeight(float Height, float Distance, FHitResult& OutHit) const
{
	const FVector Start = GetActorLocation() + FVector(0, 0, Height);
	const FVector End = Start + GetActorForwardVector() * Distance;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(ParkourForward), false, this);
	return GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, Params);
}

// ───────── Vault 판정 & 착지 찾기 ─────────
bool APlayerAvatar::FindVaultLandingLocation(const FHitResult& ObstacleHit, FVector& OutLandLoc) const
{
	// 장애물 너머로 약간 전진한 지점에서 아래로 지면 찾기
	const FVector Ahead = GetActorForwardVector() * (ObstacleHit.Distance + VaultForwardClear);
	const FVector Start = GetActorLocation() + Ahead + FVector(0, 0, HeadHeight);
	const FVector End = Start - FVector(0, 0, LandTraceDown);

	FHitResult GroundHit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(ParkourLand), false, this);
	if (GetWorld()->LineTraceSingleByChannel(GroundHit, Start, End, ECC_Visibility, Params))
	{
		OutLandLoc = GroundHit.ImpactPoint;
		return true;
	}
	return false;
}

// ───────── Climb(단/벽 올라타기) ─────────
bool APlayerAvatar::FindClimbLedgeLocation(const FHitResult& WallHit, FVector& OutLedgeLoc, FVector& OutLedgeNormal) const
{
	// 벽 노멀을 기준으로 살짝 바깥에서 위→아래로 내려찍어 ledge 탐색(간단한 맨틀 판정)
	const FVector WallNormal = WallHit.ImpactNormal;
	OutLedgeNormal = WallNormal;

	const FVector Up = FVector::UpVector;
	const FVector Start = WallHit.ImpactPoint + WallNormal * 10.f + Up * MaxClimbHeight;
	const FVector End = Start - Up * (MaxClimbHeight + 100.f);

	FHitResult LedgeHit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(ParkourLedge), false, this);
	if (GetWorld()->LineTraceSingleByChannel(LedgeHit, Start, End, ECC_Visibility, Params))
	{
		OutLedgeLoc = LedgeHit.ImpactPoint;
		return true;
	}
	return false;
}

bool APlayerAvatar::FindClimbTopStandLocation(const FHitResult& WallHit, FVector& OutStandLoc) const
{
	const FVector WallNormal = WallHit.ImpactNormal; // 캐릭터 쪽을 향함
	const FVector Up = FVector::UpVector;

	// 벽 두께 추정치(프로젝트에 맞춰 30~60 조정)
	const float OverTopDistance = 50.f;

	// 벽을 '넘어간' 지점에서 위→아래로 지면 탐색
	const FVector Start = WallHit.ImpactPoint - WallNormal * OverTopDistance + Up * MaxClimbHeight;
	const FVector End = Start - Up * LandTraceDown;

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(ParkourTop), false, this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		OutStandLoc = Hit.ImpactPoint;

		// 캡슐 반높이만큼 올려 서 있게 보정
		/*if (const UCapsuleComponent* Capsule = GetCapsuleComponent())
		{
			OutStandLoc.Z += Capsule->GetScaledCapsuleHalfHeight();
		}*/
		return true;
	}
	return false;
}
