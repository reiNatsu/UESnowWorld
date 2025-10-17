// BaseCharacter.h
// --------------------------------------------
// �÷��̾�/���� �������� ����� ���̽� Ŭ����
// - ü��/����/���
// - �޸���/�ȱ� �ӵ� ����
// - �̵�/�þ�(ȸ��) ���� API
// - ��(�Ǿ� �ĺ���) ����
// --------------------------------------------
#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"   

#include "Delegates/DelegateCombinations.h"

#include "BaseCharacter.generated.h"

// --- ��������Ʈ Ÿ�� ���� (Ŭ���� ��) ---
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, class ABaseCharacter*, DeadCharacter);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHealthChanged); // �Ķ���� ����(������)
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

	// ======= ���� �̵�/�þ� �Է� API =======
	// �̵�/ȸ�� ���� API
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void RequestMove(const FVector& WorldDir, float Scale);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void RequestLook(float YawDelta, float PitchDelta);

	// ======= �޸��� ���� =======
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetRunning(bool bRun);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool IsRunning() const 
	{ 
		return bIsRunning; 
	}

	
	//ȸ��
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void Heal(float Amount);

	// ��� ���� 
	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool IsDead() const
	{
		return bIsDead;
	}

	// �� ����(�Ǿ� �ĺ���)
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

	// �� ���� ASC ������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;

	// (����) ���߿� ���� AttributeSet Ÿ������ ĳ�����ؼ� ����
	/*UPROPERTY()
	TObjectPtr<const UAttributeSet> AttributeSet;*/
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;*/

	// �ǰ� ����
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void PlayHitReact(const FVector& FromLocation);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EndHitReact(); // AnimNotify���� ȣ��

	// === ���� ���� ó��(1ȸ��) ===
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void HandleDeath();  

	// ====== ���� �� ���� �ٽ�: ��������Ʈ ��� BP �̺�Ʈ�� ó�� ======
	// ü�� ���� �� BP���� ���� ����/����Ʈ ó��
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void BP_OnHealthChanged(float NewHealth, float InMaxHealth);

	// ��� �� BP���� ó��(��׵�/����/���� ��)
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void BP_OnDied();

	void SetHPRate();

protected:
	// ======= ���� =======
	UPROPERTY(EditAnywhere, BlueprintReadWrite ,Category = "State")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float Health = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float Defense = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	float ATKDmg = 0.0f;

	// ======= �̵� �ӵ� =======
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RunSpeed = 250.0f;

	// ======= �� =======
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
	ETeamType Team = ETeamType::Neutral;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Anim")
	UAnimMontage* HitReactMontage = nullptr; // ����/��ü �ƹ��ų�

	// ���� ��Ÿ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Anim")
	UAnimMontage* DeathMontage = nullptr;

	// ��Ʈ ���� �÷��� (AnimBP�� ����)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|State")
	bool bJustHit = false;

	public:
		UPROPERTY(BlueprintAssignable, Category = "Event")
		FDHPCanged HPChanged;


private:
	bool bIsDead = false;
	bool bIsRunning = false;
};
