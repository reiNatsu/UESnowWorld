//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//
//#include "CoreMinimal.h"
//
//#include "Perception/AIPerceptionTypes.h"
//#include "Perception/AIPerceptionComponent.h"
//#include "Perception/AISenseConfig_Sight.h"
//
//#include "AIController.h"
//
//#include "EnemyAIController.generated.h"
//
//class UAIPerceptionComponent;
//class UAISenseConfig_Sight;
//class AEnemyCharacter;
//
//UENUM(BlueprintType)
//enum class EEnemyState : uint8
//{
//	Patrol,
//	Chase,
//	ReturnHome
//};
//
//
//UCLASS()
//class UESNOWWORLD_API AEnemyAIController : public AAIController
//{
//	GENERATED_BODY()
//	
//public:
//	AEnemyAIController();
//
//	virtual void OnPossess(APawn* InPawn) override;
//	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
//	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
//
//public:
//
//
//
//	UPROPERTY(VisibleAnywhere, Category = "AI|Perception")
//	UAIPerceptionComponent* PerceptionComp;
//	
//	UPROPERTY()
//	UAISenseConfig_Sight* SightConfig;
//
//	// ���� ĳ��
//	UPROPERTY()
//	AEnemyCharacter* EnemyChar = nullptr;
//
//	//UPROPERTY()
//	//AActor* CurrentTarget = nullptr;
//	TWeakObjectPtr<AActor> CurrentTarget;
//
//	EEnemyState State = EEnemyState::Patrol;
//	FTimerHandle LoseSightTimerHandle;
//
//
//	// ==== �þ�, ���� �Ķ����
//	// ��ȿ ���� �Ÿ�(Perception SightRadius�� ���������� ���⼭�� 2�� üũ)
//	UPROPERTY(EditAnywhere, Category = "AI|Sense")
//	float SenseMaxDistance = 1600.0f;
//
//	// ��ä�� ����
//	UPROPERTY(EditAnywhere, Category = "AI|Sense")
//	float HalfFOVDegrees = 70.0f;
//
//	// ����Ʈ���̽� ä��(Visibility ����)
//	UPROPERTY(EditAnywhere, Category = "AI|Sense")
//	TEnumAsByte<ECollisionChannel> VisibilityChannel  = ECC_Visibility;
//
//
//	UPROPERTY(EditAnywhere, Category = "AI|Chase")
//	float RepathTimeInterval = 0.15f;   // �ּ� ������ ����(��)
//
//	UPROPERTY(EditAnywhere, Category = "AI|Chase")
//	float RepathDistanceThreshold = 120.f; // Ÿ���� �̸�ŭ �̵��ؾ� ������
//
//	// ����: �߰� �ֱ� ������(ƽ) ����
//	UPROPERTY(EditAnywhere, Category = "AI|Chase")
//	float ChaseRepathTickInterval = 0.25f;   // �߰� �� ������ �ֱ�(��)
//
//	// ����: ����(�ִ� ���� �Ÿ�)  �ʹ� �־����� �����ϰ� ���� ����
//	UPROPERTY(EditAnywhere, Category = "AI|Chase")
//	float MaxChaseLeashDistance = 3000.f;
//
//	UPROPERTY(EditAnywhere, Category = "AI|Chase")
//	float MaxChaseDurationSeconds = 6.0f;
//
//	// ����: �߰� ƽ Ÿ�̸�
//	FTimerHandle ChaseTickTimerHandle;
//
//	// ����: �ֱ� ������ �ð�
//	double LastRepathTime = -1.0;
//	FVector LastRepathGoal = FVector::ZeroVector;
//	TWeakObjectPtr<AActor> LastRepathTarget;
//
//
//	// ����: �߰� ���� �ð�(��)
//	double ChaseStartTime = -1.0;
//
//	// ����: ��ȯ �� �÷���(���·ε� ���������� ��������)
//	bool bReturningHome = false;
//
//	bool bRepathInProgress = false;
//
//	// ����: ���� Ÿ�̸Ӱ� �̹� �ɷ��ִ��� �÷���
//	bool bLoseSightTimerArmed = false;
//
//	// ����: �߰� ƽ �Լ�(�þ�/��� ���� ���� + ������/���� ����)
//	void ChaseTick();
//
//	// === ���� ===
//	void StartPatrol();
//	void PatrolTick();
//	void MoveToNextPatrolPoint();
//
//	void StartChase(AActor* Target);
//	void StopChaseAndReturnToPatrol();
//
//	// Perception �ݹ�
//	UFUNCTION()
//	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
//
//	// �ĺ� ����(��ä��, �Ÿ�, ��ֹ�/����/ ��ũ��)
//	bool ValidateVision(AActor* Actor) const;
//
//	// �þ� ����/���� ���
//	void GetEyes(FVector& OutLoc, FVector& OutDir) const;
//
//	//�Ӹ� ��ġ ���� (head ��Ĺ)
//	bool GetTargetHeadLocation(AActor* Target, FVector& OutHead) const;
//
//	// PingPong/RandomInCircle ������ �ʿ��� ������ ���
//	bool ComputeNextPatrolDestination(FVector& OutDest) const;
//
//	// Lose Sight
//	void HandleLoseSightTimeout();
//
//	//sight �⺻��
//	void SetupSight(float SightRedius, float LoseSightRadius, float PeripheralVisionAngleDegrees);
//
//	void SafeMoveToActor(AActor* Target);
//
//	bool IsLocationReachable(const FVector& From, const FVector& To) const;
//	bool ProjectToNavmesh(const FVector& In, FVector& Out) const;
//
//
//	// ����: ��ȯ ����(Ȩ ��ġ�� �̵� �� ���� �簳)
//	void StartReturnHome();
//
//};
