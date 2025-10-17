// Copyright Epic Games, Inc. All Rights Reserved.

#include "UESnowWorldGameMode.h"
#include "UESnowWorldCharacter.h"
#include "UObject/ConstructorHelpers.h"

#include "MissionManager.h"    //  매니저 정의
#include "MissionData.h"       //  FMissionData 정의
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

	// (선택) MissionManagerClass 기본값을 CDO에서 세팅하고 싶다면 여기서도 가능
   // static ConstructorHelpers::FClassFinder<AMissionManager> MMClass(TEXT("/Game/Blueprints/BP_MissionManager")); 
   // if (MMClass.Succeeded()) { MissionManagerClass = MMClass.Class; }
}

void AUESnowWorldGameMode::BeginPlay()
{
	Super::BeginPlay();

	// GameMode는 서버 권한에서만 동작 -> 안전하게 권한 체크
	if (!HasAuthority())
		return;

	// 1) 레벨에 이미 배치된 MissionManager찾기
	for (TActorIterator<AMissionManager> It(GetWorld()); It; ++It)
	{
		MissionManager = *It;
		break;
	}

	// 2) 없으면 스폰 (원하면 MissionManagerClass 사용)
	if (!MissionManager)
	{
		// 클래스 미지정이면 기본 클래스로 스폰
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		MissionManager = GetWorld()->SpawnActor<AMissionManager>(AMissionManager::StaticClass(), 
											FVector::ZeroVector, FRotator::ZeroRotator, Params);
		// 만약 BP 클래스로 스폰하고 싶으면 위 클래스를 MissionManagerClass로 교체
		// if (MissionManagerClass) { MissionManager = GetWorld()->SpawnActor<AMissionManager>(MissionManagerClass, ...); }
	}

	// 3) 초기화 (DT_MissionData에서 랜덤 최대 5개 선택)
	if (MissionManager && MissionManager->MissionDataTable)
	{
		// 3-1) 모든 행 가져오기
		TArray<FMissionData*> AllRows;
		MissionManager->MissionDataTable->GetAllRows<FMissionData>(TEXT("PickMissions"), AllRows);

		if (AllRows.Num() > 0)
		{
			// 3-2) 인덱스 배열 만들기
			TArray<int32> Indexes;
			Indexes.Reserve(AllRows.Num());
			for (int32 i = 0; i < AllRows.Num(); ++i)
			{
				Indexes.Add(i);
			}

			// 3-2-5) Fisher-Yates 셔플
			const uint32 Seed = static_cast<uint32>(FDateTime::Now().GetTicks() & 0xFFFFFFFF);
			FRandomStream Rng(Seed);
			for (int32 i = Indexes.Num() - 1; i > 0; --i)
			{
				const int32 j = Rng.RandRange(0, i);
				if (i != j) { Indexes.Swap(i, j); }
			}

			// 3-3) 최대 5개 픽
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

			// 3-4) 선택된 미션으로 초기화
			MissionManager->InitializeMissions(StageMissions);
		}
	}
	//if (MissionManager)
	//{
	//	// 예시 : DT_MissionData에 존재하는 미션 ID들
	//	TArray<int32> StageMissions;
	//	StageMissions.Add(1001);
	//	StageMissions.Add(1002);
	//	
	//	MissionManager->InitializeMissions(StageMissions);
	//}

}
