// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyTest.generated.h"

UCLASS()
class UESNOWWORLD_API AEnemyTest : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyTest();

	UPROPERTY(EditAnywhere, Category = "Enemy | Params")
	int HealthPoints = 100;

	UPROPERTY(EditAnywhere, Category = "Enemy | Params")
	float Strength = 10;

	UPROPERTY(EditAnywhere, Category = "Enemy | Params")
	float Armor = 1;

	UPROPERTY(EditAnywhere, Category = "Enemy | Params")
	float AttackRange = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Enemy | Params")
	float AttackInterval = 3.0f;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
