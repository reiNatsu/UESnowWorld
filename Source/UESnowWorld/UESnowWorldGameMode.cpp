// Copyright Epic Games, Inc. All Rights Reserved.

#include "UESnowWorldGameMode.h"
#include "UESnowWorldCharacter.h"
#include "UObject/ConstructorHelpers.h"

#include "MissionManager.h"    //  �Ŵ��� ����
#include "MissionData.h"       //  FMissionData ����
#include "EngineUtils.h"       //  TActorIterator
#include "Engine/World.h"

AUESnowWorldGameMode::AUESnowWorldGameMode()
{
	// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Player/BP_Visitor_M"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// (����) MissionManagerClass �⺻���� CDO���� �����ϰ� �ʹٸ� ���⼭�� ����
   // static ConstructorHelpers::FClassFinder<AMissionManager> MMClass(TEXT("/Game/Blueprints/BP_MissionManager")); 
   // if (MMClass.Succeeded()) { MissionManagerClass = MMClass.Class; }
}

void AUESnowWorldGameMode::BeginPlay()
{
	Super::BeginPlay();

	// GameMode�� ���� ���ѿ����� ���� -> �����ϰ� ���� üũ
	if (!HasAuthority())
		return;

	// 1) ������ �̹� ��ġ�� MissionManagerã��
	for (TActorIterator<AMissionManager> It(GetWorld()); It; ++It)
	{
		MissionManager = *It;
		break;
	}

	// 2) ������ ���� (���ϸ� MissionManagerClass ���)
	if (!MissionManager)
	{
		// Ŭ���� �������̸� �⺻ Ŭ������ ����
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		MissionManager = GetWorld()->SpawnActor<AMissionManager>(AMissionManager::StaticClass(), 
											FVector::ZeroVector, FRotator::ZeroRotator, Params);
		// ���� BP Ŭ������ �����ϰ� ������ �� Ŭ������ MissionManagerClass�� ��ü
		// if (MissionManagerClass) { MissionManager = GetWorld()->SpawnActor<AMissionManager>(MissionManagerClass, ...); }
	}

	// 3) �ʱ�ȭ (DT_MissionData���� ���� �ִ� 5�� ����)
	if (MissionManager && MissionManager->MissionDataTable)
	{
		// 3-1) ��� �� ��������
		TArray<FMissionData*> AllRows;
		MissionManager->MissionDataTable->GetAllRows<FMissionData>(TEXT("PickMissions"), AllRows);

		if (AllRows.Num() > 0)
		{
			// 3-2) �ε��� �迭 �����
			TArray<int32> Indexes;
			Indexes.Reserve(AllRows.Num());
			for (int32 i = 0; i < AllRows.Num(); ++i)
			{
				Indexes.Add(i);
			}

			// 3-2-5) Fisher-Yates ����
			const uint32 Seed = static_cast<uint32>(FDateTime::Now().GetTicks() & 0xFFFFFFFF);
			FRandomStream Rng(Seed);
			for (int32 i = Indexes.Num() - 1; i > 0; --i)
			{
				const int32 j = Rng.RandRange(0, i);
				if (i != j) { Indexes.Swap(i, j); }
			}

			// 3-3) �ִ� 5�� ��
			const int32 PickCount = FMath::Min(5, AllRows.Num());
			TArray<int32> StageMissions;
			StageMissions.Reserve(PickCount);

			for (int32 k = 0; k < PickCount; ++k)
			{
				if (const FMissionData* Row = AllRows[Indexes[k]])
				{
					StageMissions.Add(Row->MissionID);
				}
			}

			// 3-4) ���õ� �̼����� �ʱ�ȭ
			MissionManager->InitializeMissions(StageMissions);
		}
	}
	//if (MissionManager)
	//{
	//	// ���� : DT_MissionData�� �����ϴ� �̼� ID��
	//	TArray<int32> StageMissions;
	//	StageMissions.Add(1001);
	//	StageMissions.Add(1002);
	//	
	//	MissionManager->InitializeMissions(StageMissions);
	//}

}
