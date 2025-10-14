// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


 // ====== 적 타입 Enum ======
 // - None   : 순찰 없음 → 플레이어가 시야에 들어오면 바로 추격 + 공격
 // - Patrol : 랜덤 순찰 → 시야에 플레이어 있으면 추격 + 공격
 //
//UENUM(BlueprintType)
//enum class EEnemyType :uint8
//{
//	None	UMETA(DisplayName="None"),
//	Patrol	UMETA(DisplayName="Patrol")
//};

// ====== 블랙보드 키 네임스페이스 ======
// - BehaviorTree & Blackboard 키 이름을 한곳에서 통일 관리
//
namespace EnemyBB
{
	// 플레이어 Actor (Object)
	static const FName TargetActor = TEXT("TargetActor");

	// 랜덤 순찰 위치 (Vector)
	static const FName TargetPosition = TEXT("TargetPosition");

	// 플레이어 시야 인식 여부 (Bool)
	static const FName IsInSight = TEXT("IsInSight");

	// 자기 자신 Actor(옵션)
	static const FName SelfActor = TEXT("SelfActor");

	// 공격 범위 안인지 여부(Bool)
	static const FName InAttackRange = TEXT("InAttackRange");

	// EnemyType(Enum)
	static const FName DoPatrol = TEXT("DoPatrol");        // Bool  ← 핵심

	static const FName PatrolMode = TEXT("PatrolMode");    // Byte(Int): 0=None, 1=Patrol, 2=LinePatrol
	static const FName PatrolTarget = TEXT("PatrolTarget");  // Vector: MoveTo 타겟

	static const FName HomeLocation = TEXT("HomeLocation");   // Vector
	static const FName AtHome = TEXT("AtHome");         // Bool
	static const FName LastSeenTime = TEXT("LastSeenTime");   // Float
	static const FName ShouldChase = TEXT("ShouldChase");    // Bool
	static const FName NeedReturnHome = TEXT("NeedReturnHome");

}