//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "EnemyAIController.h"
//#include "EnemyCharacter.h"
////#include "Perception/AIPerceptionComponent.h"
////#include "Perception/AISenseConfig_Sight.h"
////#include "Perception/AIPerceptionTypes.h"
//#include "Perception/AISense_Sight.h"
//#include "NavigationSystem.h"
//#include "TimerManager.h"
//#include "DrawDebugHelpers.h"
//#include "Navigation/PathFollowingComponent.h"
//#include "AIController.h"
//#include "NavigationPath.h"
//#include "Templates/UnrealTemplate.h"
//
//
////[1]
//AEnemyAIController::AEnemyAIController()
//{
//	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
//	SetPerceptionComponent(*PerceptionComp);
//
//	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
//	// Perception은 "후보"만 제공, 최종 판정은 ValidateVision에서 ??
//	SetupSight(/*SightRadius*/ 1800.f, /*LoseSightRadius*/ 2000.f, /*FOV*/ 90.f);
//
//	PerceptionComp->ConfigureSense(*SightConfig);
//	PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
//	PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
//}
//
////[3]
//void AEnemyAIController::OnPossess(APawn* InPawn)
//{
//	Super::OnPossess(InPawn);
//	EnemyChar = Cast<AEnemyCharacter>(InPawn);
//
//	UE_LOG(LogTemp, Warning, TEXT("[AI] OnPossess Called! Pawn=%s"), *InPawn->GetName()); //
//
//	if (EnemyChar)
//	{
//		FTimerHandle Tmp;
//		GetWorldTimerManager().SetTimer(Tmp, this, &AEnemyAIController::StartPatrol, 0.1f, false);
//
//		// 보강: 1초 뒤에 한 번 더 시도 (초기화 타이밍 문제 방지)
//		FTimerHandle Tmp2;
//		GetWorldTimerManager().SetTimer(Tmp2, this, &AEnemyAIController::StartPatrol, 1.0f, false);
//	}
//
//	//if (EnemyChar)
//	//{
//	//	FTimerHandle Tmp;
//	//	GetWorldTimerManager().SetTimer(Tmp, this, &AEnemyAIController::StartPatrol, 0.1f, false);
//	//	//StartPatrol();
//	//}
//}
////[6]
//void AEnemyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
//{
//	Super::OnMoveCompleted(RequestID, Result);
//
//	if (State == EEnemyState::ReturnHome)
//	{
//		bReturningHome = false;
//		StartPatrol();
//		return;
//	}
//
//	if (State == EEnemyState::Chase && CurrentTarget.IsValid())
//	{
//		FTimerHandle Tmp;
//		GetWorldTimerManager().SetTimer(Tmp, [this]()
//			{
//				if (CurrentTarget.IsValid())
//					SafeMoveToActor(CurrentTarget.Get());
//			}, 0.01f, false);
//
//		return;
//	}
//
//	if (!EnemyChar) return;
//
//	if (State == EEnemyState::Patrol)
//	{
//		if (EnemyChar->PatrolMode == EPatrolMode::PingPongPath)
//			EnemyChar->FlipPingPongDirection();
//		MoveToNextPatrolPoint();
//	}
//}
//
//void AEnemyAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
//{
//	GetWorldTimerManager().ClearTimer(ChaseTickTimerHandle);
//	GetWorldTimerManager().ClearTimer(LoseSightTimerHandle);
//	bLoseSightTimerArmed = false;
//
//	bReturningHome = false;
//	ChaseStartTime = -1.0;
//
//	Super::EndPlay(EndPlayReason);
//}
//
//void AEnemyAIController::StartReturnHome()
//{
//	if (!EnemyChar || !GetWorld()) return;
//
//	// 상태/타이머 정리
//	State = EEnemyState::ReturnHome;
//	bReturningHome = true;
//	CurrentTarget.Reset();
//	GetWorldTimerManager().ClearTimer(ChaseTickTimerHandle);
//	GetWorldTimerManager().ClearTimer(LoseSightTimerHandle);
//	// 홈 위치로 이동
//	const FVector Home = EnemyChar->HomeLocation;
//	const FVector MyLoc = GetPawn() ? GetPawn()->GetActorLocation() : Home;
//	if (FVector::Dist2D(MyLoc, Home) <= EnemyChar->PatrolAcceptanceRadius + 20.f)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("[AI] Already at Home -> StartPatrol"));
//		StartPatrol();
//		return;
//	}
//	MoveToLocation(Home, EnemyChar->PatrolAcceptanceRadius, /*bStopOnOverlap*/ true);
//
//}
//
//void AEnemyAIController::ChaseTick()
//{
//	if (State != EEnemyState::Chase) return;
//
//	// 수정: 시간 초과 체크
//	if (MaxChaseDurationSeconds > 0.f && ChaseStartTime >= 0.0 && GetWorld())
//	{
//		const double Now = GetWorld()->GetTimeSeconds();
//		if ((Now - ChaseStartTime) >= MaxChaseDurationSeconds)
//		{
//			StartReturnHome();
//			return;
//		}
//	}
//
//	// 타깃 무효 → 순찰 복귀
//	if (!CurrentTarget.IsValid())
//	{
//		StopChaseAndReturnToPatrol();
//		return;
//	}
//
//	AActor* Target = CurrentTarget.Get();
//
//	// 리쉬: 너무 멀어지면 포기 후 순찰 복귀
//	const APawn* Me = GetPawn();
//	if (!Me || !GetWorld()) { StopChaseAndReturnToPatrol(); return; }
//
//	const float Dist = FVector::Dist(Me->GetActorLocation(), Target->GetActorLocation());
//	if (MaxChaseLeashDistance > 0.f && Dist > MaxChaseLeashDistance)
//	{
//		StopChaseAndReturnToPatrol();
//		return;
//	}
//
//	// 시야 체크: 보이면 유실 타이머 해제+추격 유지, 안 보이면 LoseSightDelay 타이머(1회) 가동
//	if (ValidateVision(Target))
//	{
//		if (bLoseSightTimerArmed)
//		{
//			bLoseSightTimerArmed = false;
//			GetWorldTimerManager().ClearTimer(LoseSightTimerHandle);
//		}
//
//		// 경로가 멈춰 있거나 Idle이면 재지시
//		if (UPathFollowingComponent* PF = GetPathFollowingComponent())
//		{
//			const auto Status = PF->GetStatus();
//			if (Status != EPathFollowingStatus::Moving)
//			{
//				SafeMoveToActor(Target); // 멈춤 방지용 재지시
//			}
//		}
//		else
//		{
//			SafeMoveToActor(Target);
//		}
//	}
//	else
//	{
//		// 아직 타이머 안 걸렸으면 유실 타이머 1회만 건다
//		if (!bLoseSightTimerArmed)
//		{
//			bLoseSightTimerArmed = true;
//			GetWorldTimerManager().SetTimer(
//				LoseSightTimerHandle, this,
//				&AEnemyAIController::HandleLoseSightTimeout,
//				EnemyChar->LoseSightDelay, false);
//		}
//	}
//}
//
////[4]
//void AEnemyAIController::StartPatrol()
//{
//	UE_LOG(LogTemp, Warning, TEXT("[AI] StartPatrol Called!"));
//
//	if (!EnemyChar)
//	{
//		UE_LOG(LogTemp, Error, TEXT("[AI] StartPatrol FAILED: EnemyChar is null"));
//		return;
//	}
//
//	State = EEnemyState::Patrol;
//	bReturningHome = false;
//	ChaseStartTime = -1.0;
//
//	CurrentTarget.Reset();
//	GetWorldTimerManager().ClearTimer(LoseSightTimerHandle);
//
//	// 수정: 초기 상태/파라미터 로그
//	UE_LOG(LogTemp, Warning, TEXT("[AI] StartPatrol: Mode=%d Start=%s End=%s Center=%s R=%.1f"),
//		(int32)EnemyChar->PatrolMode,
//		*EnemyChar->StartLocation.ToString(), *EnemyChar->EndLocation.ToString(),
//		*EnemyChar->CircleCenter.ToString(), EnemyChar->CircleRadius);
//
//	if (EnemyChar->PatrolMode == EPatrolMode::None)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("[AI] PatrolMode=None -> standing guard"));
//		StopMovement();
//		return;
//	}
//
//	// (선택) Navmesh 투영으로 목적지 유효성 한 번 선검사하고 싶으면 여기서 체크 가능
//
//	UE_LOG(LogTemp, Warning, TEXT("[AI] StartPatrol: Mode=%d"), (int32)EnemyChar->PatrolMode);
//	MoveToNextPatrolPoint();
//}
//
//void AEnemyAIController::PatrolTick()
//{
//}
//
////[5]
//void AEnemyAIController::MoveToNextPatrolPoint()
//{
//	if (!EnemyChar) return;
//
//	FVector Dest;
//	if (ComputeNextPatrolDestination(Dest))
//	{
//		// ★ 현재 위치와 목표 거리 체크 (수용 반경+여유값 이하면 다음 목적지로 전환)
//		const FVector MyLoc = GetPawn() ? GetPawn()->GetActorLocation() : Dest;
//		const float Dist2D = FVector::Dist2D(MyLoc, Dest);
//		const float Accept = EnemyChar->PatrolAcceptanceRadius;
//		if (Dist2D <= Accept + 20.f)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("[AI] Patrol Dest too close (%.1f <= %.1f), skipping"), Dist2D, Accept);
//
//			if (EnemyChar->PatrolMode == EPatrolMode::PingPongPath)
//				EnemyChar->FlipPingPongDirection();
//
//			// 너무 빠른 루프 방지: 한 틱 지연 후 다시 시도
//			FTimerHandle Tmp;
//			GetWorldTimerManager().SetTimer(Tmp, this, &AEnemyAIController::MoveToNextPatrolPoint, 0.05f, false);
//			return;
//		}
//
//		EPathFollowingRequestResult::Type Result =
//			MoveToLocation(Dest, Accept, /*bStopOnOverlap*/ true);
//
//		switch (Result)
//		{
//		case EPathFollowingRequestResult::Failed:
//			UE_LOG(LogTemp, Error, TEXT("[AI] MoveTo FAILED (Patrol) Dest=%s"), *Dest.ToString());
//			{ FTimerHandle Tmp; GetWorldTimerManager().SetTimer(Tmp, this, &AEnemyAIController::MoveToNextPatrolPoint, 0.25f, false); }
//			break;
//
//		case EPathFollowingRequestResult::AlreadyAtGoal:
//			UE_LOG(LogTemp, Warning, TEXT("[AI] AlreadyAtGoal (Patrol)"));
//			if (EnemyChar->PatrolMode == EPatrolMode::PingPongPath)
//				EnemyChar->FlipPingPongDirection();
//			// 즉시 재귀 대신 한 틱 지연
//			{ FTimerHandle Tmp; GetWorldTimerManager().SetTimer(Tmp, this, &AEnemyAIController::MoveToNextPatrolPoint, 0.05f, false); }
//			break;
//
//		case EPathFollowingRequestResult::RequestSuccessful:
//			UE_LOG(LogTemp, Warning, TEXT("[AI] MoveTo OK (Patrol) -> %s"), *Dest.ToString());
//			break;
//		}
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("[AI] ComputeNextPatrolDestination FAILED (Mode=%d)"), (int32)EnemyChar->PatrolMode);
//		FTimerHandle Tmp; GetWorldTimerManager().SetTimer(Tmp, this, &AEnemyAIController::MoveToNextPatrolPoint, 0.25f, false);
//	}
//	//FVector Dest;
//	//if (ComputeNextPatrolDestination(Dest))
//	//{
//	//	MoveToLocation(Dest, EnemyChar->PatrolAcceptanceRadius, /*bStopOnOverlap*/ true);
//	//}
//	//else
//	//{
//	//	StopMovement();
//	//}
//}
//
////[7]
//void AEnemyAIController::StartChase(AActor* Target)
//{
//	//if (!EnemyChar || !IsValid(Target)) return;
//
//	//State = EEnemyState::Chase;
//	//CurrentTarget = Target;
//
//	//// 수정: 유실/추격 틱 타이머 초기화
//	//GetWorldTimerManager().ClearTimer(LoseSightTimerHandle);
//	//bLoseSightTimerArmed = false;
//
//	//// 수정: 추격 틱 시작(주기적으로 재지시/상태 점검)
//	//GetWorldTimerManager().SetTimer(
//	//	ChaseTickTimerHandle, this, &AEnemyAIController::ChaseTick,
//	//	ChaseRepathTickInterval, true, 0.0f);
//
//	//SafeMoveToActor(Target);
//
//
//	if (!EnemyChar || !IsValid(Target)) return;
//
//	State = EEnemyState::Chase;
//	CurrentTarget = Target;
//
//	// 수정: 추격 시작 시각 기록
//	ChaseStartTime = GetWorld() ? GetWorld()->GetTimeSeconds() : -1.0;
//
//	// 기존 타이머 초기화/시작 유지
//	GetWorldTimerManager().ClearTimer(LoseSightTimerHandle);
//	bLoseSightTimerArmed = false;
//
//	GetWorldTimerManager().SetTimer(
//		ChaseTickTimerHandle, this, &AEnemyAIController::ChaseTick,
//		ChaseRepathTickInterval, true, 0.0f);
//
//	SafeMoveToActor(Target);
//
//	//if (!EnemyChar || !Target)
//	//	return;
//
//	//State = EEnemyState::Chase;
//	//CurrentTarget = Target;
//	//GetWorldTimerManager().ClearTimer(LoseSightTimerHandle);
//
//	//MoveToActor(CurrentTarget, EnemyChar->ChaseAcceptanceRadius, true);
//}
//
//void AEnemyAIController::StopChaseAndReturnToPatrol()
//{
//	// 수정: 추격 관련 타이머 정리
//	GetWorldTimerManager().ClearTimer(ChaseTickTimerHandle);
//	GetWorldTimerManager().ClearTimer(LoseSightTimerHandle);
//	bLoseSightTimerArmed = false;
//
//	CurrentTarget.Reset();
//	StartPatrol();
//}
//
//void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
//{
//	if (!EnemyChar || !IsValid(Actor)) return;
//
//	if (Stimulus.WasSuccessfullySensed())
//	{
//		// 수정: 감지 성공 시 유실 타이머 해제
//		bLoseSightTimerArmed = false;
//		GetWorldTimerManager().ClearTimer(LoseSightTimerHandle);
//
//		if (ValidateVision(Actor))
//		{
//			if (State != EEnemyState::Chase || !CurrentTarget.IsValid() || CurrentTarget.Get() != Actor)
//				StartChase(Actor);
//			else
//				SafeMoveToActor(Actor);
//		}
//	}
//	else
//	{
//		// 수정: 같은 타깃 유실 시에만 1회 타이머 건다(중복 방지)
//		if (CurrentTarget.IsValid() && CurrentTarget.Get() == Actor && State == EEnemyState::Chase && !bLoseSightTimerArmed)
//		{
//			bLoseSightTimerArmed = true;
//			GetWorldTimerManager().SetTimer(
//				LoseSightTimerHandle, this,
//				&AEnemyAIController::HandleLoseSightTimeout,
//				EnemyChar->LoseSightDelay, false);
//		}
//	}
//
//	//if (!EnemyChar)
//	//	return;
//
//	//// Perception이 알려준 "후보"를 최종 시야 규칙으로 재검증
//	//if (Stinulus.WasSuccessfullySensed())
//	//{
//	//	if (ValidateVision(Actor))
//	//		StartChase(Actor);
//	//}
//	//else
//	//{
//	//	if (Actor == CurrentTarget)
//	//	{
//	//		GetWorldTimerManager().ClearTimer(LoseSightTimerHandle);
//	//		GetWorldTimerManager().SetTimer(
//	//		LoseSightTimerHandle,
//	//		this, 
//	//		&AEnemyAIController::HandleLoseSightTimeout,
//	//		EnemyChar->LoseSightDelay,
//	//		false
//	//		);
//	//	}
//	//}
//}
//
//bool AEnemyAIController::ValidateVision(AActor* Actor) const
//{
//	if (!IsValid(Actor)) return false;
//
//	UWorld* W = GetWorld();
//	if (!W) 
//		return false;
//	
//	const APawn* P = GetPawn();
//	if (!P) 
//		return false;
//
//	// 1. 거리
//	FVector EyesLoc, EyesDir;
//	GetEyes(EyesLoc, EyesDir);
//	const FVector ToTarget = Actor->GetActorLocation() - EyesLoc;
//	const float DistSq = ToTarget.SizeSquared();
//	if (DistSq > FMath::Square(SenseMaxDistance))
//		return false;
//
//	//2. 각도
//	const FVector DirToTarget = ToTarget.GetSafeNormal();
//	const float Dot = FVector::DotProduct(EyesDir, DirToTarget);
//	const float CosHalfFov = FMath::Cos(FMath::DegreesToRadians(HalfFOVDegrees));
//	if (Dot < CosHalfFov)
//		return false;
//
//	// 3. 가시선(LineTrace) 검사
//	UWorld* World = GetWorld();
//	if (!World)
//		return false;
//
//	FCollisionQueryParams Params(SCENE_QUERY_STAT(AI_LOS), /*bTraceComplex*/ false);
//	//Params.AddIgnoredActor(GetPawn());
//	 Params.AddIgnoredActor(P);  
//	//Params.AddIgnoredActor(Actor);
//
//
//	// 3-a. 몸통으로 확인 -> 완전 가림 여부
//	FHitResult Hit;
//	const FVector BodyTarget = Actor->GetActorLocation() + FVector(0, 0, 40.0f);
//	const bool bBlockedBody = World->LineTraceSingleByChannel(Hit, EyesLoc, BodyTarget, VisibilityChannel, Params);
//
//	if (!bBlockedBody)
//		return true;
//
//	// 3-b. 머리로 재시도
//	FVector Head;
//	GetTargetHeadLocation(Actor, Head);
//
//	FHitResult HitHead;
//	const bool bBlockedHead = World->LineTraceSingleByChannel(Hit, EyesLoc, Head, VisibilityChannel, Params);
//
//	if (!bBlockedHead)
//		return true;
//
//	return false;
//}
//
//
//// ====== 시야 계산 ======
//void AEnemyAIController::GetEyes(FVector& OutLoc, FVector& OutDir) const
//{
//	OutLoc = FVector::ZeroVector;
//	OutDir = FVector::ForwardVector;
//
//	const APawn* p = GetPawn();
//	if (!p)
//		return;
//
//	FVector EyesLoc = p->GetActorLocation() + FVector(0, 0, 70.0f);
//	FRotator EyesRot = GetControlRotation();
//	OutLoc = EyesLoc;
//	OutDir = EyesRot.Vector();
//}
//
//bool AEnemyAIController::GetTargetHeadLocation(AActor* Target, FVector& OutHead) const
//{
//	if (const ACharacter* Char = Cast<ACharacter>(Target))
//	{
//		if (const USkeletalMeshComponent* Mesh = Char->GetMesh())
//		{
//			if (Mesh->DoesSocketExist(TEXT("head")))
//			{
//				OutHead = Mesh->GetSocketLocation(TEXT("head"));
//				return true;
//			}
//		}
//		FVector EyesLoc; FRotator EyesRot;
//		Char->GetActorEyesViewPoint(EyesLoc, EyesRot);
//		OutHead = EyesLoc;
//		return true;
//	}
//
//	OutHead = Target->GetActorLocation() + FVector(0, 0, 60.f);
//	return true;
//}
//
////[4]
//bool AEnemyAIController::ComputeNextPatrolDestination(FVector& OutDest) const
//{
//	if (!EnemyChar) return false;
//
//	bool bOk = false;
//	switch (EnemyChar->PatrolMode)
//	{
//	case EPatrolMode::PingPongPath:
//		bOk = EnemyChar->GetNextPingPongDestination(OutDest);
//		// 수정
//		if (!bOk) UE_LOG(LogTemp, Warning, TEXT("[AI] PingPong: GetNextPingPongDestination failed"));
//		break;
//
//	case EPatrolMode::RandomInCircle:
//		bOk = EnemyChar->GetRandomCircleDestination(OutDest);
//		// 수정
//		if (!bOk) UE_LOG(LogTemp, Warning, TEXT("[AI] RandomInCircle: GetRandomCircleDestination failed (Center=%s, R=%.1f)"),
//			*EnemyChar->CircleCenter.ToString(), EnemyChar->CircleRadius);
//		break;
//
//	case EPatrolMode::None:
//	default:
//		// 수정
//		UE_LOG(LogTemp, Warning, TEXT("[AI] PatrolMode=None (no patrol)."));
//		bOk = false;
//		break;
//	}
//
//	if (bOk)
//	{
//		if (!FMath::IsFinite(OutDest.X) || !FMath::IsFinite(OutDest.Y) || !FMath::IsFinite(OutDest.Z))
//		{
//			// 수정
//			UE_LOG(LogTemp, Error, TEXT("[AI] OutDest not finite: %s"), *OutDest.ToString());
//			return false;
//		}
//	}
//	return bOk;
//
//	/*switch (EnemyChar->PatrolMode)
//	{
//	case EPatrolMode::PingPongPath:
//		return EnemyChar->GetNextPingPongDestination(OutDest);
//	case EPatrolMode::RandomInCircle:
//		return EnemyChar->GetRandomCircleDestination(OutDest);
//	case EPatrolMode::None:
//	default:
//		return false;
//	}*/
//}
//
//void AEnemyAIController::HandleLoseSightTimeout()
//{
//	//if (State != EEnemyState::Chase) { bLoseSightTimerArmed = false; return; }
//
//	//if (!CurrentTarget.IsValid() || !ValidateVision(CurrentTarget.Get()))
//	//{
//	//	bLoseSightTimerArmed = false;
//	//	StopChaseAndReturnToPatrol();
//	//}
//	//else
//	//{
//	//	// 수정: 아직 보이면 추격 유지(유실 타이머 해제)
//	//	bLoseSightTimerArmed = false;
//	//	if (CurrentTarget.IsValid())
//	//		SafeMoveToActor(CurrentTarget.Get());
//	//}
//
//	if (State != EEnemyState::Chase) { bLoseSightTimerArmed = false; return; }
//
//	if (!CurrentTarget.IsValid() || !ValidateVision(CurrentTarget.Get()))
//	{
//		bLoseSightTimerArmed = false;
//		// 기존: StopChaseAndReturnToPatrol();
//		// 수정: 홈으로 귀환 → 그 다음 순찰
//		StartReturnHome();
//	}
//	else
//	{
//		bLoseSightTimerArmed = false;
//		if (CurrentTarget.IsValid())
//			SafeMoveToActor(CurrentTarget.Get());
//	}
//
//	// 여전히 유효 시야 아니면 순찰 복귀
//	/*if (State == EEnemyState::Chase && CurrentTarget && !ValidateVision(CurrentTarget))
//	{
//		StopChaseAndReturnToPatrol();
//	}*/
//}
//
////[2]
//void AEnemyAIController::SetupSight(float SightRedius, float LoseSightRadius, float PeripheralVisionAngleDegrees)
//{
//	SightConfig->SightRadius = SightRedius;
//	SightConfig->LoseSightRadius = LoseSightRadius;
//	SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngleDegrees;
//	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
//	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
//	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
//}
//
//void AEnemyAIController::SafeMoveToActor(AActor* Target)
//{
//	// 재진입/무효 가드
//	if (bRepathInProgress) return;
//	TGuardValue<bool> Guard(bRepathInProgress, true);
//
//	UWorld* W = GetWorld();
//	if (!EnemyChar || !IsValid(Target) || !W) return;
//
//	// 컨트롤러/경로팔로윙 유효성
//	UPathFollowingComponent* PF = GetPathFollowingComponent();
//	if (!PF) return;
//
//	// 최소 재지시 간격
//	const double Now = W->GetTimeSeconds();
//	if (LastRepathTime > 0.0 && (Now - LastRepathTime) < RepathTimeInterval) return;
//
//	// 타깃 머리 위치(혹은 눈)
//	FVector HeadLoc;
//	if (!GetTargetHeadLocation(Target, HeadLoc)) return;
//
//	// 타깃이나 목표가 크게 변했을 때만 재지시
//	const bool bTargetChanged = (!LastRepathTarget.IsValid() || LastRepathTarget.Get() != Target);
//	if (!bTargetChanged && !LastRepathGoal.IsNearlyZero() &&
//		FVector::DistSquared(LastRepathGoal, HeadLoc) < FMath::Square(RepathDistanceThreshold))
//	{
//		return;
//	}
//
//	// 네비 투영 + 도달 가능 여부
//	FVector NavGoal = HeadLoc;
//	if (!ProjectToNavmesh(HeadLoc, NavGoal)) return;
//
//	const FVector From = GetPawn() ? GetPawn()->GetActorLocation() : NavGoal;
//	if (!IsLocationReachable(From, NavGoal)) return;
//
//	// 기존 이동이 "진짜 이동 중"일 때만 중단
//	if (PF->GetStatus() == EPathFollowingStatus::Moving)
//	{
//		PF->AbortMove(*this, FPathFollowingResultFlags::ForcedScript);
//	}
//
//	// 부분 경로 허용: 급선회/높이차에서 끊김 완화
//	FAIMoveRequest Req;
//	Req.SetGoalActor(Target);
//	Req.SetAcceptanceRadius(EnemyChar->ChaseAcceptanceRadius);
//	Req.SetUsePathfinding(true);
//	Req.SetAllowPartialPath(true);
//	Req.SetCanStrafe(false);
//
//	FNavPathSharedPtr OutPath;
//	MoveTo(Req, &OutPath);
//
//	// 상태 갱신
//	LastRepathTime = Now;
//	LastRepathGoal = HeadLoc;
//	LastRepathTarget = Target;
//}
//
//bool AEnemyAIController::IsLocationReachable(const FVector& From, const FVector& To) const
//{
//	UWorld* W = GetWorld();
//	if (!W) return false;
//	if (UNavigationSystemV1* NS = UNavigationSystemV1::GetCurrent(W))
//	{
//		if (UNavigationPath* Path = NS->FindPathToLocationSynchronously(W, From, To))
//		{
//			return Path->IsValid() && Path->PathPoints.Num() > 1;
//		}
//	}
//	return false;
//}
//
//bool AEnemyAIController::ProjectToNavmesh(const FVector& In, FVector& Out) const
//{
//	UWorld* W = GetWorld();
//	if (!W) return false;
//	if (UNavigationSystemV1* NS = UNavigationSystemV1::GetCurrent(W))
//	{
//		FNavLocation L;
//		if (NS->ProjectPointToNavigation(In, L))
//		{
//			Out = L.Location;
//			return true;
//		}
//	}
//	return false;
//}
