// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "BossTypes.h"
#include "CHRBoss.generated.h"

//
class ACHRBossAIController;
class UDataTable;

/**
 * CHRBoss
 * - 보스 본체. 전투/이동/상태는 유지.
 * - 사망/리스폰은 StageController가 담당 → 보스는 죽음 신호만 보냄.
 */

UENUM(BlueprintType)
enum class EBossState : uint8
{
	Roam,
	Chase,
	Attack,
	Skill1,
	Skill2,
	Sabotage,
	Stunned,
	Dead
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossDied); // StageController에게 신호

UCLASS()
class UESNOWWORLD_API ACHRBoss : public ABaseCharacter
{
	GENERATED_BODY()
	 

public:
	ACHRBoss();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;


    virtual void HandleDeath() override;


	// ===== 데이터 주입 =====
	UFUNCTION(BlueprintCallable)
	void ApplyBossData(const FBossDataRow& InData);

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Data")
	UDataTable* BossDataTable;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Data")
    FName BossRowName; // “Reaper_01” 같은 RowName

    UPROPERTY(BlueprintAssignable)
    FOnBossDied OnBossDied;

    // ===== 전투/스킬 =====
    UFUNCTION(BlueprintCallable)
    bool CanBasicAttack() const;

    UFUNCTION(BlueprintCallable)
    bool CanUseSkill1() const;

    UFUNCTION(BlueprintCallable)
    bool CanUseSkill2() const;

    // 스턴(머리로 월드 스태틱 충돌 시)
    UFUNCTION(BlueprintCallable)
    void EnterStun(float Duration);

    // 버서크 모드 적용(발전기 3/4 수리)
    UFUNCTION(BlueprintCallable)
    void EnterBerserkMode();

    // 제너레이터 방해 시도
    UFUNCTION(BlueprintCallable)
    void TrySabotageGenerator(AActor* GeneratorActor);

protected:

     // ======================== 내부 상태 ========================
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    EBossState State = EBossState::Roam;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bIsBerserk = false;

    FBossDataRow Data;                   // 현재 보스 데이터 캐시

    // 쿨타임 관리용 시간
    float NextBasicAttackTime = 0.f;
    float NextSkill1Time = 0.f;
    float NextSkill2Time = 0.f;
    float NextSabotageTime = 0.f;
    float StunEndTime = 0.f;
    float PostAttackEndTime = 0.f;

    float DamageMultiplier = 1.f;       // 버서크 시 배수

    UPROPERTY()
    ACHRBossAIController* BossAI = nullptr;

    // ======================== 내부 처리 함수 ========================
    void TickState(float DeltaSeconds);
    void TickRoamOrChase();
    void TryDecideAction();

    void PlayMontage(UAnimMontage* Montage);
    void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    AActor* FindNearestGenerator(float Radius = 600.f) const;
    AActor* GetCurrentTarget() const;

    // 벽 충돌 시 스턴 처리
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
