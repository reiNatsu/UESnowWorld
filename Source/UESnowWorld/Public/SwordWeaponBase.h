// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwordWeaponBase.generated.h"

UCLASS()
class UESNOWWORLD_API ASwordWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASwordWeaponBase();

	// 무기 소유자(처음 오버랩된 플레이어를 기억)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon|Owner")
	ACharacter* WeaponHolder = nullptr;

	// 공격력 등 파라미터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Config")
	float Strength = 10.f;

	// 붙일 소켓 이름(기본: 기존 코드 호환을 위해 KnifeSocket 유지)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Config")
	FName AttachSocketName = TEXT("SwordSocket");

protected:

	// 스켈레탈 메시(루트)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Mesh")
	USkeletalMeshComponent* SkeletalMesh;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnWeaponBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	//void OnWeaponBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	// 사거리 체크
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool IsWithinAttackRange(float AttackRange, AActor* Target) const;

};
