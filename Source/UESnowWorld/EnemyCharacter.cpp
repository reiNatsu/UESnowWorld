//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "EnemyCharacter.h"
//#include "GameFramework/CharacterMovementComponent.h"
//#include "NavigationSystem.h"
//
//// Sets default values
//AEnemyCharacter::AEnemyCharacter()
//{
// 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
//	PrimaryActorTick.bCanEverTick = true;
//
//	
//
//	bUseControllerRotationYaw = false;
//	GetCharacterMovement()->bOrientRotationToMovement = true;
//	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
//}
//
//// Called when the game starts or when spawned
//void AEnemyCharacter::BeginPlay()
//{
//	Super::BeginPlay();
//	
//	HomeLocation = GetActorLocation();
//
//	// 기본값 보정 : 원 순찰은 중심 미지정이면 스폰 위치를 중심으로 사용
//	if (PatrolMode == EPatrolMode::RandomInCircle && CircleCenter.IsNearlyZero())
//	{
//		CircleCenter = GetActorLocation();  // 수정
//	}
//	if (PatrolMode == EPatrolMode::PingPongPath && StartLocation.IsNearlyZero())
//	{
//		StartLocation = GetActorLocation(); // 수정
//	}
//}
//
//bool AEnemyCharacter::GetNextPingPongDestination(FVector& out) const
//{
//	if (PatrolMode != EPatrolMode::PingPongPath)
//		return false;
//	out = bBlockInput ? EndLocation : StartLocation;
//	return false;
//}
//
//bool AEnemyCharacter::GetRandomCircleDestination(FVector& out) const
//{
//	if (PatrolMode != EPatrolMode::RandomInCircle)
//		return false;
//
//	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
//	if (!NavSys)
//		return false;
//
//	FNavLocation Result;
//	const bool bOk = NavSys->GetRandomReachablePointInRadius(CircleCenter, CircleRadius, Result);
//	if (bOk)
//	{
//		out = Result.Location;
//		return true;
//	}
//
//	return false;
//}
//
//void AEnemyCharacter::FlipPingPongDirection()
//{
//	bTowardEnd = !bTowardEnd;
//}
//
//// Called every frame
//void AEnemyCharacter::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}
//
//// Called to bind functionality to input
//void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}
//
