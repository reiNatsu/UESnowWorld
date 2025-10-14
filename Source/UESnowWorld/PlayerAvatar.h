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
	float PitchAngle = -15.f; // ��/������ �ణ �������� ����

	// input handlers
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);

	// ���� Combat ����
	/** ���� �ִϸ��̼� ��Ÿ��(�����Ϳ��� �Ҵ�) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* AttackMontage = nullptr;

	/** ���� ������ ���� (�ߺ��Է� ����) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking = false;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	bool bBlockMoveWhileAttacking = true;

	/** ���� �Է� ó�� */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Attack();

private:
	/** ��Ÿ�� ���� ��������Ʈ �ݹ� */
	UFUNCTION()
	void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted);


private:
    // ���� ���� �� ���� �� ��ǥ ����
    bool bHasPendingStandLocation = false;
    FVector PendingStandLocation = FVector::ZeroVector;

public:
    // ������������������ Parkour: ��Ÿ�� ������������������
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parkour|Montage")
    UAnimMontage* VaultMontage = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Parkour|Montage")
    UAnimMontage* ClimbMontage = nullptr;

    // ������������������ Parkour: �ɼ� ������������������
    /** ���� �� �̵� �Է� ���� ���� */
    UPROPERTY(EditDefaultsOnly, Category = "Parkour|Options")
    bool bBlockMoveWhileParkour = true;

    /** Climb �� �߷� 0���� ��� ���� (��Ʈ����� ���� ���� Ŭ���̸� �ѵθ� ������) */
    UPROPERTY(EditDefaultsOnly, Category = "Parkour|Options")
    bool bZeroGravityWhileClimb = true;

    // ������������������ Parkour: Ʈ���̽� �Ķ���� ������������������
    UPROPERTY(EditDefaultsOnly, Category = "Parkour|Trace")
    float ParkourTraceDistance = 150.f;   // ���� ���� �Ÿ�

    UPROPERTY(EditDefaultsOnly, Category = "Parkour|Trace")
    float KneeHeight = 40.f;              // ���� ����

    UPROPERTY(EditDefaultsOnly, Category = "Parkour|Trace")
    float ChestHeight = 90.f;             // ���� ����

    UPROPERTY(EditDefaultsOnly, Category = "Parkour|Trace")
    float HeadHeight = 150.f;             // �Ӹ� ���� (ĸ��/ĳ���Ϳ� �°� ����)

    UPROPERTY(EditDefaultsOnly, Category = "Parkour|Trace")
    float VaultForwardClear = 70.f;       // ��ֹ� �ʸӷ� �� ���ư� ����

    UPROPERTY(EditDefaultsOnly, Category = "Parkour|Trace")
    float LandTraceDown = 300.f;          // ���� ���� ã�� ���� Ʈ���̽�

    /** ��ֹ� ���� ���� (�� �����Ͽ� �°� ����) */
    UPROPERTY(EditDefaultsOnly, Category = "Parkour|Trace")
    float MinVaultHeight = 25.f;

    UPROPERTY(EditDefaultsOnly, Category = "Parkour|Trace")
    float MaxVaultHeight = 120.f;

    UPROPERTY(EditDefaultsOnly, Category = "Parkour|Trace")
    float MinClimbHeight = 120.f;

    UPROPERTY(EditDefaultsOnly, Category = "Parkour|Trace")
    float MaxClimbHeight = 220.f;

    // ������������������ ���� �÷��� ������������������
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Parkour|State")
    bool bIsInParkour = false;

    // ������ ���� Ʈ���ŷ� ������ (���� �� ���� �õ�)
    void OnJumpPressed();

protected:
    // ������������������ ���� �õ�/���� ������������������
    bool TryVault();
    bool TryClimb();

    void StartVault(const FVector& LandLocation);
    void StartClimb(const FVector& LedgeLocation, const FVector& LedgeNormal);

    // ������������������ Ʈ���̽� ��ƿ ������������������
    bool TraceForwardAtHeight(float Height, float Distance, FHitResult& OutHit) const;
    bool FindVaultLandingLocation(const FHitResult& ObstacleHit, FVector& OutLandLoc) const;
    bool FindClimbLedgeLocation(const FHitResult& WallHit, FVector& OutLedgeLoc, FVector& OutLedgeNormal) const;
    // �� ���(�÷���) ���� �� ��ġ ã��
    bool FindClimbTopStandLocation(const FHitResult& WallHit, FVector& OutStandLoc) const;

private:
    float SavedGravityScale = 1.f;
};
