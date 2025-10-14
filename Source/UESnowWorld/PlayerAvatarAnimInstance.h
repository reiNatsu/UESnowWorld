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

	// ���� Locomotion �Ķ���� ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Avatar|Movement")
	float Speed = 0.f; // 2D �ӷ�

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Avatar|Movement")
	float Direction = 0.f; // �̵� ����(��)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Avatar|Movement")
	bool bIsInAir = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Avatar|Movement")
	bool bHasAcceleration = false;

	// ���� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Avatar|State")
	EPlayerState State;

	// �ִ� ��Ƽ����/��Ÿ�� ������ ȣ�� ����
	UFUNCTION(BlueprintCallable, Category = "Avatar|State")
	void OnStateAnimationEnds();

	// ĳ����(C++)���� ���� ��ȯ �� ȣ��
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
