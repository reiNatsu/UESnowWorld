// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"

#include "CHRBossAIController.generated.h"

/**
 AIPerception(Sight) ���� �þ� ���� + Reachable ���� �ι� + Ÿ�� ���� MoveToActor.
 */


class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UNavigationSystemV1;

UCLASS()
class UESNOWWORLD_API ACHRBossAIController : public AAIController
{
	GENERATED_BODY()

public:

	ACHRBossAIController();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;

	// Ÿ��
	UFUNCTION(BlueprintCallable)
	AActor* GetCurrentTarget() const { return CurrentTarget; }
	void ClearTarget();

	// ���� �þ� ����(���� ������ �ݿ�����)
	void ConfigureSight(float SightRadius, float LoseSightRadius, float PeripheralVisionAngleDegrees);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAIPerceptionComponent* PerceptionComp;
	
	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;
	
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);


	// �ι�
	void UpdateRoam(float DeltaSeconds);
	void PickNewRoamDestination();

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Roam")
	float RoamRadius = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Boss|Roam")
	float RepathInterval = 2.5f;

	float RoamTimer = 0.f;
	FVector CurrentRoamPoint = FVector::ZeroVector;

private:
	UPROPERTY()
	AActor* CurrentTarget = nullptr;
};
