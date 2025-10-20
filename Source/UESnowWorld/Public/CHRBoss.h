// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "BossTypes.h"
#include "CHRBoss.generated.h"

//
class ACHRBossAIController;
class UDataTable;
class UAnimMontage;


/**
 * CHRBoss
 * - ���� ��ü. ����/�̵�/���´� ����.
 * - ���/�������� StageController�� ��� �� ������ ���� ��ȣ�� ����.
 */

UENUM(BlueprintType)
enum class EBossState : uint8
{
	Roam,
	Chase,
	Attack,
	Skill1,
	Skill2,
	Sabotage,
	Stunned,
	Dead
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossDied); // StageController���� ��ȣ

UCLASS()
class UESNOWWORLD_API ACHRBoss : public ABaseCharacter
{
	GENERATED_BODY()
	 

public:
	ACHRBoss();

	virtual void BeginPlay() override;

	
};
