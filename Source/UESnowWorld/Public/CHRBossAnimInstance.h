// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CHRBossAnimInstance.generated.h"

class ACHRBoss;
/** 보스 전용 AnimInstance: 상태값 캐싱 + 간단 재생 훅 */
UCLASS()
class UESNOWWORLD_API UCHRBossAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
    UCHRBossAnimInstance();

    // AnimBP: Event Blueprint Initialize Animation 에 해당 (C++)
    virtual void NativeInitializeAnimation() override;

    // AnimBP: Event Blueprint Update Animation(float DeltaSeconds)
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

    /** C++/BP 어디서든 호출 가능: 스턴 몽타주 재생 */
    UFUNCTION(BlueprintCallable, Category = "Boss|Anim")
    void PlayStunMontage();

    /** 공용 재생 함수(공격/스킬 등) */
    UFUNCTION(BlueprintCallable, Category = "Boss|Anim")
    float PlayMontage(UAnimMontage* Montage, float InPlayRate = 1.f);

public:
    /** 이동 속도 (BlendSpace용) */
    UPROPERTY(BlueprintReadOnly, Category = "Boss|Anim")
    float Speed = 0.f;

    /** 공중 여부(필요시) */
    UPROPERTY(BlueprintReadOnly, Category = "Boss|Anim")
    bool bIsInAir = false;

    /** 스턴 상태 */
    UPROPERTY(BlueprintReadOnly, Category = "Boss|State")
    bool bIsStunned = false;

    /** 사망 상태 */
    UPROPERTY(BlueprintReadOnly, Category = "Boss|State")
    bool bIsDead = false;

    /** 타깃 보유(싸움/로밍 전환 연출 등) */
    UPROPERTY(BlueprintReadOnly, Category = "Boss|State")
    bool bHasTarget = false;

    /** 캐시된 보스 포인터 */
    UPROPERTY(BlueprintReadOnly, Category = "Boss|Ref")
    TWeakObjectPtr<ACHRBoss> Boss;

    /** (선택) 스턴 몽타주 연결 */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Montage")
    TObjectPtr<UAnimMontage> StunMontage = nullptr;
};
