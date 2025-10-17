// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CHRPlayer.h"
#include "WKnife.generated.h"

UCLASS()
class UESNOWWORLD_API AWKnife : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWKnife();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ACHRPlayer* KnifeHolder = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | State")
	float ATKDmg = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon | Knife")
	float Strength = 10;



protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* staticMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION()
	void OnWeaponBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	bool IsWithinAttackRange(float AttackRange, AActor* target);

};
