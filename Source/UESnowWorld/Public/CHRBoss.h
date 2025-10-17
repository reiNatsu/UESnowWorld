// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CHRBoss.generated.h"

//
class UBossAbilityComponent;
class UBossPhaseComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class AStageController;


/**
 * CHRBoss
 * - 보스 본체. 전투/이동/상태는 유지.
 * - 사망/리스폰은 StageController가 담당 → 보스는 죽음 신호만 보냄.
 */

UCLASS()
class UESNOWWORLD_API ACHRBoss : public ABaseCharacter
{
	GENERATED_BODY()
	

public:
	ACHRBoss();

	virtual void BeginPlay() override;

	// ============== 컴포넌트 ==============
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss")
	UBossAbilityComponent* AbilityComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss")
	UBossPhaseComponent* PhaseComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Sense")
	UAIPerceptionComponent* Perception;

	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;


	/** ================== Stage Controller 참조 ================== */
	// StageController가 보스 스폰 관리용으로 사용
	UPROPERTY(BlueprintReadOnly, Category = "Boss|Stage")
	AStageController* StageControllerRef;

	// ============== 보스 스펙(보스 튜닝) ==============

	// 속도는 1종류. 필요시 페이즈에서 가속(곱셈)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Spec")
	float MoveSpeed = 350.0f;

	// 뒤치기 판정
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Spec")
	float BackstabDotThreshold = 0.65f;

	// 머리 충돌 스턴
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Stun")
	float StunDuration = 1.2f;

	// 발전기 진행도 감소(보스별로 차이 있음)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Generator")
	float GenerationReduceAmount = 0.2f; // 20&

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Generator")
	float GeneratorReduceCooldown = 10.0f;

	// ============== 스테이트 ==============
	UPROPERTY(BlueprintReadOnly, Category = "Boss|State")
	bool bIsStunned = false;

	UPROPERTY(BlueprintReadOnly, Category = "Boss|State")
	bool bIsDead = false;

	UPROPERTY(BlueprintReadOnly, Category = "Boss|State")
	float LastGenerationReduceTime = -1000.0f;

	// ==============  Events / API  ==============
	// 뒤치기 즉사 시도(히트 처리 경로에서 호출)
	UFUNCTION(BlueprintCallable)
	bool TryInstantDeathFromBackstab(AActor* Attacker, const FVector& HitFromDir);

	// 머리 충돌(파괴 불가 오브젝트) -> 스턴
	UFUNCTION(BlueprintCallable)
	void OnHeadBlocked(const FHitResult& Hit);

	UFUNCTION(BlueprintCallable)
	void EndStun();

	//// 사망 리스폰
	virtual void HandleDeath() override;

	UFUNCTION(BlueprintCallable)
	void RespawnAtRandomPoint();

	// 발전기 징행도 감소
	UFUNCTION(BlueprintCallable)
	void ReduceGeneratorProgress(AActor* GeneratorActor);

	// 속도 up, 공격력 up 모드 진입(발전기 3개 수리시)
	UFUNCTION(BlueprintCallable)
	void EnterSpeedUpMode(float SpeedMul = 1.2f, float DamageMul = 1.1f);
};
