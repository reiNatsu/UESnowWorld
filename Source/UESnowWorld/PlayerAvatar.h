// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerAvatar.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UAnimMontage;


UCLASS()
class UESNOWWORLD_API APlayerAvatar : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerAvatar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	// Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FollowCamera;

	// movement params
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float WalkSpeed = 450.f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float BaseArmLength = 350.f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float PitchAngle = -15.f; // 뒤/위에서 약간 내려보는 각도

	// input handlers
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);

	// ── Combat ──
	/** 공격 애니메이션 몽타주(에디터에서 할당) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* AttackMontage = nullptr;

	/** 공격 중인지 여부 (중복입력 방지) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking = false;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	bool bBlockMoveWhileAttacking = true;

	/** 공격 입력 처리 */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Attack();

private:
	/** 몽타주 종료 델리게이트 콜백 */
	UFUNCTION()
	void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted);


private:
    // 파쿠르 종료 시 맞춰 설 목표 지점
    bool bHasPendingStandLocation = false;
    FVector PendingStandLocation = FVector::ZeroVector;

public:
    // ───────── Parkour: 몽타주 ─────────
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parkour|Montage")
    UAnimMontage* VaultMontage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parkour|Montage")
    UAnimMontage* ClimbMontage = nullptr;

    // ───────── Parkour: 옵션 ─────────
    /** 파쿠르 시 이동 입력 차단 여부 */
    UPROPERTY(EditDefaultsOnly, Category = "Parkour|Options")
    bool bBlockMoveWhileParkour = true;

    /** Climb 중 중력 0으로 잠시 끄기 (루트모션이 위로 가는 클립이면 켜두면 안정적) */
    UPROPERTY(EditDefaultsOnly, Category = "Parkour|Options")
    bool bZeroGravityWhileClimb = true;

    // ───────── Parkour: 트레이스 파라미터 ─────────
    UPROPERTY(EditDefaultsOnly, Category = "Parkour|Trace")
    float ParkourTraceDistance = 150.f;   // 전방 판정 거리

    UPROPERTY(EditDefaultsOnly, Category = "Parkour|Trace")
    float KneeHeight = 40.f;              // 무릎 높이

    UPROPERTY(EditDefaultsOnly, Category = "Parkour|Trace")
    float ChestHeight = 90.f;             // 가슴 높이

    UPROPERTY(EditDefaultsOnly, Category = "Parkour|Trace")
    float HeadHeight = 150.f;             // 머리 높이 (캡슐/캐릭터에 맞게 조정)

    UPROPERTY(EditDefaultsOnly, Category = "Parkour|Trace")
    float VaultForwardClear = 70.f;       // 장애물 너머로 더 나아갈 여유

    UPROPERTY(EditDefaultsOnly, Category = "Parkour|Trace")
    float LandTraceDown = 300.f;          // 착지 지면 찾기 하향 트레이스

    /** 장애물 높이 기준 (씬 스케일에 맞게 조절) */
    UPROPERTY(EditDefaultsOnly, Category = "Parkour|Trace")
    float MinVaultHeight = 25.f;

    UPROPERTY(EditDefaultsOnly, Category = "Parkour|Trace")
    float MaxVaultHeight = 120.f;

    UPROPERTY(EditDefaultsOnly, Category = "Parkour|Trace")
    float MinClimbHeight = 120.f;

    UPROPERTY(EditDefaultsOnly, Category = "Parkour|Trace")
    float MaxClimbHeight = 220.f;

    // ───────── 상태 플래그 ─────────
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Parkour|State")
    bool bIsInParkour = false;

    // 점프를 파쿠르 트리거로 재정의 (점프 전 파쿠르 시도)
    void OnJumpPressed();

protected:
    // ───────── 파쿠르 시도/실행 ─────────
    bool TryVault();
    bool TryClimb();

    void StartVault(const FVector& LandLocation);
    void StartClimb(const FVector& LedgeLocation, const FVector& LedgeNormal);

    // ───────── 트레이스 유틸 ─────────
    bool TraceForwardAtHeight(float Height, float Distance, FHitResult& OutHit) const;
    bool FindVaultLandingLocation(const FHitResult& ObstacleHit, FVector& OutLandLoc) const;
    bool FindClimbLedgeLocation(const FHitResult& WallHit, FVector& OutLedgeLoc, FVector& OutLedgeNormal) const;
    // 벽 상단(플랫폼) 위에 설 위치 찾기
    bool FindClimbTopStandLocation(const FHitResult& WallHit, FVector& OutStandLoc) const;

private:
    float SavedGravityScale = 1.f;
};
