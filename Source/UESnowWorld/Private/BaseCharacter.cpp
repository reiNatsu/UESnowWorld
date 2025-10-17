// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

#include "UESnowWorldGameMode.h"
#include "MissionManager.h"

#include <Kismet/GameplayStatics.h>

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// ĳ���� ȸ�� ����
	// - ���콺/��Ʈ�ѷ��� Yaw ȸ���� ĳ���Ϳ� ���� �ݿ�
	// - Pitch/Roll�� ������� ����(ī�޶� ó��)
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	// �̵� ������Ʈ �ʱ� ����
	// - bOrientRotationToMovement=false: "ī�޶� ���� �̵�"�� ����ȭ
	//   (�÷��̾�/AI ��� ���� �̵� ��Ģ�� ���� ����)
	UCharacterMovementComponent* Move = GetCharacterMovement();
	Move->bOrientRotationToMovement = false;
	Move->RotationRate = FRotator(0.f, 540.f, 0.f);
	Move->MaxWalkSpeed = WalkSpeed;

	AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
	AbilitySystem->SetIsReplicated(true);
	AbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

}

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	/*AActor* OwnerActor = GetOwner();
	if (!OwnerActor) OwnerActor = const_cast<ABaseCharacter*>(this);

	if (AbilitySystem)
	{
		AbilitySystem->InitAbilityActorInfo(OwnerActor, this);
	}*/

	Health = MaxHealth;
	SetHPRate();
	// ü�� �ʱ�ȭ(���� ����)
	//Health = FMath::Clamp(Health, 0.f, MaxHealth);

	// ���� �޸��� ���ο� ���� �̵� �ӵ� �ݿ�
	//GetCharacterMovement()->MaxWalkSpeed = bIsRunning ? RunSpeed : WalkSpeed;
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->MaxWalkSpeed = bIsRunning ? RunSpeed : WalkSpeed;
	}

	
}

void ABaseCharacter::PlayHitReact(const FVector& FromLocation)
{
	bJustHit = true;

	if (HitReactMontage && GetMesh())
	{
		if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
		{
			Anim->Montage_Play(HitReactMontage, 1.f);
		}
	}
	// ���� �� AnimNotify�� EndHitReact() ȣ�� ����
}

void ABaseCharacter::EndHitReact()
{
	bJustHit = false;
}

void ABaseCharacter::HandleDeath()
{
	// �̵�/�Է� ����
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->DisableMovement();
		Move->StopMovementImmediately();
	}
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}

	// �浹 ��Ȱ�� (���ϸ� Pawn�� Ignore �ϵ��� �������Ϸ� ����)
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// �ǰ� ���� �÷��� ����
	bJustHit = false;

	// ���� ��Ÿ��
	if (DeathMontage && GetMesh())
	{
		if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
		{
			Anim->Montage_Play(DeathMontage, 1.f);
		}
	}

	if (Team == ETeamType::Enemy)
	{
		if (auto* GM = Cast<AUESnowWorldGameMode>(GetWorld()->GetAuthGameMode()))
		{
			if (auto* MM = GM->GetMissionManager())
			{
				MM->HandleEvent("OnMonsterDead", this, 1);
			}
		}
	}

	// BP �� (UI/����/FX ��)
	BP_OnDied();

	// ���� �ð� �� ���� ���� (�ʿ� ������ �ּ�)
	SetLifeSpan(5.f);
}

void ABaseCharacter::SetHPRate()
{
	
	if (HPChanged.IsBound())
	{
		HPChanged.Broadcast(Health / MaxHealth);

		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::Red,
			FString::Printf(TEXT("HP: %.1f / %.1f"), Health, MaxHealth)
		);

	}
	else
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::White,
			TEXT("Not In SetHPRate()")
		);
	}
}

//void ABaseCharacter::BP_OnHealthChanged(float NewHealth, float InMaxHealth)
//{
//	// ���� �����. ���� ������ BP����.
//}
//
//void ABaseCharacter::BP_OnDied()
//{
//	// ���� �����. ���� ������ BP����.
//}


void ABaseCharacter::RequestMove(const FVector& WorldDir, float Scale)
{
	// ���� �̵� API
	// - WorldDir: ���� ���� ����(���� ī�޶� Yaw �������� ���� X/Y ��)
	// - Scale: -1 ~ +1 ������ �� ��

	if (IsDead())
		return;

	if (!WorldDir.IsNearlyZero() && FMath::Abs(Scale) > KINDA_SMALL_NUMBER)
	{
		AddMovementInput(WorldDir.GetSafeNormal(), Scale);
	}
}

void ABaseCharacter::RequestLook(float YawDelta, float PitchDelta)
{
	// ���� ȸ�� API(�÷��̾�� ���콺/�е�, AI�� �ʿ�� ���)
	if (IsDead())
		return;

	AddControllerYawInput(YawDelta);
	AddControllerPitchInput(PitchDelta);
}

void ABaseCharacter::SetRunning(bool bRun)
{
	// �޸��� on/off �� �̵� �ӵ� ��ȯ
	bIsRunning = bRun;
	GetCharacterMovement()->MaxWalkSpeed = bIsRunning ? RunSpeed : WalkSpeed;
}

// ===ȸ�� ( ü�� ��ȭ) ===
void ABaseCharacter::Heal(float Amount)
{
	if (IsDead() || Amount <= 0.f) return;

	Health = FMath::Clamp(Health + Amount, 0.f, MaxHealth);

	// ��������Ʈ ��� BP �̺�Ʈ ȣ��
	BP_OnHealthChanged(Health, MaxHealth);

	SetHPRate();
}


// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// === ���� ĳ���� HP��� ( ü�� ��ȭ) ===
void ABaseCharacter::DealDamage(AActor* Target)
{
	if (!Target || ATKDmg <= 0.f) return;

	UGameplayStatics::ApplyDamage(
		Target,
		ATKDmg,
		GetController(),
		this,
		UDamageType::StaticClass()
	);
}

// === ���� ( ü�� ��ȭ) ===
float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float Incoming = FMath::Max(0.f, DamageAmount);
	if (Incoming <= 0.f || bIsDead) return 0.f;

	// ���� ����
	const float Actual = FMath::Max(0.f, Incoming - Defense);

	const float Prev = Health;
	Health = FMath::Clamp(Health - Actual, 0.f, MaxHealth);

	// ü�� ���� �˸�
	BP_OnHealthChanged(Health, MaxHealth);

	SetHPRate();
	// �ǰ� ����
	PlayHitReact(DamageCauser ? DamageCauser->GetActorLocation()
		: (GetActorLocation() - GetActorForwardVector() * 10.f));

	// ��� ����
	if (Health <= 0.f && !bIsDead)
	{
		bIsDead = true;
		HandleDeath();     // << ���� ó��
	}

	return Prev - Health; // ���� ���ҷ� ��ȯ
}

