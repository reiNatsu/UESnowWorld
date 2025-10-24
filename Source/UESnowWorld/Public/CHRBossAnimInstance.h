// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CHRBossAnimInstance.generated.h"

class ACHRBoss;
/** ���� ���� AnimInstance: ���°� ĳ�� + ���� ��� �� */
UCLASS()
class UESNOWWORLD_API UCHRBossAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
    UCHRBossAnimInstance();

    // AnimBP: Event Blueprint Initialize Animation �� �ش� (C++)
    virtual void NativeInitializeAnimation() override;

    // AnimBP: Event Blueprint Update Animation(float DeltaSeconds)
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

    /** C++/BP ��𼭵� ȣ�� ����: ���� ��Ÿ�� ��� */
    UFUNCTION(BlueprintCallable, Category = "Boss|Anim")
    void PlayStunMontage();

    /** ���� ��� �Լ�(����/��ų ��) */
    UFUNCTION(BlueprintCallable, Category = "Boss|Anim")
    float PlayMontage(UAnimMontage* Montage, float InPlayRate = 1.f);

public:
    /** �̵� �ӵ� (BlendSpace��) */
    UPROPERTY(BlueprintReadOnly, Category = "Boss|Anim")
    float Speed = 0.f;

    /** ���� ����(�ʿ��) */
    UPROPERTY(BlueprintReadOnly, Category = "Boss|Anim")
    bool bIsInAir = false;

    /** ���� ���� */
    UPROPERTY(BlueprintReadOnly, Category = "Boss|State")
    bool bIsStunned = false;

    /** ��� ���� */
    UPROPERTY(BlueprintReadOnly, Category = "Boss|State")
    bool bIsDead = false;

    /** Ÿ�� ����(�ο�/�ι� ��ȯ ���� ��) */
    UPROPERTY(BlueprintReadOnly, Category = "Boss|State")
    bool bHasTarget = false;

    /** ĳ�õ� ���� ������ */
    UPROPERTY(BlueprintReadOnly, Category = "Boss|Ref")
    TWeakObjectPtr<ACHRBoss> Boss;

    /** (����) ���� ��Ÿ�� ���� */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Montage")
    TObjectPtr<UAnimMontage> StunMontage = nullptr;
};
