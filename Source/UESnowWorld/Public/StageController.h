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

    // �������� ����(Ŭ����) Ʈ����: �� �Լ��� ȣ����!
    UFUNCTION(BlueprintCallable, Category = "Stage")
    void StageClear();

    // ���� Ʈ����(�ʿ� ��)
    UFUNCTION(BlueprintCallable, Category = "Stage")
    void StageFail();

    // �÷��̾� �ڵ� Ž�� ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage|Player")
    bool bAutoDetectPlayer = true;

    // ���� ������ �÷��̾�
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage|Player")
    TSoftObjectPtr<ACharacter> PlayerActor;

    // HP ����: �� ���� �̻��� �� ���� (0.5 = 50%)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage|Rule")
    float EndHpThreshold = 0.5f;

    // ��� �̼� �Ϸ�Ǹ� �ڵ����� StageClear() ȣ��
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage|Rule")
    bool bAutoClearWhenAllMissionsCompleted = false;

    // =========== ���� ���� ���� ���� ======================
       // ���� ����Ʈ��(������ ��ġ�� TargetPoint���� �巡��/���)
    UPROPERTY(EditAnywhere, Category = "Stage")
    TArray<AActor*> BossSpawnPoints;

    // �̹� �ǿ� ����� ������ Ŭ���� �ĺ���(BP_CHRBossBase �ڽ� BP��)
    UPROPERTY(EditAnywhere, Category = "Stage")
    TArray<TSubclassOf<ACHRBoss>> BossClassCandidates;

    // �̹� �ǿ� Ȯ���� ����(ó���� ���� 1�� ���� �� �������� ������ Ŭ���� ����)
    UPROPERTY(VisibleAnywhere, Category = "Stage")
    TSubclassOf<ACHRBoss> ChosenBossClass;

    // ���� ������ ������
    UPROPERTY(EditAnywhere, Category = "Stage")
    float RespawnDelay = 8.f;

    // �������� 4�� ������ �̼� ��ȣ(Ȯ������ ����: �ܺ� �ý��ۿ��� ȣ��)
    UFUNCTION(BlueprintCallable)
    void NotifyGeneratorFixedCountChanged(int32 FixedCount);


protected:
    virtual void BeginPlay() override;

    // ---- UI Ŭ���� ���� (�����Ϳ��� ����) ----
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UMissionPanel> MissionPanelClass;      // WBP_MissionPanel

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> MissionButtonClass;       // WBP_MissionButton (BtnMission ����)

    // ---- ��Ÿ�� �ν��Ͻ� ----
    UPROPERTY() UMissionPanel* MissionPanel = nullptr;
    UPROPERTY() UUserWidget* MissionButton = nullptr;

    // �̼� �Ŵ���(������ ��ġ�� �ְų� GameMode�� ����)
    UPROPERTY(EditInstanceOnly, Category = "Mission")
    AMissionManager* MissionManagerRef = nullptr; // �𸣸� �ڵ� Ž��

    UFUNCTION() void OnClickMissionButton();

private:
    AMissionManager* GetMissionManager() const;
    ACharacter* ResolvePlayer() const;
    float GetPlayerHpRatio(ACharacter* InPlayer) const;

    UFUNCTION()
    void OnAllMissionsCompleted();

    // =========== ���� ���� ���� ���� ======================
    UPROPERTY()
    ACHRBoss* CurrentBoss = nullptr;

    void SpawnBossAtRandomPoint();
    void HandleBossDied(); // ��������Ʈ ���ε�

    // ����ũ Ʈ���� �÷���
    bool bBerserkTriggered = false;
};
