// PlayerAvatarAnimInstance.h
// --------------------------------------------
// �÷��̾� ���̷�Ż�޽ÿ� �ٴ� AnimInstance
// - �ӵ�, �Է� ����, ���� ����, �޸��� ���� ���� �����
//   �ִ� �׷���(StateMachine/BlendSpace/Sequence)���� ���.
// --------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CHRPlayerAimInstance.generated.h"


class ACHRPlayer;
class UCharacterMovementComponent;

UCLASS()
class UESNOWWORLD_API UCHRPlayerAimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// AnimInstance ���� �ʱ�ȭ(������/��Ÿ�� �� �� ȣ��)
	virtual void NativeInitializeAnimation() override;

	// �� ������ ����
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	// ===== ������ ĳ���� & �����Ʈ =====
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	ACHRPlayer* OwnerCharacter = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	UCharacterMovementComponent* CharacterMovement = nullptr;

public:
	// ===== �ִ� �׷������� ���� ���� ������ =====
	// �̵� �ӵ�(���� ���, m/s)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim")
	float Speed = 0.f;

	// �Է�(�Ǵ� �̵�) ���� ����(��Ʈ�ѷ� Yaw ����)
	// BlendSpace 2D � ��� ����(��/��, ��/��)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim")
	float ForwardInput = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim")
	float SideInput = 0.f;

	// ����/���� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim")
	bool bIsInAir = false;

	// �޸��� ����(�ӵ� ���� ������ ��� ����)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim")
	bool bIsRunning = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim")
	bool bJustBecameInAir = false;   // �̹� �����ӿ� ���� -> �������� �Ѿ

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim")
	bool bJustLanded = false;        // �̹� �����ӿ� ���� -> �������� �Ѿ

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim")
	float VerticalSpeed = 0.f;       // Z�ӵ�(���/�ϰ� �Ǵܿ�)

	// ���� ���� ������
	bool bPrevIsInAir = false;
};
