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

	// ���� ������(ó�� �������� �÷��̾ ���)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon|Owner")
	ACharacter* WeaponHolder = nullptr;

	// ���ݷ� �� �Ķ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Config")
	float Strength = 10.f;

	// ���� ���� �̸�(�⺻: ���� �ڵ� ȣȯ�� ���� KnifeSocket ����)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Config")
	FName AttachSocketName = TEXT("SwordSocket");

protected:

	// ���̷�Ż �޽�(��Ʈ)
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

	// ��Ÿ� üũ
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool IsWithinAttackRange(float AttackRange, AActor* Target) const;

};
