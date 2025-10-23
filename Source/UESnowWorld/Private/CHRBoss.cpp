// CHRBoss.cpp
// ------------------------------------------------------------
// BaseCharacter�� ������� �� ���� ���� ����
// - Tick���� ���¸ӽ� ����
// - ����/��ų/�纸Ÿ��/���� ó��
// - �θ��� HandleDeath() �������̵�� StageController ���
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

	// �� �浹�� �̺�Ʈ (�Ӹ� ��ġ�� �ÿ� ����)
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ACHRBoss::OnHit);
}

void ACHRBoss::BeginPlay()
{

	Super::BeginPlay();

	BossAI = Cast<ACHRBossAIController>(GetController());

	// DataTable �ο������ ������ �ҷ�����
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
// �θ� HandleDeath() �������̵�
// BaseCharacter::HandleDeath() �� ü��/�÷���/�̺�Ʈ ȣ��
// ���� StageController ���� �� �߰� ó��
// ====================================================================
void ACHRBoss::HandleDeath()
{
	// ���� �θ� ���� ��� ó��(bIsDead �÷���, ��Ÿ��, BP_OnDied ....)
	Super::HandleDeath();

	// ======= �߰� =======
	// ���� ���� Dead�� ����
	State = EBossState::Dead;

	// AI �̵� ����
	if (BossAI)
		BossAI->StopMovement();

	// StageController �� ������ Ʈ���� ��ȣ
	OnBossDied.Broadcast();

	// ���� �ð� �� ��ü �ڵ� ����
	SetLifeSpan(3.0f);

}

// ====================================================================
// DataTable �� ĳ���� ������ �ݿ�
// ====================================================================
void ACHRBoss::ApplyBossData(const FBossDataRow& InData)
{

	Data = InData;

	// �̵� �ӵ�
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
		Move->MaxWalkSpeed = Data.WalkSpeed;

	// �þ� ���� (AI Controller �ʿ� ����)
	if (BossAI)
		BossAI->ConfigureSight(Data.SightRadius, Data.LoseSightRadius, Data.PeripheralVisionAngle);

	DamageMultiplier = 1.0f;
}


// ====================================================================
// ��Ÿ�� Ȯ�� �Լ���


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
// ���� ó�� (�� �浹 ��)

void ACHRBoss::EnterStun(float Duration)
{
	if (State == EBossState::Dead)
		return;

	StunEndTime = GetWorld()->TimeSeconds + Duration;
	State = EBossState::Stunned;

	// �̵� ����
	if (BossAI)
		BossAI->StopMovement();

	// TODO : ���� �ִϸ��̼� PlayMontage();
}

// ====================================================================
// ����ũ ��� (������ 3/4 �Ϸ� ��)
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
// ���ʷ����� ���� �õ�
// ====================================================================
void ACHRBoss::TrySabotageGenerator(AActor* GeneratorActor)
{

	if (!GeneratorActor)
		return;

	if (GetWorld()->TimeSeconds < NextSabotageTime)
		return;

	// �������̽� ȣ��
	if (GeneratorActor->GetClass()->ImplementsInterface(UGeneratorInteractable::StaticClass()))
	{
		State = EBossState::Sabotage;

		// ���ʷ����� ���൵ ����
		IGeneratorInteractable::Execute_ReduceProgress(GeneratorActor, Data.GeneratorSabotageAmount);

		NextSabotageTime = GetWorld()->TimeSeconds + Data.GeneratorSabotageCooldown;

		// ���� �� ��� �ĵ�
		PostAttackEndTime = GetWorld()->TimeSeconds + 0.5f;
	}
}


// ====================================================================
// ���� �ӽ� ���� ����
// ====================================================================
void ACHRBoss::TickState(float DeltaSeconds)
{
	const float Now = GetWorld()->TimeSeconds;

	// ���� ���̸� �ð� ������� ���
	if (State == EBossState::Sabotage)
	{
		if (Now >= StunEndTime)
			State = EBossState::Roam;

		return;
	}

	// ����.��ų,�纸Ÿ�� ���̸� �ĵ� Ÿ���� ������ ����
	if (State == EBossState::Attack || State == EBossState::Skill1
		|| State == EBossState::Skill2 || State == EBossState::Sabotage)
	{
		if (Now >= PostAttackEndTime)
			State = EBossState::Roam;

		return;
	}


	// �ι� , �߰� �� �ൿ �Ǵ�
	TickRoamOrChase();
	TryDecideAction();
}

// �ι�/�߰� ���� ����
void ACHRBoss::TickRoamOrChase()
{
	if (GetCurrentTarget())
		State = EBossState::Chase;
	else
		State = EBossState::Roam;
}

// ����/��ų/�纸Ÿ�� ����
void ACHRBoss::TryDecideAction()
{
	AActor* Target = GetCurrentTarget();
	if (!Target)
		return;

	const float Dist = FVector::Dist(Target->GetActorLocation(), GetActorLocation());

	// ��ó ���ʷ����� �켱 ó��
	if (GetWorld()->TimeSeconds >= NextSabotageTime)
	{
		if (AActor* Gen = FindNearestGenerator(600.0f))
		{
			TrySabotageGenerator(Gen);
			return;
		}
	}


	// ��ų �켱���� �� �⺻����
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
// �ִϸ��̼� ��Ÿ�� ���
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
	// ������ ó���� AnimNotify���� ������ ���� 
}


// ====================================================================
// ���ʷ����� Ž�� (�±� ��� ���� Ž��)
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
// ���� Ÿ�� �������� (AIController ����)
// ====================================================================
AActor* ACHRBoss::GetCurrentTarget() const
{
	return BossAI ? BossAI->GetCurrentTarget() : nullptr;
}


// ====================================================================
// �� �浹 �� ����
// ====================================================================
void ACHRBoss::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor) return;

	// �±� "Wall" �Ǵ� "Solid"�� ������Ʈ �浹 �� ����
	const bool bNonDestructible = OtherActor->ActorHasTag(FName("Wall")) || OtherActor->ActorHasTag(FName("Solid"));

	if (bNonDestructible)
	{
		// �浹 ������ ���� ������ ���� ����
		const FVector Forward = GetActorForwardVector();
		const float Facing = FVector::DotProduct(Forward * -1.f, Hit.ImpactNormal);
		if (Facing > 0.65f)
		{
			EnterStun(0.75f);
		}
	}
}
