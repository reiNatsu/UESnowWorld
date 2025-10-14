// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


 // ====== �� Ÿ�� Enum ======
 // - None   : ���� ���� �� �÷��̾ �þ߿� ������ �ٷ� �߰� + ����
 // - Patrol : ���� ���� �� �þ߿� �÷��̾� ������ �߰� + ����
 //
//UENUM(BlueprintType)
//enum class EEnemyType :uint8
//{
//	None	UMETA(DisplayName="None"),
//	Patrol	UMETA(DisplayName="Patrol")
//};

// ====== ������ Ű ���ӽ����̽� ======
// - BehaviorTree & Blackboard Ű �̸��� �Ѱ����� ���� ����
//
namespace EnemyBB
{
	// �÷��̾� Actor (Object)
	static const FName TargetActor = TEXT("TargetActor");

	// ���� ���� ��ġ (Vector)
	static const FName TargetPosition = TEXT("TargetPosition");

	// �÷��̾� �þ� �ν� ���� (Bool)
	static const FName IsInSight = TEXT("IsInSight");

	// �ڱ� �ڽ� Actor(�ɼ�)
	static const FName SelfActor = TEXT("SelfActor");

	// ���� ���� ������ ����(Bool)
	static const FName InAttackRange = TEXT("InAttackRange");

	// EnemyType(Enum)
	static const FName DoPatrol = TEXT("DoPatrol");        // Bool  �� �ٽ�

	static const FName PatrolMode = TEXT("PatrolMode");    // Byte(Int): 0=None, 1=Patrol, 2=LinePatrol
	static const FName PatrolTarget = TEXT("PatrolTarget");  // Vector: MoveTo Ÿ��

	static const FName HomeLocation = TEXT("HomeLocation");   // Vector
	static const FName AtHome = TEXT("AtHome");         // Bool
	static const FName LastSeenTime = TEXT("LastSeenTime");   // Float
	static const FName ShouldChase = TEXT("ShouldChase");    // Bool
	static const FName NeedReturnHome = TEXT("NeedReturnHome");

}