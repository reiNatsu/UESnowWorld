// PlayerAvatarAnimInstance.h
// --------------------------------------------
// 플레이어 스켈레탈메시에 붙는 AnimInstance
// - 속도, 입력 방향, 공중 여부, 달리기 여부 등을 계산해
//   애님 그래프(StateMachine/BlendSpace/Sequence)에서 사용.
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
	// AnimInstance 최초 초기화(에디터/런타임 둘 다 호출)
	virtual void NativeInitializeAnimation() override;

	// 매 프레임 갱신
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	// ===== 소유자 캐릭터 & 무브먼트 =====
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	ACHRPlayer* OwnerCharacter = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	UCharacterMovementComponent* CharacterMovement = nullptr;

public:
	// ===== 애님 그래프에서 쓰는 노출 변수들 =====
	// 이동 속도(수평 평면, m/s)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim")
	float Speed = 0.f;

	// 입력(또는 이동) 방향 성분(컨트롤러 Yaw 기준)
	// BlendSpace 2D 등에 사용 가능(전/후, 좌/우)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim")
	float ForwardInput = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim")
	float SideInput = 0.f;

	// 점프/낙하 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim")
	bool bIsInAir = false;

	// 달리기 여부(속도 선택 블렌딩에 사용 가능)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim")
	bool bIsRunning = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim")
	bool bJustBecameInAir = false;   // 이번 프레임에 지면 -> 공중으로 넘어감

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim")
	bool bJustLanded = false;        // 이번 프레임에 공중 -> 지면으로 넘어감

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anim")
	float VerticalSpeed = 0.f;       // Z속도(상승/하강 판단용)

	// 내부 상태 추적용
	bool bPrevIsInAir = false;
};
