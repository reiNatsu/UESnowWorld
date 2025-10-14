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


	// 캐릭터 회전 세팅
	// - 마우스/컨트롤러의 Yaw 회전은 캐릭터에 직접 반영
	// - Pitch/Roll은 사용하지 않음(카메라가 처리)
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	// 이동 컴포넌트 초기 세팅
	// - bOrientRotationToMovement=false: "카메라 기준 이동"을 공용화
	//   (플레이어/AI 모두 같은 이동 규칙을 쓰기 위함)
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
	// 체력 초기화(범위 보정)
	//Health = FMath::Clamp(Health, 0.f, MaxHealth);

	// 현재 달리기 여부에 따라 이동 속도 반영
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
	// 끝날 때 AnimNotify로 EndHitReact() 호출 권장
}

void ABaseCharacter::EndHitReact()
{
	bJustHit = false;
}

void ABaseCharacter::HandleDeath()
{
	// 이동/입력 차단
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->DisableMovement();
		Move->StopMovementImmediately();
	}
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		DisableInput(PC);
	}

	// 충돌 비활성 (원하면 Pawn만 Ignore 하도록 프로파일로 조정)
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// 피격 상태 플래그 정리
	bJustHit = false;

	// 죽음 몽타주
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

	// BP 훅 (UI/사운드/FX 등)
	BP_OnDied();

	// 일정 시간 뒤 액터 제거 (필요 없으면 주석)
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
//	// 여긴 비워둠. 실제 구현은 BP에서.
//}
//
//void ABaseCharacter::BP_OnDied()
//{
//	// 여긴 비워둠. 실제 구현은 BP에서.
//}


void ABaseCharacter::RequestMove(const FVector& WorldDir, float Scale)
{
	// 공용 이동 API
	// - WorldDir: 월드 방향 벡터(보통 카메라 Yaw 기준으로 만든 X/Y 축)
	// - Scale: -1 ~ +1 정도의 축 값

	if (IsDead())
		return;

	if (!WorldDir.IsNearlyZero() && FMath::Abs(Scale) > KINDA_SMALL_NUMBER)
	{
		AddMovementInput(WorldDir.GetSafeNormal(), Scale);
	}
}

void ABaseCharacter::RequestLook(float YawDelta, float PitchDelta)
{
	// 공용 회전 API(플레이어는 마우스/패드, AI는 필요시 사용)
	if (IsDead())
		return;

	AddControllerYawInput(YawDelta);
	AddControllerPitchInput(PitchDelta);
}

void ABaseCharacter::SetRunning(bool bRun)
{
	// 달리기 on/off 시 이동 속도 전환
	bIsRunning = bRun;
	GetCharacterMovement()->MaxWalkSpeed = bIsRunning ? RunSpeed : WalkSpeed;
}

// ===회복 ( 체력 변화) ===
void ABaseCharacter::Heal(float Amount)
{
	if (IsDead() || Amount <= 0.f) return;

	Health = FMath::Clamp(Health + Amount, 0.f, MaxHealth);

	// 델리게이트 대신 BP 이벤트 호출
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

// === 상태 캐릭터 HP깍기 ( 체력 변화) ===
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

// === 피해 ( 체력 변화) ===
float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float Incoming = FMath::Max(0.f, DamageAmount);
	if (Incoming <= 0.f || bIsDead) return 0.f;

	// 방어력 적용
	const float Actual = FMath::Max(0.f, Incoming - Defense);

	const float Prev = Health;
	Health = FMath::Clamp(Health - Actual, 0.f, MaxHealth);

	// 체력 변경 알림
	BP_OnHealthChanged(Health, MaxHealth);

	SetHPRate();
	// 피격 연출
	PlayHitReact(DamageCauser ? DamageCauser->GetActorLocation()
		: (GetActorLocation() - GetActorForwardVector() * 10.f));

	// 사망 판정
	if (Health <= 0.f && !bIsDead)
	{
		bIsDead = true;
		HandleDeath();     // << 통합 처리
	}

	return Prev - Health; // 실제 감소량 반환
}

