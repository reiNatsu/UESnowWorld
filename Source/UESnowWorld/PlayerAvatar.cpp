// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAvatar.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "PlayerAvatarAnimInstance.h" // AnimInstance�� ���� ���޿�

// Sets default values
APlayerAvatar::APlayerAvatar()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// ĳ���ʹ� �Է� �������� ȸ�� (��Ʈ�ѷ� yaw�� ī�޶� ȸ������ ���)
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// �̵� ������Ʈ ����
	auto* MoveComp = GetCharacterMovement();
	MoveComp->bOrientRotationToMovement = true;
	MoveComp->RotationRate = FRotator(0.f, 540.f, 0.f);
	MoveComp->MaxWalkSpeed = WalkSpeed;

	// ��������
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = BaseArmLength;
	SpringArm->bUsePawnControlRotation = true;             // ��Ʈ�ѷ� ȸ���� ����
	SpringArm->SetRelativeRotation(FRotator(PitchAngle, 0.f, 0.f));
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 12.f;

	// ī�޶�
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;         // ī�޶�� �������ϸ� ����
}

// Called when the game starts or when spawned
void APlayerAvatar::BeginPlay()
{
	Super::BeginPlay();

	// ��������: ���� �� �ӵ� ����
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


	// ���� Axis ���ε� ����
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerAvatar::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &APlayerAvatar::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APlayerAvatar::Turn);   // Mouse X
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APlayerAvatar::LookUp); // Mouse Y

	// ������ ���� �ʹٸ�:
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ACharacter::StopJumping);

	// Action ���ε�(������Ʈ ���� > Input�� "Attack" ��� �ʿ�)
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
		return; // �ߺ� ���� ���� or ���Ҵ�
	}

	// ���� �ִ� �ν��Ͻ�
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		if (UAnimInstance* AnimInst = MeshComp->GetAnimInstance())
		{
			// ���� ��ȯ: Attack
			if (UPlayerAvatarAnimInstance* Typed = Cast<UPlayerAvatarAnimInstance>(AnimInst))
			{
				Typed->SetState(EPlayerState::Attack);
			}

			// ��Ÿ�� ���
			const float PlayResult = AnimInst->Montage_Play(AttackMontage, 1.0f);
			if (PlayResult > 0.f)
			{
				bIsAttacking = true;

				// ��� ���� (���� ����)
				if (auto* MoveComp = GetCharacterMovement())
				{
					MoveComp->StopMovementImmediately();
					// MoveComp->BrakingFrictionFactor = 0.f; // �ʿ�� ���� �� ���̱� (����)
				}

				//���� �̵� �Է� ����(����) ī�޶� ȸ���� ����
				if (bBlockMoveWhileAttacking)
				{
					if (APlayerController* PC = Cast<APlayerController>(GetController()))
					{
						PC->SetIgnoreMoveInput(true);
					}
				}

				// ��Ÿ�� ���� ����
				AnimInst->OnMontageEnded.AddDynamic(this, &APlayerAvatar::HandleMontageEnded);
			}
		}
	}
}

// ������������������ ��Ÿ�� ����: Attack + Parkour ���� ���� ������������������
void APlayerAvatar::HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// �츮�� ����� ���� ��Ÿ�ְ� �������� Ȯ��
	if (Montage == AttackMontage)
	{
		bIsAttacking = false;

		// �̵� �Է� ����(����)
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

		// ��Ʈ����� ª�� ���� ������ �� ���� ���� ����� ���� ����
		if (bHasPendingStandLocation)
		{
			const float Dist = FVector::Dist2D(GetActorLocation(), PendingStandLocation);
			// 10~30cm �̻� ���̳��� ����(���� ����/������Ʈ�� ����)
			if (Dist > 20.f)
			{
				// �浹 ���� �ڷ���Ʈ(�ɸ� ����). �浹 ����Ϸ��� Sweep �̵� �������� ��ü.
				SetActorLocation(PendingStandLocation, false, nullptr, ETeleportType::TeleportPhysics);
			}
			bHasPendingStandLocation = false;
		}

		// AnimInstance ���� ����
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

	// ���� �켱 �õ�
	if (TryVault() || TryClimb())
	{
		return;
	}

	// ���� �� �Ϲ� ����
	Jump();
}

bool APlayerAvatar::TryVault()
{
	// ���� ���̿����� �°�, �Ӹ� ���̿����� �� ������ �� ���� ��ֹ� = Vault �ĺ� (Ȯ����)
	FHitResult KneeHit, HeadHit;
	const bool bKneeHit = TraceForwardAtHeight(KneeHeight, ParkourTraceDistance, KneeHit);
	const bool bHeadHit = TraceForwardAtHeight(HeadHeight, ParkourTraceDistance, HeadHit);

	if (!bKneeHit || bHeadHit)
		return false;

	// ��ֹ� ���� �ٻ�: �Ӹ�-���� ���̷� �Ǵ�(���� ����)
	const float ObstacleHeight = HeadHeight - KneeHeight; // �ܼ� �ٻ�(������Ʈ�� �°� ���� ����)
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
		// ����/�Է�/�ӵ� ����
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

		// �ٶ󺸴� ������ ���� �������� �̼� ����(����)
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
			// ���� �� ����
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

	// �� ��(�÷���) ���ĵ� ������ ã�� ��
	FVector TopStandLoc;
	bHasPendingStandLocation = FindClimbTopStandLocation(KneeHit, TopStandLoc);
	if (bHasPendingStandLocation) PendingStandLocation = TopStandLoc;

	StartClimb(LedgeLoc, LedgeNormal);
	return true;
	//// ����/�Ӹ� ���� ��� ������ �� ���� ��
	//FHitResult KneeHit, HeadHit;
	//const bool bKneeHit = TraceForwardAtHeight(KneeHeight, ParkourTraceDistance, KneeHit);
	//const bool bHeadHit = TraceForwardAtHeight(HeadHeight, ParkourTraceDistance, HeadHit);

	//if (!(bKneeHit && bHeadHit))
	//	return false;

	//FVector LedgeLoc, LedgeNormal;
	//if (!FindClimbLedgeLocation(KneeHit, LedgeLoc, LedgeNormal))
	//	return false;

	//// ledge ���� �뷫 ����(���� �ٻ�)
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

		// ���� �������� �ٶ󺸰� ȸ�� ����
		const FRotator FaceWallYaw(0.f, (-LedgeNormal).Rotation().Yaw, 0.f);
		SetActorRotation(FaceWallYaw);

		if (auto* Move = GetCharacterMovement())
		{
			Move->StopMovementImmediately();

			if (bZeroGravityWhileClimb)
			{
				SavedGravityScale = Move->GravityScale;
				Move->GravityScale = 0.f; // ��Ʈ��� ���� �̵� �� ����
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
			// ���� �� ����
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

// ������������������ ���� ����Ʈ���̽�(���̺�) ������������������
bool APlayerAvatar::TraceForwardAtHeight(float Height, float Distance, FHitResult& OutHit) const
{
	const FVector Start = GetActorLocation() + FVector(0, 0, Height);
	const FVector End = Start + GetActorForwardVector() * Distance;

	FCollisionQueryParams Params(SCENE_QUERY_STAT(ParkourForward), false, this);
	return GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, Params);
}

// ������������������ Vault ���� & ���� ã�� ������������������
bool APlayerAvatar::FindVaultLandingLocation(const FHitResult& ObstacleHit, FVector& OutLandLoc) const
{
	// ��ֹ� �ʸӷ� �ణ ������ �������� �Ʒ��� ���� ã��
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

// ������������������ Climb(��/�� �ö�Ÿ��) ������������������
bool APlayerAvatar::FindClimbLedgeLocation(const FHitResult& WallHit, FVector& OutLedgeLoc, FVector& OutLedgeNormal) const
{
	// �� ����� �������� ��¦ �ٱ����� ����Ʒ��� ������� ledge Ž��(������ ��Ʋ ����)
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
	const FVector WallNormal = WallHit.ImpactNormal; // ĳ���� ���� ����
	const FVector Up = FVector::UpVector;

	// �� �β� ����ġ(������Ʈ�� ���� 30~60 ����)
	const float OverTopDistance = 50.f;

	// ���� '�Ѿ' �������� ����Ʒ��� ���� Ž��
	const FVector Start = WallHit.ImpactPoint - WallNormal * OverTopDistance + Up * MaxClimbHeight;
	const FVector End = Start - Up * LandTraceDown;

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(ParkourTop), false, this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		OutStandLoc = Hit.ImpactPoint;

		// ĸ�� �ݳ��̸�ŭ �÷� �� �ְ� ����
		/*if (const UCapsuleComponent* Capsule = GetCapsuleComponent())
		{
			OutStandLoc.Z += Capsule->GetScaledCapsuleHalfHeight();
		}*/
		return true;
	}
	return false;
}
