// Fill out your copyright notice in the Description page of Project Settings.


#include "FSM.h"
#include <Kismet/KismetSystemLibrary.h>

// Sets default values for this component's properties
UFSM::UFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFSM::BeginPlay()
{
	Super::BeginPlay();

	// ...
	_owner = GetOwner();
	_currentState = AIState::AIState_Idle;
}


// Called every frame
void UFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
	// ...
	FSM(DeltaTime);
}

void UFSM::FSM(float dt)
{
	if (_owner.IsValid() == false)
		return;

	switch (_currentState)
	{
	case AIState::AIState_Idle:
		Idel(dt);
		break;
	case AIState::AIState_Patrol:
		Patrol(dt);
		break;
	case AIState::AIState_Chase:
		Chase(dt);
		break;
	case AIState::AIState_Attack:
		Attack(dt);
		break;
	default:
		break;
	}
}

static FName PLAYER = FName("Player");
void UFSM::Idel(float dt)
{
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	TArray<AActor*> IgnoreActors;
	TArray<AActor*> OutActors;

	// 1.�÷��̾ �� �þ߿� ���� �ϴ��� ã�ƺ��� ���󰣴�. 
	// ��ó�� �������ؼ�
	
	auto position = _owner->GetActorLocation();
	if (UKismetSystemLibrary::SphereOverlapActors(GetWorld(), position, 500, objectTypes, nullptr, IgnoreActors, OutActors))
	{
		for (auto actor : OutActors)
		{
			if (actor->ActorHasTag(PLAYER))
			{
				_target = actor;
				_currentState = AIState::AIState_Chase;

				return;
			}
		}
	}
	
	//2. �÷��̾ �� �þ߳��� ���ٸ� ��Ʈ�� �Ѵ�
	_currentDirection = FMath::VRand();
	_currentDirection.Z = 0;
	_destination = position + _currentDirection * FMath::RandRange(100, 500);  // 1M~ 5M ������ ���� ��ġ�� �̵�

	_currentState = AIState::AIState_Patrol;
}

void UFSM::Patrol(float dt)
{
	// ������ ��ġ�� �̵��Ѵ�
	// �������� �����ϸ� ����
	auto position = _owner->GetActorLocation();
	if (FVector::DistSquared(_destination, position) < 500)
	{
		//100cm ���� ����� ���� Idle ���·�
		_currentState = AIState::AIState_Idle;
	}

	_owner->SetActorLocation(position + _currentDirection * _speed * dt);
}

void UFSM::Chase(float dt)
{
	// Ÿ�� ��ó ��ġ�� �̵��Ѵ�.
	if (_target.IsValid() == false)
	{
		_currentState = AIState::AIState_Idle;
		return;
	}

	auto position = _owner->GetActorLocation();
	auto targetPos = _target->GetActorLocation();
	if (FVector::DistSquared(targetPos, position) < 500)
	{
		_currentState = AIState::AIState_Attack;
	}
	_currentDirection = (targetPos - position).GetSafeNormal();
	_owner->SetActorLocation(position + _currentDirection * _speed * dt);
}

void UFSM::Attack(float dt)
{
	// Ÿ���� �����Ѵ�. 
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Purple, TEXT("IsAttack Player!!!!"));

	_currentState = AIState::AIState_Idle;
}

