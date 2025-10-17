// BaseCharacter.h
// --------------------------------------------
// 플레이어/적이 공통으로 사용할 베이스 클래스
// - 체력/피해/사망
// - 달리기/걷기 속도 관리
// - 이동/시야(회전) 공용 API
// - 팀(피아 식별용) 유지
// --------------------------------------------
#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"   

#include "Delegates/DelegateCombinations.h"

#include "BaseCharacter.generated.h"

// --- 델리게이트 타입 선언 (클래스 밖) ---
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, class ABaseCharacter*, DeadCharacter);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthChanged); // 파라미터 없음(안정형)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDHPCanged, float, hpRate);

UENUM(BlueprintType)
enum class ETeamType : uint8
{
	Player UMETA(DisplayName="Player"),
	Enemy UMETA(DisplayName = "Enemy"),
	Neutral UMETA(DisplayName = "Neutral")
};

class UAbilitySystemComponent;

UCLASS()
class UESNOWWORLD_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()


public:
	// Sets default values for this character's properties
	ABaseCharacter();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintPure, Category = "GAS")
	UAbilitySystemComponent* GetASC() const { return AbilitySystem; }

	// ======= 공용 이동/시야 입력 API =======
	// 이동/회전 공용 API
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void RequestMove(const FVector& WorldDir, float Scale);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void RequestLook(float YawDelta, float PitchDelta);

	// ======= 달리기 제어 =======
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetRunning(bool bRun);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsRunning() const 
	{ 
		return bIsRunning; 
	}

	
	//회복
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void Heal(float Amount);

	// 사망 여부 
	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool IsDead() const
	{
		return bIsDead;
	}

	// 팀 정보(피아 식별용)
	UFUNCTION(BlueprintCallable, Category = "Team")
	ETeamType GetTeam() const
	{
		return Team;
	}

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetHealthRatio() const { return MaxHealth > 0.f ? Health / MaxHealth : 0.f; }

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void DealDamage(AActor* Target);

	virtual float TakeDamage(float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ★ 실제 ASC 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;

	// (선택) 나중에 실제 AttributeSet 타입으로 캐스팅해서 보관
	/*UPROPERTY()
	TObjectPtr<const UAttributeSet> AttributeSet;*/
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;*/

	// 피격 연출
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void PlayHitReact(const FVector& FromLocation);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EndHitReact(); // AnimNotify에서 호출

	// === 죽음 공통 처리(1회만) ===
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void HandleDeath();  

	// ====== 여기 두 개가 핵심: 델리게이트 대신 BP 이벤트로 처리 ======
	// 체력 변경 시 BP에서 위젯 갱신/이펙트 처리
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void BP_OnHealthChanged(float NewHealth, float InMaxHealth);

	// 사망 시 BP에서 처리(라그돌/사운드/위젯 등)
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void BP_OnDied();

	void SetHPRate();

protected:
	// ======= 스탯 =======
	UPROPERTY(EditAnywhere, BlueprintReadWrite ,Category = "State")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float Health = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float Defense = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float ATKDmg = 0.0f;

	// ======= 이동 속도 =======
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RunSpeed = 250.0f;

	// ======= 팀 =======
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
	ETeamType Team = ETeamType::Neutral;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Anim")
	UAnimMontage* HitReactMontage = nullptr; // 전신/상체 아무거나

	// 죽음 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Anim")
	UAnimMontage* DeathMontage = nullptr;

	// 히트 진입 플래그 (AnimBP에 전달)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|State")
	bool bJustHit = false;

	public:
		UPROPERTY(BlueprintAssignable, Category = "Event")
		FDHPCanged HPChanged;


private:
	bool bIsDead = false;
	bool bIsRunning = false;
};
