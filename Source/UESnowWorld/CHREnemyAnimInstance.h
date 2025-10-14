// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CHREnemyAnimInstance.generated.h"

class ACHREnemy;
class UCharacterMovementComponent;

/** 적 애님 인스턴스: Speed/공중/추격 여부 제공 */
UCLASS()
class UESNOWWORLD_API UCHREnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
    UPROPERTY(BlueprintReadOnly) ACHREnemy* Owner = nullptr;
    UPROPERTY(BlueprintReadOnly) UCharacterMovementComponent* Move = nullptr;

public:
    float Speed01 = 0.f;
    float SpeedABS = 0.f; 

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim") float Speed = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim") bool bIsInAir = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim") bool bIsChasing = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim") bool bIsAttacking = false; // 노티파이로 관리 권장
};
