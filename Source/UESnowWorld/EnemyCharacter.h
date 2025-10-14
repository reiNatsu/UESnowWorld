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
//	// pingpongpath 시작점, 끝점. 
//	UPROPERTY(EditAnywhere, Category = "AI|Patrol|PingPong")
//	FVector StartLocation = FVector::ZeroVector;
//
//	UPROPERTY(EditAnywhere, Category = "AI|Patrol|PingPong")
//	FVector EndLocation = FVector(1200.0f, 0.0f, 0.0f);
//
//	// randomincircle 원 영역 중심, 반지름
//	UPROPERTY(EditAnywhere, Category = "AI|Patrol|RandomCircle")
//	FVector CircleCenter = FVector::ZeroVector;
//
//	UPROPERTY(EditAnywhere, Category = "AI|Patrol|RandomCircle", meta = (ClampMin = "100.0", UIMin = "100.0"))
//	float CircleRadius = 800.0f;
//
//	// 수정: 귀환 위치(스폰/대기 위치) 저장
//	UPROPERTY(VisibleInstanceOnly, Category = "AI|Home")
//	FVector HomeLocation = FVector::ZeroVector;
//
//	// 공통 변수 : 허용 반경
//	UPROPERTY(EditAnywhere, Category = "AI|Patrol")
//	float PatrolAcceptanceRadius = 80.0f;
//
//	// 추격 허용 반경
//	UPROPERTY(EditAnywhere, Category = "AI|Chase")
//	float ChaseAcceptanceRadius = 120.0f;
//
//	// 타깃을 잃은 뒤 순찰 복귀까지의 지연
//	UPROPERTY(EditAnywhere, Category = "AI|Chase")
//	float LoseSightDelay = 1.0f;
//
//	// 현재 pingpong 방향 true -> end 쪽, false -> start 쪽
//	UPROPERTY(VisibleInstanceOnly, Category = "AI|Patrol")
//	bool bTowardEnd = true;
//
//	// 현재 목적지(컨트롤러가 조회)
//	bool GetNextPingPongDestination(FVector& out) const;
//	bool GetRandomCircleDestination(FVector& out) const;
//
//	// pingpong 도착 시 방향 전환
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
