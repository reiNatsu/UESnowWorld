// Fill out your copyright notice in the Description page of Project Settings.


#include "SwordWeaponBase.h"
#include "CHRPlayer.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

// Sets default values
ASwordWeaponBase::ASwordWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SetRootComponent(SkeletalMesh);

	SkeletalMesh->SetGenerateOverlapEvents(true);

	// ������ �浹 ����
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SkeletalMesh->SetCollisionObjectType(ECC_WorldDynamic);
	SkeletalMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	SkeletalMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

// Called when the game starts or when spawned
void ASwordWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	//OnActorBeginOverlap.AddDynamic(this, &ASwordWeaponBase::OnWeaponBeginOverlap);
	SkeletalMesh->OnComponentBeginOverlap.AddDynamic(this, &ASwordWeaponBase::OnWeaponBeginOverlap);
}

// Called every frame
void ASwordWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASwordWeaponBase::OnWeaponBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// �ڱ� �ڽ� ����
	if (OtherActor == this || OtherActor == nullptr)
	{
		return;
	}

	// ĳ���Ϳ͸� ��ȣ�ۿ�
	ACharacter* Character = Cast<ACharacter>(OtherActor);
	
	if (Character == nullptr)
	{
		return;
	}

	// ���� Ȧ���� �������� �ʾҴٸ�, �÷��̾�(=CHRPlayer)���� Ȯ�� �� ����
	if (WeaponHolder == nullptr)
	{
		ACHRPlayer* Player = Cast<ACHRPlayer>(Character);
	
		if (Player != nullptr)
		{
			WeaponHolder = Player;

			// �÷��̾��� Mesh ���Ͽ� ����
			AttachToComponent(
				Player->GetMesh(),
				FAttachmentTransformRules::SnapToTargetIncludingScale,
				AttachSocketName
			);
		}
	}
}

//void ASwordWeaponBase::OnWeaponBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
//{
//	
//}

bool ASwordWeaponBase::IsWithinAttackRange(float AttackRange, AActor* Target) const
{
	if (Target == nullptr)
	{
		return false;
	}

	return (AttackRange <= 0.0f) ||
		(FVector::Distance(Target->GetActorLocation(), GetActorLocation()) <= AttackRange);
}

