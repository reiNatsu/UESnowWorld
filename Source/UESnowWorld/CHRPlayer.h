// PlayerAvatar.h
// --------------------------------------------
// 플레이어 전용 캐릭터
// - Enhanced Input 바인딩(IMC/IA)
// - 이동/룩/달리기/점프
// - 카메라 줌(TargetArmLength) & 위치(TargetOffset) 실시간 조절
// - 애님 인스턴스(UPlayerAvatarAnimInstance)가 참조할 데이터 갱신에 필요한 것들은
//   대부분 BaseCharacter + MovementComponent에서 제공
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

	/** 서버: 컨트롤러가 소유할 때 호출 */
	virtual void PossessedBy(AController* NewController) override;

	/** 클라: PlayerState 복제되면 호출 */
	virtual void OnRep_PlayerState() override;

	// 초기화/틱/입력 바인딩
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 최종 공격력 계산(BPI 호출해서 플레이어+무기 합산)
	UFUNCTION(BlueprintCallable, Category = "Combat|Stat")
	float ComputeFinalAttackByBPI() const;

	// 장착 무기 지정/조회
	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	void SetEquippedWeaponActor(AActor* NewWeapon);

	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
	AActor* GetEquippedWeaponActor() const { return EquippedWeaponActor; }

	// IA_Attack Started에서 불릴 래퍼
	void OnAttackStarted(const FInputActionValue& Value);
	void OnAttackCompleted(const FInputActionValue& Value); // ETriggerEvent::Completed 연결
	UFUNCTION(BlueprintCallable, Category = "Attack")
	void AttackPressed();


protected:
	// === 배열 기반 공격 처리 ===
	void PlayUnarmedMontage(int32 Index);
	//void QueueNextUnarmedAttack();
	void PlayWeaponMontage(int32 Index);
	//void QueueNextWeaponAttack();

	/** 첫 섹션(또는 지정 섹션)부터 시작 */
	//void StartAttackSection(int32 SectionIndex);

	/** 지금 재생 중인 섹션이 끝나면 다음 섹션으로 가도록 "예약" */
	//void QueueNextAttack();

	// [NEW] 다음 인덱스 계산 헬퍼 (루프/중단 정책 반영)
	int32 CalcNextWeaponIndex(int32 FromIndex) const;
	int32 CalcNextUnarmedIndex(int32 FromIndex) const;

	// [NEW] 끝난 뒤 “연결猶豫(유예) 시간” 관리
	void StartWeaponChainGrace(int32 NextIndex);
	void StartUnarmedChainGrace(int32 NextIndex);
	void ClearWeaponChainGrace();
	void ClearUnarmedChainGrace();

	//= == 몽타주 종료 콜백 == =
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnUnarmedMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	/** 현재 재생 중인 섹션 이름을 얻는다 (실패 시 NAME_None) */
	//FName GetCurrentSectionName() const;

	bool HasWeapon() const { return EquippedWeaponActor != nullptr; }

private:

	/** 플레이어용 ASC 초기화(서버/클라 공용 헬퍼) */
	void InitASCForPlayer();

	// === 무기 없는 경우 (배열) ===
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Unarmed")
	TArray<UAnimMontage*> UnarmedAttackMontages;

	int32 UnarmedComboIndex = 0;

	// ===== 무기 있는 경우 (배열 기반) =====
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Weapon")
	TArray<UAnimMontage*> WeaponAttackMontages;

	int32 WeaponComboIndex = 0;


	//** 공격 몽타주 (섹션 4개: Attack0~Attack3 같은 네이밍) 
	//UPROPERTY(EditDefaultsOnly, Category = "Attack|Montage")
	////UAnimMontage* AttackMontage = nullptr;
	//TArray<UAnimMontage*> WeaponAttackMontages; // == 

	//** 섹션 이름들: 에디터에서 실제 섹션명과 100% 일치하게 채워줘 
	//UPROPERTY(EditDefaultsOnly, Category = "Attack|Montage")
	//TArray<FName> ComboSections;

	//// 시퀀스 기반 단발 공격용
	//UPROPERTY(EditDefaultsOnly, Category = "Attack|Unarmed")
	////UAnimSequenceBase * UnarmedAttackAnim = nullptr;   // 무기 없을 때 재생할 시퀀스
	//TArray<UAnimMontage*> UnarmedAttackMontages;


	//UPROPERTY(EditDefaultsOnly, Category = "Attack|Unarmed")
	//FName UnarmedSlotName = TEXT("DefaultSlot");      // AnimGraph의 Slot 이름(보통 DefaultSlot)

	//UPROPERTY(EditDefaultsOnly, Category = "Attack|Unarmed")
	//float UnarmedPlayRate = 1.0f;

	//UPROPERTY(EditDefaultsOnly, Category = "Attack|Unarmed")
	//float UnarmedBlendIn = 0.05f;

	//UPROPERTY(EditDefaultsOnly, Category = "Attack|Unarmed")
	//float UnarmedBlendOut = 0.05f;


	/** 마지막 섹션에서 입력이 있으면 1타로 순환할지 */
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Combo")
	bool bLoopComboFromLast = true;

	// ====== 런타임 상태 ======
	bool bIsAttacking = false; // 재생 중(입력 무시)
	bool bQueuedNextAttack = false;     // 재생 중 입력 버퍼
	bool bAttackHeld = false;           // 공격 버튼 홀드 여부

	// [NEW] “끝 직후 연결猶豫(유예) 시간” 윈도우
	UPROPERTY(EditDefaultsOnly, Category = "Attack|Combo")
	float ComboChainGraceWindow = 0.35f;   // 끝나고 이 시간 내 입력/홀드면 다음으로

	// [NEW] 무기/맨손 각각 유예 상태와 “다음 인덱스 후보”
	bool bWeaponCanChainFromEnd = false;
	bool bUnarmedCanChainFromEnd = false;
	int32 WeaponNextFromEnd = 0;
	int32 UnarmedNextFromEnd = 0;
	FTimerHandle WeaponChainGraceTimer;
	FTimerHandle UnarmedChainGraceTimer;

	/** 시작할 때만 사용(첫 1타 섹션 선택용). 이후엔 AnimInstance에서 현재 섹션을 직접 물어봄 */
	int32 ComboIndex = -1;

	// 런타임에 반환된 동적 몽타주 포인터(끝났는지 확인용)
	UPROPERTY(Transient)
	UAnimMontage* UnarmedDynMontage = nullptr;


protected:
	// ======= 카메라 구성 요소 =======
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess="true"))
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	// ======= Enhanced Input 에셋 =======
	// IMC/IA는 에디터에서 할당(블루프린트 or 인스펙터)
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

	

	// ======= 카메라 파라미터 =======
	// 네 BP 캡처 기준: 줌 범위 250 ~ 450
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float TargetArmLengthMin = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float TargetArmLengthMax = 450.f;

	// 줌 보간 속도(부드럽게)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float ZoomInterpSpeed = 10.f;

	// 카메라 피벗 오프셋(캐릭터 기준으로 위로 올리는 등)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FVector TargetOffset = FVector(0.f, 0.f, 60.f);

	UPROPERTY(EditAnywhere, Category = "Camera|Limits")
	float PitchMinDeg = -60.f;   // 아래로 60도

	UPROPERTY(EditAnywhere, Category = "Camera|Limits")
	float PitchMaxDeg = 45.f;   // 위로 45도


private:
	// 매 프레임 이 값으로 보간하여 SpringArm.TargetArmLength에 반영
	float DesiredArmLength = 200.0f;

	// 현재 장착 무기 (없으면 nullptr)
	UPROPERTY(VisibleAnywhere, Category = "Combat|Weapon")
	AActor* EquippedWeaponActor = nullptr;

	// 입력 콜백
	void OnMoveTriggered(const FInputActionValue& Value);       // Input: Vector2 (X=Right, Y=Forward)
	void OnLookTriggered(const FInputActionValue& Value);       // Input: Vector2 (X=Yaw,   Y=Pitch)
	void OnRunStarted(const FInputActionValue& Value);          // Input: Bool/Trigger
	void OnRunCompleted(const FInputActionValue& Value);        // Input: Bool/Trigger
	void OnJumpTriggered(const FInputActionValue& Value);       // Triggered
	void OnCameraLength(const FInputActionValue& Value);        // Input: float (휠 값)
	void OnCameraPosition(const FInputActionValue& Value);      // Input: Vector (X/Y/Z 오프셋 변화량)



};
