// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Animation/AnimMontage.h"
#include "BossTypes.generated.h"

// 보스 타입(데이터 테이블에서 선택할 열 이름으로 사용)
// 보스 타입(데이터 테이블에서 선택할 열 이름으로 사용)
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

    // 어떤 보스인지 구분
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EBossKind BossKinf = EBossKind::Unknown;
    
    // 이동 기본 속도
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float WalkSpeed = 420.0f;

    // 시야
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float SightRadius = 2500.f;        // 넓은 탐지 거리
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float LoseSightRadius = 3000.f;    // 시야 이탈 거리
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float PeripheralVisionAngle = 85.f; // 양옆 시야각(절반각)

    // 공격/스킬
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

    // 쿨타임
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float BasicAttackCooldown = 2.5f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Skill1Cooldown = 8.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float Skill2Cooldown = 12.f;

    // 공격 후 후딜(딜레이)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float PostAttackDelay = 0.6f;

    // 제너레이터 방해
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float GeneratorSabotageAmount = 0.18f; // 진행도 -18% (0~1 사이 가정, 확실하지 않음)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float GeneratorSabotageCooldown = 10.f;

    // 버서크(발전기 3/4 수리 후 가속/공격력 배수)
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float BerserkSpeedMultiplier = 1.25f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float BerserkDamageMultiplier = 1.15f;
};