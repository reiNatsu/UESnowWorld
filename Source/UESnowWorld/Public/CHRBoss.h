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
 * - ���� ��ü. ����/�̵�/���´� ����.
 * - ���/�������� StageController�� ��� �� ������ ���� ��ȣ�� ����.
 */

UCLASS()
class UESNOWWORLD_API ACHRBoss : public ABaseCharacter
{
	GENERATED_BODY()
	

public:
	ACHRBoss();

	virtual void BeginPlay() override;

	// ============== ������Ʈ ==============
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss")
	UBossAbilityComponent* AbilityComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss")
	UBossPhaseComponent* PhaseComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Sense")
	UAIPerceptionComponent* Perception;

	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;


	/** ================== Stage Controller ���� ================== */
	// StageController�� ���� ���� ���������� ���
	UPROPERTY(BlueprintReadOnly, Category = "Boss|Stage")
	AStageController* StageControllerRef;

	// ============== ���� ����(���� Ʃ��) ==============

	// �ӵ��� 1����. �ʿ�� ������� ����(����)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Spec")
	float MoveSpeed = 350.0f;

	// ��ġ�� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Spec")
	float BackstabDotThreshold = 0.65f;

	// �Ӹ� �浹 ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Stun")
	float StunDuration = 1.2f;

	// ������ ���൵ ����(�������� ���� ����)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Generator")
	float GenerationReduceAmount = 0.2f; // 20&

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Generator")
	float GeneratorReduceCooldown = 10.0f;

	// ============== ������Ʈ ==============
	UPROPERTY(BlueprintReadOnly, Category = "Boss|State")
	bool bIsStunned = false;

	UPROPERTY(BlueprintReadOnly, Category = "Boss|State")
	bool bIsDead = false;

	UPROPERTY(BlueprintReadOnly, Category = "Boss|State")
	float LastGenerationReduceTime = -1000.0f;

	// ==============  Events / API  ==============
	// ��ġ�� ��� �õ�(��Ʈ ó�� ��ο��� ȣ��)
	UFUNCTION(BlueprintCallable)
	bool TryInstantDeathFromBackstab(AActor* Attacker, const FVector& HitFromDir);

	// �Ӹ� �浹(�ı� �Ұ� ������Ʈ) -> ����
	UFUNCTION(BlueprintCallable)
	void OnHeadBlocked(const FHitResult& Hit);

	UFUNCTION(BlueprintCallable)
	void EndStun();

	//// ��� ������
	virtual void HandleDeath() override;

	UFUNCTION(BlueprintCallable)
	void RespawnAtRandomPoint();

	// ������ ¡�൵ ����
	UFUNCTION(BlueprintCallable)
	void ReduceGeneratorProgress(AActor* GeneratorActor);

	// �ӵ� up, ���ݷ� up ��� ����(������ 3�� ������)
	UFUNCTION(BlueprintCallable)
	void EnterSpeedUpMode(float SpeedMul = 1.2f, float DamageMul = 1.1f);
};
