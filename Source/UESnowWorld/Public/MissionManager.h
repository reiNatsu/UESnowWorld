
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "MissionData.h"			 // FMissionData (데이터테이블 RowStruct)
#include "MissionRuntime.h"			// FMissionRuntime (런타임 상태)
#include "MissionManager.generated.h"

/**
 * 
 */

 // UI에 진행도 갱신 통지
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMissionProgressUpdated, int32, MissionID, const FMissionRuntime&, Runtime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllMissionsCompleted);


UCLASS()
class UESNOWWORLD_API AMissionManager : public AActor
{
	GENERATED_BODY()

public:
	AMissionManager();

	// 스테이지 시작 시 호출 : 미션 ID 세팅
	UFUNCTION(BlueprintCallable)
	void InitializeMissions(const TArray<int32>& MissionIDs);

	// 게임 내 이벤트를 여기로 던지면 자동 처리
	UFUNCTION(BlueprintCallable)
	void HandleEvent(FName EventType, UObject* Context = nullptr, int32 Amount = 1);

	// DataTable 참조 (에디터에서 지정하거나 코드로 로드)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mission|Config")
	UDataTable* MissionDataTable = nullptr;

	/** 시작 시 자동 초기화할 ID들 (옵션) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission|Config")
	TArray<int32> DefaultMissionIDs;

	// 현재 활성 미션들
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mission|Runtime")
	TMap<int32, FMissionRuntime> ActiveMissions;


	// UI 바인딩용 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Mission|Event")
	FOnMissionProgressUpdated OnMissionProgressUpdated;
	
	UPROPERTY(BlueprintAssignable, Category = "Mission|Event")
	FOnAllMissionsCompleted OnAllMissionsCompleted;

private:
	void Broadcast(int32 MissionID);
	bool CheckAllCompleted() const;

	/** RowName을 무시하고 MissionID 컬럼으로 행을 찾는다 */
	const FMissionData* FindMissionRowById(int32 MissionID) const;
};
