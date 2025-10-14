// Fill out your copyright notice in the Description page of Project Settings.


#include "WKnife.h"
#include "CHRPlayer.h"

// Sets default values
AWKnife::AWKnife()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	staticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	SetRootComponent(staticMesh);
}

// Called when the game starts or when spawned
void AWKnife::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &AWKnife::OnWeaponBeginOverlap);
	
}

// Called every frame
void AWKnife::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWKnife::OnWeaponBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	auto character = Cast<ACharacter>(OtherActor);
	if (character == nullptr)
		return;

	if (KnifeHolder == nullptr)
	{
		auto playerAvatar = Cast<ACHRPlayer>(character);
		// 플레이어가 null이 아니면
		if (playerAvatar != nullptr)
		{
			KnifeHolder = playerAvatar;

			AttachToComponent(KnifeHolder->GetMesh(),
				FAttachmentTransformRules::SnapToTargetIncludingScale,
				FName("SwordSocket"));

			//플레이어에게 "현재 장착 무기" 세팅 알림
			KnifeHolder->SetEquippedWeaponActor(this);

			// (선택) 손에 든 동안 충돌 끄기 → 재오버랩 방지
			if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(GetRootComponent()))
			{
				Prim->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}

	}
}

bool AWKnife::IsWithinAttackRange(float AttackRange, AActor* target)
{
	bool result = AttackRange <= 0.0f || FVector::Distance(target->GetActorLocation(),
		GetActorLocation()) <= AttackRange;

	return result;
}

