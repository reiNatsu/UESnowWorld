// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAvatarAnimInstance.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum  class EPlayerState : uint8
{
	Locomotion,
	Attack,
	Jump,
	Hit,
	Die,
	Vault,
	Climb
};


UCLASS()
class UESNOWWORLD_API UPlayerAvatarAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UPlayerAvatarAnimInstance();

	// ── Locomotion 파라미터 ──
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Avatar|Movement")
	float Speed = 0.f; // 2D 속력

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Avatar|Movement")
	float Direction = 0.f; // 이동 방향(도)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Avatar|Movement")
	bool bIsInAir = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Avatar|Movement")
	bool bHasAcceleration = false;

	// ── 상태 ──
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatar|State")
	EPlayerState State;

	// 애님 노티파이/몽타주 끝에서 호출 가능
	UFUNCTION(BlueprintCallable, Category = "Avatar|State")
	void OnStateAnimationEnds();

	// 캐릭터(C++)에서 상태 전환 시 호출
	UFUNCTION(BlueprintCallable, Category = "Avatar|State")
	void SetState(EPlayerState NewState) { State = NewState; }

protected:
	// UAnimInstance overrides
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(Transient)
	ACharacter* OwnerCharacter = nullptr;

	UPROPERTY(Transient)
	class UCharacterMovementComponent* MoveComp = nullptr;


};
