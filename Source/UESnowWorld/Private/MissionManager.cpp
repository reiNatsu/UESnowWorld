// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionManager.h"
#include "UObject/ConstructorHelpers.h"

AMissionManager::AMissionManager()
{
	PrimaryActorTick.bCanEverTick = false;

	// (����) �ڵ�� DT �ε��ϰ� �ʹٸ� ��� ����
	// static ConstructorHelpers::FObjectFinder<UDataTable> DT(TEXT("/Game/DataTables/DT_MissionData.DT_MissionData"));
	// if (DT.Succeeded()) MissionDataTable = DT.Object;
}

void AMissionManager::InitializeMissions(const TArray<int32>& MissionIDs)
{
	if (!MissionDataTable)
	{
		//UE_LOG(LogTemp, Warning, TEXT("[MissionManager] MissionDataTable is NULL"));
		return;
	}

	ActiveMissions.Empty();

	for (int32 ID : MissionIDs)
	{
		// �� RowName�� �ƴ� MissionID �÷����� �˻�
		const FMissionData* Row = FindMissionRowById(ID);
		if (!Row)
		{
			//UE_LOG(LogTemp, Warning, TEXT("[MissionManager] InitMissions: MissionID %d not found in DT (by column)"), ID);
			continue;
		}

		FMissionRuntime Runtime;
		Runtime.MissionID = Row->MissionID;
		Runtime.TargetCount = Row->TargetCount;
		Runtime.Description = Row->Description;
		Runtime.CheckType = Row->CheckType;
		Runtime.Param = Row->Param;
		Runtime.CurrentCount = 0;
		Runtime.bCompleted = false;

		ActiveMissions.Add(Runtime.MissionID, Runtime);

		// �ʱ� ���µ� UI�� �ѷ��ֱ� (0/n)
		Broadcast(Runtime.MissionID);
	}

	//check(MissionDataTable);		// DT �������̸� ������ �ٷ� �˸�

	//ActiveMissions.Empty();
	//
	//for (int32 ID : MissionIDs)
	//{
	//	const FMissionData* Row = MissionDataTable->FindRow<FMissionData>(*FString::FromInt(ID), TEXT("InitMissions"));
	//	
	//	if (!Row)
	//		continue;

	//	FMissionRuntime Runtime;
	//	Runtime.MissionID = Row->MissionID;
	//	Runtime.TargetCount = Row->TargetCount;
	//	Runtime.Description = Row->Description;
	//	Runtime.CheckType = Row->CheckType;
	//	Runtime.Param = Row->Param;

	//	ActiveMissions.Add(Runtime.MissionID, Runtime);

	//	//�ʱ� ���µ� UI�� �ѷ��ֱ� (0/n)
	//	Broadcast(Runtime.MissionID);
	//}
}


void AMissionManager::HandleEvent(FName EventType, UObject* Context, int32 Amount)
{
	if (ActiveMissions.Num() == 0)
		return;

	for (auto& Pair : ActiveMissions)
	{
		FMissionRuntime& R = Pair.Value;

		if (R.bCompleted)
			continue;
		if (R.CheckType != EventType)
			continue;

		// (�ʿ� ��) Context/Param�� ���͸� ���� (��: Ư�� ���͸� ī��Ʈ)
	   // if (!R.Param.IsEmpty()) { ... }

		R.CurrentCount = FMath::Clamp(R.CurrentCount + Amount, 0, R.TargetCount);
		if (R.CurrentCount >= R.TargetCount)
			R.bCompleted = true;

		Broadcast(R.MissionID);
	}

	if (CheckAllCompleted())
		OnAllMissionsCompleted.Broadcast();
}


void AMissionManager::Broadcast(int32 MissionID)
{
	if (FMissionRuntime* Found = ActiveMissions.Find(MissionID))
		OnMissionProgressUpdated.Broadcast(MissionID, *Found);
}

bool AMissionManager::CheckAllCompleted() const
{
	if (ActiveMissions.Num() == 0) return false;

	for (const auto& P : ActiveMissions)
	{
		if (!P.Value.bCompleted)
			return false;
	}
	return true;

	/*if (ActiveMissions.Num() == 0) return false;

	for (const auto& P : ActiveMissions)
	{
		if (!P.Value.bCompleted)
			return false;
	}

	return ActiveMissions.Num() > 0;*/
}

const FMissionData* AMissionManager::FindMissionRowById(int32 MissionID) const
{
	if (!MissionDataTable) return nullptr;

	// RowName�� �����ϰ� RowStruct �޸𸮸� ���� ��ȸ
	const TMap<FName, uint8*>& RowMap = MissionDataTable->GetRowMap();
	for (const TPair<FName, uint8*>& It : RowMap)
	{
		const FMissionData* Row = reinterpret_cast<const FMissionData*>(It.Value);
		if (Row && Row->MissionID == MissionID)
		{
			return Row;
		}
	}
	return nullptr;
}

