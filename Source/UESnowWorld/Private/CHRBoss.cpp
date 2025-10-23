// CHRBoss.cpp
// ------------------------------------------------------------
// BaseCharacter를 기반으로 한 보스 전용 로직
// - Tick에서 상태머신 갱신
// - 공격/스킬/사보타주/스턴 처리
// - 부모의 HandleDeath() 오버라이드로 StageController 통신
// ------------------------------------------------------------

#include "CHRBoss.h"
#include "CHRBossAIController.h"
#include "BossTypes.h"
#include "GeneratorInteractable.h"

#include "Perception/AIPerceptionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "Animation/AnimInstance.h"
#include "Engine/DataTable.h"
#include "TimerManager.h"

ACHRBoss::ACHRBoss()
{
	PrimaryActorTick.bCanEverTick = true;

	// 벽 충돌용 이벤트 (머리 박치기 시에 스턴)
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ACHRBoss::OnHit);
}

void ACHRBoss::BeginPlay()
{

	Super::BeginPlay();

	BossAI = Cast<ACHRBossAIController>(GetController());

	// DataTable 로우명으로 데이터 불러오기
	if (BossDataTable && BossRowName != NAME_None)
	{
		if (const FBossDataRow* Row = BossDataTable->FindRow<FBossDataRow>(BossRowName, TEXT("BossInit")))
		{
			ApplyBossData(*Row);
		}
	}
}

void ACHRBoss::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TickState(DeltaSeconds);
}


// ====================================================================
// 부모 HandleDeath() 오버라이드
// BaseCharacter::HandleDeath() → 체력/플래그/이벤트 호출
// 이후 StageController 연동 및 추가 처리
// ====================================================================
void ACHRBoss::HandleDeath()
{
	// 먼저 부모 공통 사망 처리(bIsDead 플랙그, 몽타주, BP_OnDied ....)
	Super::HandleDeath();

	// ======= 추가 =======
	// 보스 상태 Dead로 변경
	State = EBossState::Dead;

	// AI 이동 중지
	if (BossAI)
		BossAI->StopMovement();

	// StageController 에 리스폰 트리거 신호
	OnBossDied.Broadcast();

	// 일정 시간 뒤 시체 자동 제거
	SetLifeSpan(3.0f);

}

// ====================================================================
// DataTable → 캐릭터 데이터 반영
// ====================================================================
void ACHRBoss::ApplyBossData(const FBossDataRow& InData)
{

	Data = InData;

	// 이동 속도
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
		Move->MaxWalkSpeed = Data.WalkSpeed;

	// 시야 설정 (AI Controller 쪽에 전달)
	if (BossAI)
		BossAI->ConfigureSight(Data.SightRadius, Data.LoseSightRadius, Data.PeripheralVisionAngle);

	DamageMultiplier = 1.0f;
}


// ====================================================================
// 쿨타임 확인 함수들


bool ACHRBoss::CanBasicAttack() const
{
	bool isCanBasicAtk = GetWorld()->TimeSeconds >= NextBasicAttackTime && Data.BasicAttackMontage != nullptr;
	return isCanBasicAtk;
}

bool ACHRBoss::CanUseSkill1() const
{
	bool isCasUseSkill1 = GetWorld()->TimeSeconds >= NextSkill1Time && Data.Skill1Montage != nullptr;
	return isCasUseSkill1;
}

bool ACHRBoss::CanUseSkill2() const
{
	bool isCasUseSkill2 = GetWorld()->TimeSeconds >= NextSkill2Time && Data.Skill2Montage != nullptr;
	return isCasUseSkill2;
}


// ====================================================================
// 스턴 처리 (벽 충돌 시)

void ACHRBoss::EnterStun(float Duration)
{
	if (State == EBossState::Dead)
		return;

	StunEndTime = GetWorld()->TimeSeconds + Duration;
	State = EBossState::Stunned;

	// 이동 정지
	if (BossAI)
		BossAI->StopMovement();

	// TODO : 스텅 애니메이션 PlayMontage();
}

// ====================================================================
// 버서크 모드 (발전기 3/4 완료 시)
// ====================================================================
void ACHRBoss::EnterBerserkMode()
{

	if (bIsBerserk)
		return;

	bIsBerserk = true;

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
		Move->MaxWalkSpeed = Data.WalkSpeed * Data.BerserkSpeedMultiplier;

	DamageMultiplier = Data.BerserkDamageMultiplier;
}

// ====================================================================
// 제너레이터 방해 시도
// ====================================================================
void ACHRBoss::TrySabotageGenerator(AActor* GeneratorActor)
{

	if (!GeneratorActor)
		return;

	if (GetWorld()->TimeSeconds < NextSabotageTime)
		return;

	// 인터페이스 호출
	if (GeneratorActor->GetClass()->ImplementsInterface(UGeneratorInteractable::StaticClass()))
	{
		State = EBossState::Sabotage;

		// 제너레이터 진행도 감소
		IGeneratorInteractable::Execute_ReduceProgress(GeneratorActor, Data.GeneratorSabotageAmount);

		NextSabotageTime = GetWorld()->TimeSeconds + Data.GeneratorSabotageCooldown;

		// 방해 후 잠시 후딜
		PostAttackEndTime = GetWorld()->TimeSeconds + 0.5f;
	}
}


// ====================================================================
// 상태 머신 메인 루프
// ====================================================================
void ACHRBoss::TickState(float DeltaSeconds)
{
	const float Now = GetWorld()->TimeSeconds;

	// 스턴 중이면 시간 종료까지 대기
	if (State == EBossState::Sabotage)
	{
		if (Now >= StunEndTime)
			State = EBossState::Roam;

		return;
	}

	// 공격.스킬,사보타주 중이면 후딜 타임이 끝나야 복귀
	if (State == EBossState::Attack || State == EBossState::Skill1
		|| State == EBossState::Skill2 || State == EBossState::Sabotage)
	{
		if (Now >= PostAttackEndTime)
			State = EBossState::Roam;

		return;
	}


	// 로밍 , 추격 중 행동 판단
	TickRoamOrChase();
	TryDecideAction();
}

// 로밍/추격 상태 갱신
void ACHRBoss::TickRoamOrChase()
{
	if (GetCurrentTarget())
		State = EBossState::Chase;
	else
		State = EBossState::Roam;
}

// 공격/스킬/사보타주 결정
void ACHRBoss::TryDecideAction()
{
	AActor* Target = GetCurrentTarget();
	if (!Target)
		return;

	const float Dist = FVector::Dist(Target->GetActorLocation(), GetActorLocation());

	// 근처 제너레이터 우선 처리
	if (GetWorld()->TimeSeconds >= NextSabotageTime)
	{
		if (AActor* Gen = FindNearestGenerator(600.0f))
		{
			TrySabotageGenerator(Gen);
			return;
		}
	}


	// 스킬 우선순위 → 기본공격
	if (CanUseSkill2() && Dist <= Data.Skill2Range)
	{
		State = EBossState::Skill2;
		PlayMontage(Data.Skill2Montage);
		NextSkill2Time = GetWorld()->TimeSeconds + Data.Skill2Cooldown;
		PostAttackEndTime = GetWorld()->TimeSeconds + Data.PostAttackDelay;
		return;
	}

	if (CanUseSkill1() && Dist <= Data.Skill1Range)
	{
		State = EBossState::Skill1;
		PlayMontage(Data.Skill1Montage);
		NextSkill1Time = GetWorld()->TimeSeconds + Data.Skill1Cooldown;
		PostAttackEndTime = GetWorld()->TimeSeconds + Data.PostAttackDelay;
		return;
	}

	if (CanBasicAttack() && Dist <= Data.BasicAttackRange)
	{
		State = EBossState::Attack;
		PlayMontage(Data.BasicAttackMontage);
		NextBasicAttackTime = GetWorld()->TimeSeconds + Data.BasicAttackCooldown;
		PostAttackEndTime = GetWorld()->TimeSeconds + Data.PostAttackDelay;
		return;
	}

}


// ====================================================================
// 애니메이션 몽타주 재생
// ====================================================================
void ACHRBoss::PlayMontage(UAnimMontage* Montage)
{
	if (!Montage || !GetMesh()) return;
	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
	{
		Anim->Montage_Play(Montage);
		Anim->OnMontageEnded.Clear();
		Anim->OnMontageEnded.AddDynamic(this, &ACHRBoss::OnMontageEnded);
	}
}

void ACHRBoss::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 데미지 처리는 AnimNotify에서 별도로 수행 
}


// ====================================================================
// 제너레이터 탐색 (태그 기반 간단 탐색)
// ====================================================================
AActor* ACHRBoss::FindNearestGenerator(float Radius) const
{
	TArray<AActor*> Candidates;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Generator"), Candidates);

	AActor* Best = nullptr;
	float BestDist = Radius;

	for (AActor* A : Candidates)
	{
		const float D = FVector::Dist(A->GetActorLocation(), GetActorLocation());
		if (D <= BestDist)
		{
			BestDist = D;
			Best = A;
		}
	}
	return Best;
}


// ====================================================================
// 현재 타깃 가져오기 (AIController 통해)
// ====================================================================
AActor* ACHRBoss::GetCurrentTarget() const
{
	return BossAI ? BossAI->GetCurrentTarget() : nullptr;
}


// ====================================================================
// 벽 충돌 시 스턴
// ====================================================================
void ACHRBoss::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor) return;

	// 태그 "Wall" 또는 "Solid"인 오브젝트 충돌 시 스턴
	const bool bNonDestructible = OtherActor->ActorHasTag(FName("Wall")) || OtherActor->ActorHasTag(FName("Solid"));

	if (bNonDestructible)
	{
		// 충돌 방향이 거의 정면일 때만 스턴
		const FVector Forward = GetActorForwardVector();
		const float Facing = FVector::DotProduct(Forward * -1.f, Hit.ImpactNormal);
		if (Facing > 0.65f)
		{
			EnterStun(0.75f);
		}
	}
}
