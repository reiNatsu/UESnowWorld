
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "MissionData.h"			 // FMissionData (���������̺� RowStruct)
#include "MissionRuntime.h"			// FMissionRuntime (��Ÿ�� ����)
#include "MissionManager.generated.h"

/**
 * 
 */

 // UI�� ���൵ ���� ����
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMissionProgressUpdated, int32, MissionID, const FMissionRuntime&, Runtime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllMissionsCompleted);


UCLASS()
class UESNOWWORLD_API AMissionManager : public AActor
{
	GENERATED_BODY()

public:
	AMissionManager();

	// �������� ���� �� ȣ�� : �̼� ID ����
	UFUNCTION(BlueprintCallable)
	void InitializeMissions(const TArray<int32>& MissionIDs);

	// ���� �� �̺�Ʈ�� ����� ������ �ڵ� ó��
	UFUNCTION(BlueprintCallable)
	void HandleEvent(FName EventType, UObject* Context = nullptr, int32 Amount = 1);

	// DataTable ���� (�����Ϳ��� �����ϰų� �ڵ�� �ε�)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission|Config")
	UDataTable* MissionDataTable = nullptr;

	/** ���� �� �ڵ� �ʱ�ȭ�� ID�� (�ɼ�) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission|Config")
	TArray<int32> DefaultMissionIDs;

	// ���� Ȱ�� �̼ǵ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mission|Runtime")
	TMap<int32, FMissionRuntime> ActiveMissions;


	// UI ���ε��� ��������Ʈ
	UPROPERTY(BlueprintAssignable, Category = "Mission|Event")
	FOnMissionProgressUpdated OnMissionProgressUpdated;
	
	UPROPERTY(BlueprintAssignable, Category = "Mission|Event")
	FOnAllMissionsCompleted OnAllMissionsCompleted;

private:
	void Broadcast(int32 MissionID);
	bool CheckAllCompleted() const;

	/** RowName�� �����ϰ� MissionID �÷����� ���� ã�´� */
	const FMissionData* FindMissionRowById(int32 MissionID) const;
};
