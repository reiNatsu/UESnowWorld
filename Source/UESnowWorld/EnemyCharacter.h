//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Character.h"
//#include "EnemyCharacter.generated.h"
//
//
//
//UCLASS()
//class UESNOWWORLD_API AEnemyCharacter : public ACharacter
//{
//	GENERATED_BODY()
//
//public:
//	// Sets default values for this character's properties
//	AEnemyCharacter();
//
//protected:
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;
//
//public:
//	
//
//
//	// pingpongpath ������, ����. 
//	UPROPERTY(EditAnywhere, Category = "AI|Patrol|PingPong")
//	FVector StartLocation = FVector::ZeroVector;
//
//	UPROPERTY(EditAnywhere, Category = "AI|Patrol|PingPong")
//	FVector EndLocation = FVector(1200.0f, 0.0f, 0.0f);
//
//	// randomincircle �� ���� �߽�, ������
//	UPROPERTY(EditAnywhere, Category = "AI|Patrol|RandomCircle")
//	FVector CircleCenter = FVector::ZeroVector;
//
//	UPROPERTY(EditAnywhere, Category = "AI|Patrol|RandomCircle", meta = (ClampMin = "100.0", UIMin = "100.0"))
//	float CircleRadius = 800.0f;
//
//	// ����: ��ȯ ��ġ(����/��� ��ġ) ����
//	UPROPERTY(VisibleInstanceOnly, Category = "AI|Home")
//	FVector HomeLocation = FVector::ZeroVector;
//
//	// ���� ���� : ��� �ݰ�
//	UPROPERTY(EditAnywhere, Category = "AI|Patrol")
//	float PatrolAcceptanceRadius = 80.0f;
//
//	// �߰� ��� �ݰ�
//	UPROPERTY(EditAnywhere, Category = "AI|Chase")
//	float ChaseAcceptanceRadius = 120.0f;
//
//	// Ÿ���� ���� �� ���� ���ͱ����� ����
//	UPROPERTY(EditAnywhere, Category = "AI|Chase")
//	float LoseSightDelay = 1.0f;
//
//	// ���� pingpong ���� true -> end ��, false -> start ��
//	UPROPERTY(VisibleInstanceOnly, Category = "AI|Patrol")
//	bool bTowardEnd = true;
//
//	// ���� ������(��Ʈ�ѷ��� ��ȸ)
//	bool GetNextPingPongDestination(FVector& out) const;
//	bool GetRandomCircleDestination(FVector& out) const;
//
//	// pingpong ���� �� ���� ��ȯ
//	void FlipPingPongDirection();
//
//public:	
//	// Called every frame
//	virtual void Tick(float DeltaTime) override;
//
//	// Called to bind functionality to input
//	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
//
//};
