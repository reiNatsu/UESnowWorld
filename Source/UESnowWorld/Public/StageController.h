// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MissionPanel.h"
#include "GameFramework/Actor.h"

#include "StageController.generated.h"


class AMissionManager;
class ACharacter;
class UButton;
class AMissionManager;

class ACHRBoss;
class UDataTable;

UCLASS(Blueprintable)
class UESNOWWORLD_API AStageController : public AActor
{
    GENERATED_BODY()

public:
    AStageController();

    // 스테이지 성공(클리어) 트리거: 이 함수를 호출해!
    UFUNCTION(BlueprintCallable, Category = "Stage")
    void StageClear();

    // 실패 트리거(필요 시)
    UFUNCTION(BlueprintCallable, Category = "Stage")
    void StageFail();

    // 플레이어 자동 탐지 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage|Player")
    bool bAutoDetectPlayer = true;

    // 수동 지정용 플레이어
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage|Player")
    TSoftObjectPtr<ACharacter> PlayerActor;

    // HP 조건: 이 비율 이상일 때 성공 (0.5 = 50%)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage|Rule")
    float EndHpThreshold = 0.5f;

    // 모든 미션 완료되면 자동으로 StageClear() 호출
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage|Rule")
    bool bAutoClearWhenAllMissionsCompleted = false;

    // =========== 보스 몬스터 스폰 관련 ======================
       // 스폰 포인트들(레벨에 배치된 TargetPoint들을 드래그/등록)
    UPROPERTY(EditAnywhere, Category = "Stage")
    TArray<AActor*> BossSpawnPoints;

    // 이번 판에 사용할 “보스 클래스 후보”(BP_CHRBossBase 자식 BP들)
    UPROPERTY(EditAnywhere, Category = "Stage")
    TArray<TSubclassOf<ACHRBoss>> BossClassCandidates;

    // 이번 판에 확정된 보스(처음에 랜덤 1종 선택 → 리스폰도 동일한 클래스 유지)
    UPROPERTY(VisibleAnywhere, Category = "Stage")
    TSubclassOf<ACHRBoss> ChosenBossClass;

    // 보스 리스폰 딜레이
    UPROPERTY(EditAnywhere, Category = "Stage")
    float RespawnDelay = 8.f;

    // “발전기 4개 수리” 미션 신호(확실하지 않음: 외부 시스템에서 호출)
    UFUNCTION(BlueprintCallable)
    void NotifyGeneratorFixedCountChanged(int32 FixedCount);


protected:
    virtual void BeginPlay() override;

    // ---- UI 클래스 지정 (에디터에서 지정) ----
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UMissionPanel> MissionPanelClass;      // WBP_MissionPanel

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> MissionButtonClass;       // WBP_MissionButton (BtnMission 포함)

    // ---- 런타임 인스턴스 ----
    UPROPERTY() UMissionPanel* MissionPanel = nullptr;
    UPROPERTY() UUserWidget* MissionButton = nullptr;

    // 미션 매니저(레벨에 배치돼 있거나 GameMode가 스폰)
    UPROPERTY(EditInstanceOnly, Category = "Mission")
    AMissionManager* MissionManagerRef = nullptr; // 모르면 자동 탐색

    UFUNCTION() void OnClickMissionButton();

private:
    AMissionManager* GetMissionManager() const;
    ACharacter* ResolvePlayer() const;
    float GetPlayerHpRatio(ACharacter* InPlayer) const;

    UFUNCTION()
    void OnAllMissionsCompleted();

    // =========== 보스 몬스터 스폰 관련 ======================
    UPROPERTY()
    ACHRBoss* CurrentBoss = nullptr;

    void SpawnBossAtRandomPoint();
    void HandleBossDied(); // 델리게이트 바인딩

    // 버서크 트리거 플래그
    bool bBerserkTriggered = false;
};
