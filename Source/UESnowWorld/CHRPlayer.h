// PlayerAvatar.h
// --------------------------------------------
// �÷��̾� ���� ĳ����
// - Enhanced Input ���ε�(IMC/IA)
// - �̵�/��/�޸���/����
// - ī�޶� ��(TargetArmLength) & ��ġ(TargetOffset) �ǽð� ����
// - �ִ� �ν��Ͻ�(UPlayerAvatarAnimInstance)�� ������ ������ ���ſ� �ʿ��� �͵���
//   ��κ� BaseCharacter + MovementComponent���� ����
// --------------------------------------------


#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h" 

#include "CHRPlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UAnimMontage;
class USphereComponent;

UCLASS()
class UESNOWWORLD_API ACHRPlayer : public ABaseCharacter
{
	GENERATED_BODY()
	

public:
	ACHRPlayer();

	/** ����: ��Ʈ�ѷ��� ������ �� ȣ�� */
	virtual void PossessedBy(AController* NewController) override;

	/** Ŭ��: PlayerState �����Ǹ� ȣ�� */
	virtual void OnRep_PlayerState() override;

	// �ʱ�ȭ/ƽ/�Է� ���ε�
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ���� ���ݷ� ���(BPI ȣ���ؼ� �÷��̾�+���� �ջ�)
	UFUNCTION(BlueprintCallable, Category = "Combat|Stat")
	float ComputeFinalAttackByBPI() const;

	// ���� ���� ����/��ȸ
	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	void SetEquippedWeaponActor(AActor* NewWeapon);

	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	AActor* GetEquippedWeaponActor() const { return EquippedWeaponActor; }

	// IA_Attack Started���� �Ҹ� ����
	void OnAttackStarted(const FInputActionValue& Value);
	void OnAttackCompleted(const FInputActionValue& Value); // ETriggerEvent::Completed ����
	UFUNCTION(BlueprintCallable, Category = "Attack")
	void AttackPressed();


protected:
	// === �迭 ��� ���� ó�� ===
	void PlayUnarmedMontage(int32 Index);
	//void QueueNextUnarmedAttack();
	void PlayWeaponMontage(int32 Index);
	//void QueueNextWeaponAttack();

	/** ù ����(�Ǵ� ���� ����)���� ���� */
	//void StartAttackSection(int32 SectionIndex);

	/** ���� ��� ���� ������ ������ ���� �������� ������ "����" */
	//void QueueNextAttack();

	// [NEW] ���� �ε��� ��� ���� (����/�ߴ� ��å �ݿ�)
	int32 CalcNextWeaponIndex(int32 FromIndex) const;
	int32 CalcNextUnarmedIndex(int32 FromIndex) const;

	// [NEW] ���� �� ���������(����) �ð��� ����
	void StartWeaponChainGrace(int32 NextIndex);
	void StartUnarmedChainGrace(int32 NextIndex);
	void ClearWeaponChainGrace();
	void ClearUnarmedChainGrace();

	//= == ��Ÿ�� ���� �ݹ� == =
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnUnarmedMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	/** ���� ��� ���� ���� �̸��� ��´� (���� �� NAME_None) */
	//FName GetCurrentSectionName() const;

	bool HasWeapon() const { return EquippedWeaponActor != nullptr; }

private:

	/** �÷��̾�� ASC �ʱ�ȭ(����/Ŭ�� ���� ����) */
	void InitASCForPlayer();

	// === ���� ���� ��� (�迭) ===
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Unarmed")
	TArray<UAnimMontage*> UnarmedAttackMontages;

	int32 UnarmedComboIndex = 0;

	// ===== ���� �ִ� ��� (�迭 ���) =====
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Weapon")
	TArray<UAnimMontage*> WeaponAttackMontages;

	int32 WeaponComboIndex = 0;


	//** ���� ��Ÿ�� (���� 4��: Attack0~Attack3 ���� ���̹�) 
	//UPROPERTY(EditDefaultsOnly, Category = "Attack|Montage")
	////UAnimMontage* AttackMontage = nullptr;
	//TArray<UAnimMontage*> WeaponAttackMontages; // == 

	//** ���� �̸���: �����Ϳ��� ���� ���Ǹ�� 100% ��ġ�ϰ� ä���� 
	//UPROPERTY(EditDefaultsOnly, Category = "Attack|Montage")
	//TArray<FName> ComboSections;

	//// ������ ��� �ܹ� ���ݿ�
	//UPROPERTY(EditDefaultsOnly, Category = "Attack|Unarmed")
	////UAnimSequenceBase * UnarmedAttackAnim = nullptr;   // ���� ���� �� ����� ������
	//TArray<UAnimMontage*> UnarmedAttackMontages;


	//UPROPERTY(EditDefaultsOnly, Category = "Attack|Unarmed")
	//FName UnarmedSlotName = TEXT("DefaultSlot");      // AnimGraph�� Slot �̸�(���� DefaultSlot)

	//UPROPERTY(EditDefaultsOnly, Category = "Attack|Unarmed")
	//float UnarmedPlayRate = 1.0f;

	//UPROPERTY(EditDefaultsOnly, Category = "Attack|Unarmed")
	//float UnarmedBlendIn = 0.05f;

	//UPROPERTY(EditDefaultsOnly, Category = "Attack|Unarmed")
	//float UnarmedBlendOut = 0.05f;


	/** ������ ���ǿ��� �Է��� ������ 1Ÿ�� ��ȯ���� */
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Combo")
	bool bLoopComboFromLast = true;

	// ====== ��Ÿ�� ���� ======
	bool bIsAttacking = false; // ��� ��(�Է� ����)
	bool bQueuedNextAttack = false;     // ��� �� �Է� ����
	bool bAttackHeld = false;           // ���� ��ư Ȧ�� ����

	// [NEW] ���� ���� �������(����) �ð��� ������
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Combo")
	float ComboChainGraceWindow = 0.35f;   // ������ �� �ð� �� �Է�/Ȧ��� ��������

	// [NEW] ����/�Ǽ� ���� ���� ���¿� ������ �ε��� �ĺ���
	bool bWeaponCanChainFromEnd = false;
	bool bUnarmedCanChainFromEnd = false;
	int32 WeaponNextFromEnd = 0;
	int32 UnarmedNextFromEnd = 0;
	FTimerHandle WeaponChainGraceTimer;
	FTimerHandle UnarmedChainGraceTimer;

	/** ������ ���� ���(ù 1Ÿ ���� ���ÿ�). ���Ŀ� AnimInstance���� ���� ������ ���� ��� */
	int32 ComboIndex = -1;

	// ��Ÿ�ӿ� ��ȯ�� ���� ��Ÿ�� ������(�������� Ȯ�ο�)
	UPROPERTY(Transient)
	UAnimMontage* UnarmedDynMontage = nullptr;


protected:
	// ======= ī�޶� ���� ��� =======
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess="true"))
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	// ======= Enhanced Input ���� =======
	// IMC/IA�� �����Ϳ��� �Ҵ�(�������Ʈ or �ν�����)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* IMC_Player = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* IA_Move = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* IA_Look = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* IA_Run = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* IA_Jump = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* IA_Attack = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* IA_CameraLength = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	UInputAction* IA_CameraPosition = nullptr;

	

	// ======= ī�޶� �Ķ���� =======
	// �� BP ĸó ����: �� ���� 250 ~ 450
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float TargetArmLengthMin = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float TargetArmLengthMax = 450.f;

	// �� ���� �ӵ�(�ε巴��)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float ZoomInterpSpeed = 10.f;

	// ī�޶� �ǹ� ������(ĳ���� �������� ���� �ø��� ��)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FVector TargetOffset = FVector(0.f, 0.f, 60.f);

	UPROPERTY(EditAnywhere, Category = "Camera|Limits")
	float PitchMinDeg = -60.f;   // �Ʒ��� 60��

	UPROPERTY(EditAnywhere, Category = "Camera|Limits")
	float PitchMaxDeg = 45.f;   // ���� 45��


private:
	// �� ������ �� ������ �����Ͽ� SpringArm.TargetArmLength�� �ݿ�
	float DesiredArmLength = 200.0f;

	// ���� ���� ���� (������ nullptr)
	UPROPERTY(VisibleAnywhere, Category = "Combat|Weapon")
	AActor* EquippedWeaponActor = nullptr;

	// �Է� �ݹ�
	void OnMoveTriggered(const FInputActionValue& Value);       // Input: Vector2 (X=Right, Y=Forward)
	void OnLookTriggered(const FInputActionValue& Value);       // Input: Vector2 (X=Yaw,   Y=Pitch)
	void OnRunStarted(const FInputActionValue& Value);          // Input: Bool/Trigger
	void OnRunCompleted(const FInputActionValue& Value);        // Input: Bool/Trigger
	void OnJumpTriggered(const FInputActionValue& Value);       // Triggered
	void OnCameraLength(const FInputActionValue& Value);        // Input: float (�� ��)
	void OnCameraPosition(const FInputActionValue& Value);      // Input: Vector (X/Y/Z ������ ��ȭ��)



};
