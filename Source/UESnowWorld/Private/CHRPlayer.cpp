// Fill out your copyright ...

#include "CHRPlayer.h"

// ī�޶� ���� ���
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// �ִϸ��̼�
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

// �̵�/��ƿ
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Enhanced Input
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerState.h" 

// =======================
//   ���� ���� �ջ� ����
// =======================
float ACHRPlayer::ComputeFinalAttackByBPI() const
{
	// ---- ���� ���� ���� BP �������̽� ȣ�� ----
	auto CallFloatGetter = [](UObject* Target, const FName FuncName, float& OutValue) -> bool
		{
			if (!IsValid(Target)) return false;

			if (UFunction* Func = Target->FindFunction(FuncName))
			{
				// ���Ķ���� + float ��ȯ �ñ״�ó
				struct FNoParam_RetFloat
				{
					float ReturnValue;
				} Params;
				Params.ReturnValue = 0.f;

				Target->ProcessEvent(Func, &Params);
				OutValue = Params.ReturnValue;
				return true;
			}
			return false;
		};

	float PlayerAtk = 0.f;
	float WeaponAtk = 0.f;

	// 1) �÷��̾�(BP_CHRPlayer) �� GetAttackStat ȣ��
	CallFloatGetter(const_cast<ACHRPlayer*>(this), TEXT("GetAttackStat"), PlayerAtk);

	// 2) ���� ���Ⱑ ������ ����(BP_Weapon ��) �� GetAttackStat ȣ��
	if (AActor* Weapon = EquippedWeaponActor)
	{
		CallFloatGetter(Weapon, TEXT("GetAttackStat"), WeaponAtk);
	}

	return PlayerAtk + WeaponAtk;
}

void ACHRPlayer::SetEquippedWeaponActor(AActor* NewWeapon)
{
	EquippedWeaponActor = NewWeapon;
}

// =======================
//      �Է� �� ����
// =======================

// [CHANGED] ���� ��ư Down(Started) ��: Ȧ�� �÷��� ON + AttackPressed() ȣ��
void ACHRPlayer::OnAttackStarted(const FInputActionValue& /*Value*/)
{
	bAttackHeld = true;     // [NEW] ��ư Ȧ�� ����
	AttackPressed();        // [NEW] �츮�� ��Ģ(����/ü��) ��� �Է� ó��
}

// [NEW] ���� ��ư Up(Completed) ��: Ȧ�� �÷��� OFF
void ACHRPlayer::OnAttackCompleted(const FInputActionValue& /*Value*/)
{
	bAttackHeld = false;
}

// [CHANGED] �ٽ� �Է� ó�� ��Ģ
//  - ��� ���̸� '����' �ٽ� ������� ���� �� bQueuedNextAttack = true �� '�� ����' ����
//  - ��� ���� �ƴϸ�: ����â(���� ����) ���̸� ���� �ε�������, �ƴϸ� 1Ÿ����
void ACHRPlayer::AttackPressed()
{
	const bool bHasWeapon = (EquippedWeaponActor != nullptr);

	// [�ٽ�] ��� �߿� ���� ����� ���� �� ��Ÿ ������ 1��Ʈ�θ� ����
	if (bIsAttacking)
	{
		bQueuedNextAttack = true; // ���� �� OnMontageEnded���� 1ȸ�� �Һ�
		return;
	}

	// [�ٽ�] ��� ���� �ƴ� �� ����:
	//  - ����â �÷���(bWeaponCanChainFromEnd / bUnarmedCanChainFromEnd)�� ����������
	//    �� '���� �ε���'���� ���� (ü��)
	//  - �ƴϸ� 1Ÿ����
	if (bHasWeapon)
	{
		if (bWeaponCanChainFromEnd)        // [NEW] ����â(�� ����) ����
		{
			ClearWeaponChainGrace();       // ���� �÷��� �Ҹ�
			WeaponComboIndex = WeaponNextFromEnd;
			PlayWeaponMontage(WeaponComboIndex);
		}
		else                               // �Ϲ� ����(1Ÿ)
		{
			WeaponComboIndex = 0;
			PlayWeaponMontage(WeaponComboIndex);
		}
	}
	else
	{
		if (bUnarmedCanChainFromEnd)       // [NEW] ����â(�� ����) ����
		{
			ClearUnarmedChainGrace();
			UnarmedComboIndex = UnarmedNextFromEnd;
			PlayUnarmedMontage(UnarmedComboIndex);
		}
		else                               // �Ϲ� ����(1Ÿ)
		{
			UnarmedComboIndex = 0;
			PlayUnarmedMontage(UnarmedComboIndex);
		}
	}
}

// =======================
//  ��� �Լ� (����/�Ǽ�)
// =======================

// [CHANGED] ��� ���� �� ���� �÷��� ����: bIsAttacking=true, bQueuedNextAttack=false
void ACHRPlayer::PlayUnarmedMontage(int32 Index)
{
	if (!GetMesh() || !UnarmedAttackMontages.IsValidIndex(Index)) return;

	if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
	{
		bIsAttacking = true;         // [NEW] ��� ��
		bQueuedNextAttack = false;   // [NEW] ���� ������ ���۴� �Ұ�
		UAnimMontage* Montage = UnarmedAttackMontages[Index];
		AnimInst->Montage_Play(Montage, 1.0f);
	}
}

void ACHRPlayer::PlayWeaponMontage(int32 Index)
{
	if (!GetMesh() || !WeaponAttackMontages.IsValidIndex(Index)) return;

	if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
	{
		bIsAttacking = true;         // [NEW]
		bQueuedNextAttack = false;   // [NEW]
		UAnimMontage* Montage = WeaponAttackMontages[Index];
		AnimInst->Montage_Play(Montage, 1.0f);
	}
}

// =======================
//    ���� �ε��� ���
// =======================

int32 ACHRPlayer::CalcNextWeaponIndex(int32 FromIndex) const
{
	if (WeaponAttackMontages.Num() <= 0) return INDEX_NONE;

	int32 Next = FromIndex + 1;
	if (Next >= WeaponAttackMontages.Num())
	{
		if (!bLoopComboFromLast) return INDEX_NONE; // ���������� ����
		Next = 0;                                   // ����
	}
	return Next;
}

int32 ACHRPlayer::CalcNextUnarmedIndex(int32 FromIndex) const
{
	if (UnarmedAttackMontages.Num() <= 0) return INDEX_NONE;

	int32 Next = FromIndex + 1;
	if (Next >= UnarmedAttackMontages.Num())
	{
		if (!bLoopComboFromLast) return INDEX_NONE;
		Next = 0;
	}
	return Next;
}

// =======================
//  ����â(Chain Grace) ����
//  - ������ ���� 'ª�� ������' ���ȸ� ü�� ���
//  - �� �ð� ������ 1Ÿ����
// =======================

void ACHRPlayer::StartWeaponChainGrace(int32 NextIndex)
{
	bWeaponCanChainFromEnd = true;
	WeaponNextFromEnd = NextIndex;

	if (UWorld* W = GetWorld())
	{
		FTimerDelegate D = FTimerDelegate::CreateUObject(this, &ACHRPlayer::ClearWeaponChainGrace);
		W->GetTimerManager().SetTimer(WeaponChainGraceTimer, D, ComboChainGraceWindow, false);
	}
}

void ACHRPlayer::StartUnarmedChainGrace(int32 NextIndex)
{
	bUnarmedCanChainFromEnd = true;
	UnarmedNextFromEnd = NextIndex;

	if (UWorld* W = GetWorld())
	{
		FTimerDelegate D = FTimerDelegate::CreateUObject(this, &ACHRPlayer::ClearUnarmedChainGrace);
		W->GetTimerManager().SetTimer(UnarmedChainGraceTimer, D, ComboChainGraceWindow, false);
	}
}

void ACHRPlayer::ClearWeaponChainGrace()
{
	bWeaponCanChainFromEnd = false;
	WeaponNextFromEnd = 0;

	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().ClearTimer(WeaponChainGraceTimer);
	}
}

void ACHRPlayer::ClearUnarmedChainGrace()
{
	bUnarmedCanChainFromEnd = false;
	UnarmedNextFromEnd = 0;

	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().ClearTimer(UnarmedChainGraceTimer);
	}
}

// =======================
//  ��Ÿ�� ���� �ݹ�(����/�Ǽ�)
//  - ���⼭�� '��������' �̾�� ����
//  - ��� �� ��Ÿ�� bQueuedNextAttack 1ȸ�θ� ����
// =======================

void ACHRPlayer::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// �� �ݹ��� ��� ��Ÿ�ֿ��� ȣ��� �� ������, '���� �迭' �Ҽ����� Ȯ��
	if (!Montage || WeaponAttackMontages.Num() == 0) return;
	if (!WeaponAttackMontages.Contains(Montage)) return;

	const int32 NextIdx = CalcNextWeaponIndex(WeaponComboIndex);

	// [�ٽ�] ������ ����: ����(�Ǵ� Ȧ��)�� ������ '�� ����' ��������
	const bool bShouldChain = (bQueuedNextAttack || bAttackHeld);
	bQueuedNextAttack = false;   // [NEW] ���⼭ �ݵ�� �Ҹ�(���� �� �̾����� �ʰ�)

	if (bShouldChain && NextIdx != INDEX_NONE)
	{
		WeaponComboIndex = NextIdx;
		PlayWeaponMontage(WeaponComboIndex);
		return;
	}

	// [NEW] ������ ���������� ������ �̾�� ���� �� �� ª�� ����â ������
	if (NextIdx != INDEX_NONE)
	{
		StartWeaponChainGrace(NextIdx);
	}
	else
	{
		ClearWeaponChainGrace();
	}

	// [����] ��� ���� ���·�
	bIsAttacking = false;
	WeaponComboIndex = 0; // ������ ���� ������ 1Ÿ����
}

void ACHRPlayer::OnUnarmedMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// �� �ݹ��� ��� ��Ÿ�ֿ��� ȣ��� �� ������, '�Ǽ� �迭' �Ҽ����� Ȯ��
	if (!Montage || UnarmedAttackMontages.Num() == 0) return;
	if (!UnarmedAttackMontages.Contains(Montage)) return;

	const int32 NextIdx = CalcNextUnarmedIndex(UnarmedComboIndex);

	const bool bShouldChain = (bQueuedNextAttack || bAttackHeld);
	bQueuedNextAttack = false;   // [NEW] �ݵ�� �Ҹ�

	if (bShouldChain && NextIdx != INDEX_NONE)
	{
		UnarmedComboIndex = NextIdx;
		PlayUnarmedMontage(UnarmedComboIndex);
		return;
	}

	if (NextIdx != INDEX_NONE)
	{
		StartUnarmedChainGrace(NextIdx);
	}
	else
	{
		ClearUnarmedChainGrace();
	}

	bIsAttacking = false;
	UnarmedComboIndex = 0;
}

void ACHRPlayer::InitASCForPlayer()
{
	if (!AbilitySystem) return;

	// �÷��̾�� PlayerState�� Owner�� ���� �� ���� ����(����/���� �ϰ���)
	/*AActor* OwnerActor = GetPlayerState() ? Cast<AActor>(GetPlayerState()) : GetOwner();
	AbilitySystem->InitAbilityActorInfo(OwnerActor, this);*/

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		// PlayerState�� ������ �װ� Owner��, ������ ���� Owner ���
		AActor* OwnerActor = GetPlayerState() ? static_cast<AActor*>(GetPlayerState()) : GetOwner();
		//  static_cast���� �ʿ� ���� - �Ʒ�ó�� �� ������ �ᵵ ��:
		// AActor* OwnerActor = GetPlayerState() ? GetPlayerState() : GetOwner();

		ASC->InitAbilityActorInfo(OwnerActor, this);
	}
}

// =======================
//        ����/�ʱ�ȭ
// =======================

ACHRPlayer::ACHRPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	// ===== ī�޶� ���� =====
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;        // ī�޶� �ǹ��� ��Ʈ�ѷ� ȸ��
	SpringArm->TargetArmLength = DesiredArmLength;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm);
	FollowCamera->bUsePawnControlRotation = false;     // ī�޶�� ���������� ȸ�� ���

	// �� ����
	Team = ETeamType::Player;
}

void ACHRPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitASCForPlayer();              // �������� ���� �ʱ�ȭ
}

void ACHRPlayer::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitASCForPlayer();              // Ŭ�󿡼��� �ʱ�ȭ ����
}

void ACHRPlayer::BeginPlay()
{
	Super::BeginPlay();

	// Enhanced Input IMC ���
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsys
			= ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (IMC_Player)
			{
				Subsys->AddMappingContext(IMC_Player, 0);
			}
		}
	}

	// ī�޶� ������
	SpringArm->TargetOffset = TargetOffset;

	// [CHANGED] ��Ÿ�� ���� �ݹ� ���ε�(�� �� ����!)
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		if (UAnimInstance* AnimInst = MeshComp->GetAnimInstance())
		{
			// ���� ��������Ʈ�� ���ο��� � �迭�� ���ϴ��� Ȯ�� �� ó��
			AnimInst->OnMontageEnded.AddDynamic(this, &ACHRPlayer::OnAttackMontageEnded);
			AnimInst->OnMontageEnded.AddDynamic(this, &ACHRPlayer::OnUnarmedMontageEnded);
		}
	}
}

void ACHRPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// ī�޶� �� ����
	SpringArm->TargetArmLength = FMath::FInterpTo(
		SpringArm->TargetArmLength,
		DesiredArmLength,
		DeltaSeconds,
		ZoomInterpSpeed
	);
}

// =======================
//        �Է� ���ε�
// =======================

void ACHRPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Move: Vector2 (X=Right, Y=Forward)
		if (IA_Move) EIC->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ACHRPlayer::OnMoveTriggered);

		// Look: Vector2 (X=Yaw, Y=Pitch)
		if (IA_Look) EIC->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ACHRPlayer::OnLookTriggered);

		// Run ���
		if (IA_Run)
		{
			EIC->BindAction(IA_Run, ETriggerEvent::Started, this, &ACHRPlayer::OnRunStarted);
			EIC->BindAction(IA_Run, ETriggerEvent::Completed, this, &ACHRPlayer::OnRunCompleted);
		}

		// Jump
		if (IA_Jump) EIC->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &ACHRPlayer::OnJumpTriggered);

		// Camera Length (��)
		if (IA_CameraLength) EIC->BindAction(IA_CameraLength, ETriggerEvent::Triggered, this, &ACHRPlayer::OnCameraLength);

		// Camera Position
		if (IA_CameraPosition) EIC->BindAction(IA_CameraPosition, ETriggerEvent::Triggered, this, &ACHRPlayer::OnCameraPosition);

		// [CHANGED] Attack: Started/Completed�� ��� (Triggered�� ���� �� ��)
		if (IA_Attack)
		{
			EIC->BindAction(IA_Attack, ETriggerEvent::Started, this, &ACHRPlayer::OnAttackStarted);    // ��ư Down
			EIC->BindAction(IA_Attack, ETriggerEvent::Completed, this, &ACHRPlayer::OnAttackCompleted); // ��ư Up  [NEW]
		}
	}
}

// =======================
//        �̵�/�� ��
// =======================

void ACHRPlayer::OnMoveTriggered(const FInputActionValue& Value)
{
	// ���� �� �̵� �Է� ����(�����ο� �°� ���� ��)
	if (bIsAttacking) return;

	const FVector2D Axis = Value.Get<FVector2D>();

	// ī�޶� Yaw ���� ���� ���� ����
	const FRotator YawRot(0.f, Controller ? Controller->GetControlRotation().Yaw : 0.f, 0.f);
	const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	RequestMove(Forward, Axis.Y);
	RequestMove(Right, Axis.X);
}

void ACHRPlayer::OnLookTriggered(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>(); // X=Yaw, Y=Pitch

	// Yaw�� ���� API��
	RequestLook(Axis.X, 0.f);

	// Pitch�� ���� ���� + Ŭ����
	if (Controller)
	{
		FRotator R = Controller->GetControlRotation();
		R.Pitch = FMath::ClampAngle(R.Pitch + Axis.Y, PitchMinDeg, PitchMaxDeg);
		Controller->SetControlRotation(R);
	}
}

void ACHRPlayer::OnRunStarted(const FInputActionValue& /*Value*/)
{
	SetRunning(true);
}

void ACHRPlayer::OnRunCompleted(const FInputActionValue& /*Value*/)
{
	SetRunning(false);
}

void ACHRPlayer::OnJumpTriggered(const FInputActionValue& /*Value*/)
{
	if (bIsAttacking) return; // ���� �� ���� ����(�����ο� ���� ����)
	if (!IsDead()) Jump();
}

void ACHRPlayer::OnCameraLength(const FInputActionValue& Value)
{
	const float Delta = Value.Get<float>();
	DesiredArmLength = FMath::Clamp(DesiredArmLength + Delta, TargetArmLengthMin, TargetArmLengthMax);
}

void ACHRPlayer::OnCameraPosition(const FInputActionValue& Value)
{
	const FVector Offset = Value.Get<FVector>();
	SpringArm->TargetOffset += Offset;
}
