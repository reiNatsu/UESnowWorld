// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CHREnemyAIController.generated.h"


class UBehaviorTree;
class UBlackboardComponent;


/** Àû AIController: BehaviorTree ½ÇÇà */

UCLASS()
class UESNOWWORLD_API ACHREnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
    ACHREnemyAIController();

protected:
    virtual void OnPossess(APawn* InPawn) override;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    UBehaviorTree* BehaviorTreeAsset = nullptr;

    UPROPERTY(Transient)
    UBlackboardComponent* BB = nullptr;
};