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
		// �÷��̾ null�� �ƴϸ�
		if (playerAvatar != nullptr)
		{
			KnifeHolder = playerAvatar;

			AttachToComponent(KnifeHolder->GetMesh(),
				FAttachmentTransformRules::SnapToTargetIncludingScale,
				FName("SwordSocket"));

			//�÷��̾�� "���� ���� ����" ���� �˸�
			KnifeHolder->SetEquippedWeaponActor(this);

			// (����) �տ� �� ���� �浹 ���� �� ������� ����
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

