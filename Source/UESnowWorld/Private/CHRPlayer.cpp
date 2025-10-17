// Fill out your copyright ...

#include "CHRPlayer.h"

// 카메라 구성 요소
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// 애니메이션
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"

// 이동/유틸
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Enhanced Input
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerState.h" 

// =======================
//   전투 스텟 합산 예제
// =======================
float ACHRPlayer::ComputeFinalAttackByBPI() const
{
	// ---- 헬퍼 없이 직접 BP 인터페이스 호출 ----
	auto CallFloatGetter = [](UObject* Target, const FName FuncName, float& OutValue) -> bool
		{
			if (!IsValid(Target)) return false;

			if (UFunction* Func = Target->FindFunction(FuncName))
			{
				// 무파라미터 + float 반환 시그니처
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

	// 1) 플레이어(BP_CHRPlayer) 쪽 GetAttackStat 호출
	CallFloatGetter(const_cast<ACHRPlayer*>(this), TEXT("GetAttackStat"), PlayerAtk);

	// 2) 장착 무기가 있으면 무기(BP_Weapon 등) 쪽 GetAttackStat 호출
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
//      입력 → 공격
// =======================

// [CHANGED] 공격 버튼 Down(Started) 시: 홀드 플래그 ON + AttackPressed() 호출
void ACHRPlayer::OnAttackStarted(const FInputActionValue& /*Value*/)
{
	bAttackHeld = true;     // [NEW] 버튼 홀드 시작
	AttackPressed();        // [NEW] 우리의 규칙(버퍼/체인) 기반 입력 처리
}

// [NEW] 공격 버튼 Up(Completed) 시: 홀드 플래그 OFF
void ACHRPlayer::OnAttackCompleted(const FInputActionValue& /*Value*/)
{
	bAttackHeld = false;
}

// [CHANGED] 핵심 입력 처리 규칙
//  - 재생 중이면 '절대' 다시 재생하지 않음 → bQueuedNextAttack = true 로 '한 번만' 버퍼
//  - 재생 중이 아니면: 유예창(끝난 직후) 안이면 다음 인덱스부터, 아니면 1타부터
void ACHRPlayer::AttackPressed()
{
	const bool bHasWeapon = (EquippedWeaponActor != nullptr);

	// [핵심] 재생 중엔 절대 재시작 금지 → 연타 누적을 1비트로만 보관
	if (bIsAttacking)
	{
		bQueuedNextAttack = true; // 끝날 때 OnMontageEnded에서 1회만 소비
		return;
	}

	// [핵심] 재생 중이 아닐 때 시작:
	//  - 유예창 플래그(bWeaponCanChainFromEnd / bUnarmedCanChainFromEnd)가 켜져있으면
	//    그 '다음 인덱스'부터 시작 (체인)
	//  - 아니면 1타부터
	if (bHasWeapon)
	{
		if (bWeaponCanChainFromEnd)        // [NEW] 유예창(끝 직후) 연결
		{
			ClearWeaponChainGrace();       // 유예 플래그 소모
			WeaponComboIndex = WeaponNextFromEnd;
			PlayWeaponMontage(WeaponComboIndex);
		}
		else                               // 일반 시작(1타)
		{
			WeaponComboIndex = 0;
			PlayWeaponMontage(WeaponComboIndex);
		}
	}
	else
	{
		if (bUnarmedCanChainFromEnd)       // [NEW] 유예창(끝 직후) 연결
		{
			ClearUnarmedChainGrace();
			UnarmedComboIndex = UnarmedNextFromEnd;
			PlayUnarmedMontage(UnarmedComboIndex);
		}
		else                               // 일반 시작(1타)
		{
			UnarmedComboIndex = 0;
			PlayUnarmedMontage(UnarmedComboIndex);
		}
	}
}

// =======================
//  재생 함수 (무기/맨손)
// =======================

// [CHANGED] 재생 시작 시 상태 플래그 정리: bIsAttacking=true, bQueuedNextAttack=false
void ACHRPlayer::PlayUnarmedMontage(int32 Index)
{
	if (!GetMesh() || !UnarmedAttackMontages.IsValidIndex(Index)) return;

	if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
	{
		bIsAttacking = true;         // [NEW] 재생 중
		bQueuedNextAttack = false;   // [NEW] 전에 누적된 버퍼는 소거
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
//    다음 인덱스 계산
// =======================

int32 ACHRPlayer::CalcNextWeaponIndex(int32 FromIndex) const
{
	if (WeaponAttackMontages.Num() <= 0) return INDEX_NONE;

	int32 Next = FromIndex + 1;
	if (Next >= WeaponAttackMontages.Num())
	{
		if (!bLoopComboFromLast) return INDEX_NONE; // 마지막에서 멈춤
		Next = 0;                                   // 루프
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
//  유예창(Chain Grace) 관리
//  - 끝나는 순간 '짧은 윈도우' 동안만 체인 허용
//  - 그 시간 지나면 1타부터
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
//  몽타주 종료 콜백(무기/맨손)
//  - 여기서만 '다음으로' 이어갈지 결정
//  - 재생 중 연타는 bQueuedNextAttack 1회로만 인정
// =======================

void ACHRPlayer::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 이 콜백은 모든 몽타주에서 호출될 수 있으니, '무기 배열' 소속인지 확인
	if (!Montage || WeaponAttackMontages.Num() == 0) return;
	if (!WeaponAttackMontages.Contains(Montage)) return;

	const int32 NextIdx = CalcNextWeaponIndex(WeaponComboIndex);

	// [핵심] 끝나는 순간: 버퍼(또는 홀드)가 있으면 '한 번만' 다음으로
	const bool bShouldChain = (bQueuedNextAttack || bAttackHeld);
	bQueuedNextAttack = false;   // [NEW] 여기서 반드시 소모(여러 번 이어지지 않게)

	if (bShouldChain && NextIdx != INDEX_NONE)
	{
		WeaponComboIndex = NextIdx;
		PlayWeaponMontage(WeaponComboIndex);
		return;
	}

	// [NEW] 다음이 존재하지만 지금은 이어가지 않을 때 → 짧은 유예창 열어줌
	if (NextIdx != INDEX_NONE)
	{
		StartWeaponChainGrace(NextIdx);
	}
	else
	{
		ClearWeaponChainGrace();
	}

	// [정리] 재생 종료 상태로
	bIsAttacking = false;
	WeaponComboIndex = 0; // 다음에 새로 누르면 1타부터
}

void ACHRPlayer::OnUnarmedMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 이 콜백은 모든 몽타주에서 호출될 수 있으니, '맨손 배열' 소속인지 확인
	if (!Montage || UnarmedAttackMontages.Num() == 0) return;
	if (!UnarmedAttackMontages.Contains(Montage)) return;

	const int32 NextIdx = CalcNextUnarmedIndex(UnarmedComboIndex);

	const bool bShouldChain = (bQueuedNextAttack || bAttackHeld);
	bQueuedNextAttack = false;   // [NEW] 반드시 소모

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

	// 플레이어는 PlayerState를 Owner로 쓰는 게 가장 안전(복제/권한 일관성)
	/*AActor* OwnerActor = GetPlayerState() ? Cast<AActor>(GetPlayerState()) : GetOwner();
	AbilitySystem->InitAbilityActorInfo(OwnerActor, this);*/

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		// PlayerState가 있으면 그걸 Owner로, 없으면 기존 Owner 사용
		AActor* OwnerActor = GetPlayerState() ? static_cast<AActor*>(GetPlayerState()) : GetOwner();
		//  static_cast조차 필요 없음 - 아래처럼 더 간단히 써도 됨:
		// AActor* OwnerActor = GetPlayerState() ? GetPlayerState() : GetOwner();

		ASC->InitAbilityActorInfo(OwnerActor, this);
	}
}

// =======================
//        생성/초기화
// =======================

ACHRPlayer::ACHRPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	// ===== 카메라 구성 =====
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;        // 카메라 피벗은 컨트롤러 회전
	SpringArm->TargetArmLength = DesiredArmLength;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm);
	FollowCamera->bUsePawnControlRotation = false;     // 카메라는 스프링암이 회전 담당

	// 팀 세팅
	Team = ETeamType::Player;
}

void ACHRPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitASCForPlayer();              // 서버에서 먼저 초기화
}

void ACHRPlayer::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitASCForPlayer();              // 클라에서도 초기화 보장
}

void ACHRPlayer::BeginPlay()
{
	Super::BeginPlay();

	// Enhanced Input IMC 등록
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

	// 카메라 오프셋
	SpringArm->TargetOffset = TargetOffset;

	// [CHANGED] 몽타주 종료 콜백 바인딩(딱 한 번만!)
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		if (UAnimInstance* AnimInst = MeshComp->GetAnimInstance())
		{
			// 같은 델리게이트라도 내부에서 어떤 배열에 속하는지 확인 후 처리
			AnimInst->OnMontageEnded.AddDynamic(this, &ACHRPlayer::OnAttackMontageEnded);
			AnimInst->OnMontageEnded.AddDynamic(this, &ACHRPlayer::OnUnarmedMontageEnded);
		}
	}
}

void ACHRPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 카메라 줌 보간
	SpringArm->TargetArmLength = FMath::FInterpTo(
		SpringArm->TargetArmLength,
		DesiredArmLength,
		DeltaSeconds,
		ZoomInterpSpeed
	);
}

// =======================
//        입력 바인딩
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

		// Run 토글
		if (IA_Run)
		{
			EIC->BindAction(IA_Run, ETriggerEvent::Started, this, &ACHRPlayer::OnRunStarted);
			EIC->BindAction(IA_Run, ETriggerEvent::Completed, this, &ACHRPlayer::OnRunCompleted);
		}

		// Jump
		if (IA_Jump) EIC->BindAction(IA_Jump, ETriggerEvent::Triggered, this, &ACHRPlayer::OnJumpTriggered);

		// Camera Length (휠)
		if (IA_CameraLength) EIC->BindAction(IA_CameraLength, ETriggerEvent::Triggered, this, &ACHRPlayer::OnCameraLength);

		// Camera Position
		if (IA_CameraPosition) EIC->BindAction(IA_CameraPosition, ETriggerEvent::Triggered, this, &ACHRPlayer::OnCameraPosition);

		// [CHANGED] Attack: Started/Completed만 사용 (Triggered는 쓰지 말 것)
		if (IA_Attack)
		{
			EIC->BindAction(IA_Attack, ETriggerEvent::Started, this, &ACHRPlayer::OnAttackStarted);    // 버튼 Down
			EIC->BindAction(IA_Attack, ETriggerEvent::Completed, this, &ACHRPlayer::OnAttackCompleted); // 버튼 Up  [NEW]
		}
	}
}

// =======================
//        이동/룩 등
// =======================

void ACHRPlayer::OnMoveTriggered(const FInputActionValue& Value)
{
	// 공격 중 이동 입력 무시(디자인에 맞게 꺼도 됨)
	if (bIsAttacking) return;

	const FVector2D Axis = Value.Get<FVector2D>();

	// 카메라 Yaw 기준 월드 방향 벡터
	const FRotator YawRot(0.f, Controller ? Controller->GetControlRotation().Yaw : 0.f, 0.f);
	const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);

	RequestMove(Forward, Axis.Y);
	RequestMove(Right, Axis.X);
}

void ACHRPlayer::OnLookTriggered(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>(); // X=Yaw, Y=Pitch

	// Yaw은 기존 API로
	RequestLook(Axis.X, 0.f);

	// Pitch는 직접 누적 + 클램프
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
	if (bIsAttacking) return; // 공격 중 점프 무시(디자인에 따라 변경)
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
