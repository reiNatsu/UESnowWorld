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
//	// 내부 캐시
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
//	// ==== 시야, 인지 파라미터
//	// 유효 감지 거리(Perception SightRadius와 유사하지만 여기서도 2차 체크)
//	UPROPERTY(EditAnywhere, Category = "AI|Sense")
//	float SenseMaxDistance = 1600.0f;
//
//	// 부채꼴 각도
//	UPROPERTY(EditAnywhere, Category = "AI|Sense")
//	float HalfFOVDegrees = 70.0f;
//
//	// 라인트레이스 채널(Visibility 권장)
//	UPROPERTY(EditAnywhere, Category = "AI|Sense")
//	TEnumAsByte<ECollisionChannel> VisibilityChannel  = ECC_Visibility;
//
//
//	UPROPERTY(EditAnywhere, Category = "AI|Chase")
//	float RepathTimeInterval = 0.15f;   // 최소 재지시 간격(초)
//
//	UPROPERTY(EditAnywhere, Category = "AI|Chase")
//	float RepathDistanceThreshold = 120.f; // 타깃이 이만큼 이동해야 재지시
//
//	// 수정: 추격 주기 재지시(틱) 설정
//	UPROPERTY(EditAnywhere, Category = "AI|Chase")
//	float ChaseRepathTickInterval = 0.25f;   // 추격 중 재지시 주기(초)
//
//	// 수정: 리쉬(최대 추적 거리)  너무 멀어지면 포기하고 순찰 복귀
//	UPROPERTY(EditAnywhere, Category = "AI|Chase")
//	float MaxChaseLeashDistance = 3000.f;
//
//	UPROPERTY(EditAnywhere, Category = "AI|Chase")
//	float MaxChaseDurationSeconds = 6.0f;
//
//	// 수정: 추격 틱 타이머
//	FTimerHandle ChaseTickTimerHandle;
//
//	// 수정: 최근 재지시 시각
//	double LastRepathTime = -1.0;
//	FVector LastRepathGoal = FVector::ZeroVector;
//	TWeakObjectPtr<AActor> LastRepathTarget;
//
//
//	// 수정: 추격 시작 시각(초)
//	double ChaseStartTime = -1.0;
//
//	// 수정: 귀환 중 플래그(상태로도 구분하지만 가독성용)
//	bool bReturningHome = false;
//
//	bool bRepathInProgress = false;
//
//	// 수정: 유실 타이머가 이미 걸려있는지 플래그
//	bool bLoseSightTimerArmed = false;
//
//	// 수정: 추격 틱 함수(시야/경로 상태 점검 + 재지시/복귀 결정)
//	void ChaseTick();
//
//	// === 동작 ===
//	void StartPatrol();
//	void PatrolTick();
//	void MoveToNextPatrolPoint();
//
//	void StartChase(AActor* Target);
//	void StopChaseAndReturnToPatrol();
//
//	// Perception 콜백
//	UFUNCTION()
//	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
//
//	// 후보 검증(부채꼴, 거리, 장애물/높이/ 웅크림)
//	bool ValidateVision(AActor* Actor) const;
//
//	// 시야 원점/방향 얻기
//	void GetEyes(FVector& OutLoc, FVector& OutDir) const;
//
//	//머리 위치 샘플 (head 소캣)
//	bool GetTargetHeadLocation(AActor* Target, FVector& OutHead) const;
//
//	// PingPong/RandomInCircle 순찰에 필요한 목적기 계산
//	bool ComputeNextPatrolDestination(FVector& OutDest) const;
//
//	// Lose Sight
//	void HandleLoseSightTimeout();
//
//	//sight 기본값
//	void SetupSight(float SightRedius, float LoseSightRadius, float PeripheralVisionAngleDegrees);
//
//	void SafeMoveToActor(AActor* Target);
//
//	bool IsLocationReachable(const FVector& From, const FVector& To) const;
//	bool ProjectToNavmesh(const FVector& In, FVector& Out) const;
//
//
//	// 수정: 귀환 시작(홈 위치로 이동 후 순찰 재개)
//	void StartReturnHome();
//
//};
