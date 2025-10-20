// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Animation/AnimMontage.h"
#include "BossTypes.generated.h"

// ���� Ÿ��(������ ���̺��� ������ �� �̸����� ���)
// ���� Ÿ��(������ ���̺��� ������ �� �̸����� ���)
UENUM(BlueprintType)
enum class EBossKind : uint8
{
    Unknown = 0,
    Stalker,
    Reaper,
    Warden,
};

USTRUCT(BlueprintType)
struct FBossDataRow : public FTableRowBase
{
    GENERATED_BODY()

    // � �������� ����
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EBossKind BossKinf = EBossKind::Unknown;
    
    // �̵� �⺻ �ӵ�
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float WalkSpeed = 420.0f;

    // �þ�
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float SightRadius = 2500.f;        // ���� Ž�� �Ÿ�
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float LoseSightRadius = 3000.f;    // �þ� ��Ż �Ÿ�
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float PeripheralVisionAngle = 85.f; // �翷 �þ߰�(���ݰ�)

    // ����/��ų
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UAnimMontage* BasicAttackMontage = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UAnimMontage* Skill1Montage = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    UAnimMontage* Skill2Montage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float BasicAttackRange = 180.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Skill1Range = 300.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Skill2Range = 450.f;

    // ��Ÿ��
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float BasicAttackCooldown = 2.5f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Skill1Cooldown = 8.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Skill2Cooldown = 12.f;

    // ���� �� �ĵ�(������)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float PostAttackDelay = 0.6f;

    // ���ʷ����� ����
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float GeneratorSabotageAmount = 0.18f; // ���൵ -18% (0~1 ���� ����, Ȯ������ ����)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float GeneratorSabotageCooldown = 10.f;

    // ����ũ(������ 3/4 ���� �� ����/���ݷ� ���)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float BerserkSpeedMultiplier = 1.25f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float BerserkDamageMultiplier = 1.15f;
};