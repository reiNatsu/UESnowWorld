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
 * - 보스 본체. 전투/이동/상태는 유지.
 * - 사망/리스폰은 StageController가 담당 → 보스는 죽음 신호만 보냄.
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossDied); // StageController에게 신호

UCLASS()
class UESNOWWORLD_API ACHRBoss : public ABaseCharacter
{
	GENERATED_BODY()
	 

public:
	ACHRBoss();

	virtual void BeginPlay() override;

	
};
