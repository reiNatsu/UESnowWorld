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
 * - ���� ��ü. ����/�̵�/���´� ����.
 * - ���/�������� StageController�� ��� �� ������ ���� ��ȣ�� ����.
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossDied); // StageController���� ��ȣ

UCLASS()
class UESNOWWORLD_API ACHRBoss : public ABaseCharacter
{
	GENERATED_BODY()
	 

public:
	ACHRBoss();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;


    virtual void HandleDeath() override;


	// ===== ������ ���� =====
	UFUNCTION(BlueprintCallable)
	void ApplyBossData(const FBossDataRow& InData);

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Data")
	UDataTable* BossDataTable;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Data")
    FName BossRowName; // ��Reaper_01�� ���� RowName

    UPROPERTY(BlueprintAssignable)
    FOnBossDied OnBossDied;

    // ===== ����/��ų =====
    UFUNCTION(BlueprintCallable)
    bool CanBasicAttack() const;

    UFUNCTION(BlueprintCallable)
    bool CanUseSkill1() const;

    UFUNCTION(BlueprintCallable)
    bool CanUseSkill2() const;

    // ����(�Ӹ��� ���� ����ƽ �浹 ��)
    UFUNCTION(BlueprintCallable)
    void EnterStun(float Duration);

    // ����ũ ��� ����(������ 3/4 ����)
    UFUNCTION(BlueprintCallable)
    void EnterBerserkMode();

    // ���ʷ����� ���� �õ�
    UFUNCTION(BlueprintCallable)
    void TrySabotageGenerator(AActor* GeneratorActor);

protected:

     // ======================== ���� ���� ========================
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    EBossState State = EBossState::Roam;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bIsBerserk = false;

    FBossDataRow Data;                   // ���� ���� ������ ĳ��

    // ��Ÿ�� ������ �ð�
    float NextBasicAttackTime = 0.f;
    float NextSkill1Time = 0.f;
    float NextSkill2Time = 0.f;
    float NextSabotageTime = 0.f;
    float StunEndTime = 0.f;
    float PostAttackEndTime = 0.f;

    float DamageMultiplier = 1.f;       // ����ũ �� ���

    UPROPERTY()
    ACHRBossAIController* BossAI = nullptr;

    // ======================== ���� ó�� �Լ� ========================
    void TickState(float DeltaSeconds);
    void TickRoamOrChase();
    void TryDecideAction();

    void PlayMontage(UAnimMontage* Montage);
    void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    AActor* FindNearestGenerator(float Radius = 600.f) const;
    AActor* GetCurrentTarget() const;

    // �� �浹 �� ���� ó��
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
