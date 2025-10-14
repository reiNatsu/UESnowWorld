// Fill out your copyright notice in the Description page of Project Settings.


#include "StageController.h"

#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"

//#include "UESnowWorldGameMode.h"
#include "MissionPanel.h"
#include "MissionManager.h"
#include "HealthProvider.h"

#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AStageController::AStageController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AStageController::BeginPlay()
{
	Super::BeginPlay();

    // ���� ǥ��
    //UKismetSystemLibrary::PrintString(this, TEXT("[SC] BeginPlay"), true, true, FLinearColor::Yellow, 2.f);

    // ���� �÷��̾� ��Ʈ�ѷ� Ȯ��
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC) 
    {
        //UKismetSystemLibrary::PrintString(this, TEXT("[SC] No PlayerController"), true, true, FLinearColor::Red, 3.f);
        return;
    }

    // 2) �̼� �г� ���� & �⺻ ����
    if (MissionPanelClass)
    {
        MissionPanel = CreateWidget<UMissionPanel>(PC, MissionPanelClass);
        if (MissionPanel)
        {
            MissionPanel->AddToViewport(10);          // z-order
            MissionPanel->SetPanelWidthFraction(0.3333f); // ȭ���� 1/3 ��

            //// �Ŵ��� ���� ������ ���忡�� 1�� ã�� ����
            //AMissionManager* MM = MissionManagerRef;
            //if (!MM)
            //{
            //    for (TActorIterator<AMissionManager> It(GetWorld()); It; ++It) { MM = *It; break; }
            //}
            //if (MM)
            //{
            //    UKismetSystemLibrary::PrintString(
            //        this,
            //        FString::Printf(TEXT("[SC] Found MissionManager: %s"), *GetNameSafe(MM)), // �� ����!
            //        true, true, FLinearColor::Yellow, 2.f);

            //    UKismetSystemLibrary::PrintString(
            //        this,
            //        FString::Printf(TEXT("[SC] ActiveMissions Num = %d"), MM->ActiveMissions.Num()),
            //        true, true, FLinearColor::Blue, 2.f);


            //    MissionPanel->InitializeWithManager(MM);
            //}
            //else
            //{
            //
            //    UKismetSystemLibrary::PrintString(this, TEXT("[SC] MissionManager NOT FOUND"), true, true, FLinearColor::Red, 3.f);
            //}

            // ������ ���� (WBP������ PanelRoot=�������� �ص״ٸ� �� ���� ������ġ)
            MissionPanel->HidePanel();
        }
        else
        {
           // UKismetSystemLibrary::PrintString(this, TEXT("[SC] Panel create FAILED"), true, true, FLinearColor::Red, 3.f);
        }
    }

    // 3) MissionManager ã�� (���� ��ġ or ����)
    AMissionManager* MM = MissionManagerRef;
    if (!MM)
    {
        for (TActorIterator<AMissionManager> It(GetWorld()); It; ++It)
        {
            MM = *It;
            break;
        }
    }

    if (MM)
    {
      /*  UKismetSystemLibrary::PrintString(
            this,
            FString::Printf(TEXT("[SC] Found MissionManager: %s"), *GetNameSafe(MM)),
            true, true, FLinearColor::Yellow, 2.f
        );*/
    }
    else
    {
        //UKismetSystemLibrary::PrintString(this, TEXT("[SC] MissionManager NOT FOUND"), true, true, FLinearColor::Red, 3.f);
        // �ʿ��ϸ� ���⼭ ���� ���� �߰� ���� (������ ���� ��ġ ����)
    }

    // ----------------------------------------------------------------------------------------------
   // 4) (�ٽ�) ��Ƽ�� �̼��� ��� ������, DT���� ���� �ִ� 5�� �̾� �ʱ�ȭ
    if (MM && MM->ActiveMissions.Num() == 0 && MM->MissionDataTable)
    {
        TArray<FMissionData*> Rows;
        MM->MissionDataTable->GetAllRows<FMissionData>(TEXT("InitFromDT"), Rows);

        /*UKismetSystemLibrary::PrintString(
            this,
            FString::Printf(TEXT("[SC] DT Rows = %d"), Rows.Num()),
            true, true, FLinearColor::Blue, 2.f
        );*/

        if (Rows.Num() > 0)
        {
            // �ε��� ���� (UE5.5: Fisher-Yates)
            TArray<int32> Indexes; Indexes.Reserve(Rows.Num());
            for (int32 i = 0; i < Rows.Num(); ++i) Indexes.Add(i);

            FRandomStream Rng(FDateTime::Now().GetTicks());
            for (int32 i = Indexes.Num() - 1; i > 0; --i)
            {
                const int32 j = Rng.RandRange(0, i);
                if (i != j) Indexes.Swap(i, j);
            }

            const int32 PickCount = FMath::Min(5, Rows.Num());
            TArray<int32> IDs; IDs.Reserve(PickCount);

            for (int32 k = 0; k < PickCount; ++k)
            {
                if (const FMissionData* Row = Rows[Indexes[k]])
                {
                    IDs.Add(Row->MissionID);
                }
            }

            MM->InitializeMissions(IDs);

            /*UKismetSystemLibrary::PrintString(
                this,
                FString::Printf(TEXT("[SC] Init %d missions from DT"), IDs.Num()),
                true, true, FLinearColor::Green, 2.f
            );*/
        }
        else
        {
            //UKismetSystemLibrary::PrintString(this, TEXT("[SC] DT empty or wrong RowStruct"), true, true, FLinearColor::Red, 3.f);
        }
    }

    // ----------------------------------------------------------------------------------------------
    // 5) �гο� �Ŵ��� ���� (�ʱ� ��� ���� & ��������Ʈ ���ε�)
    if (MissionPanel && MM)
    {
        MissionPanel->InitializeWithManager(MM);
        //UKismetSystemLibrary::PrintString(this, TEXT("[SC] Panel InitializeWithManager OK"), true, true, FLinearColor::Green, 2.f);
    }


    // ----------------------------------------------------------------------------------------------
   // 6) "�̼� ����" ��ư ���� +OnClicked ��� ���ε�
    if (MissionButtonClass)
    {
        MissionButton = CreateWidget<UUserWidget>(PC, MissionButtonClass);
        if (MissionButton)
        {
            MissionButton->AddToViewport(11);

            if (UButton* Btn = Cast<UButton>(MissionButton->GetWidgetFromName(TEXT("BtnMission"))))
            {
                //UKismetSystemLibrary::PrintString(this, TEXT("[SC] Bind BtnMission OK"), true, true, FLinearColor::Green, 2.f);
                Btn->OnClicked.Clear();
                Btn->OnClicked.AddDynamic(this, &AStageController::OnClickMissionButton);
            }
            else
            {
                //UKismetSystemLibrary::PrintString(this, TEXT("[SC] BtnMission NOT FOUND"), true, true, FLinearColor::Red, 3.f);
            }
        }
        //else
            //UKismetSystemLibrary::PrintString(this, TEXT("[SC] MissionButton create FAILED"), true, true, FLinearColor::Red, 3.f);
    }
    //else
        //UKismetSystemLibrary::PrintString(this, TEXT("[SC] MissionButtonClass NOT set"), true, true, FLinearColor::Red, 3.f);
    // (����) ��� �̼� �Ϸ� �� �ڵ� Ŭ����
    /*if (AMissionManager* MM = GetMissionManager())
    {
        if (bAutoClearWhenAllMissionsCompleted)
        {
            MM->OnAllMissionsCompleted.AddDynamic(this, &AStageController::OnAllMissionsCompleted);
        }
    }*/
}



void AStageController::OnAllMissionsCompleted()
{
    StageClear();
}

void AStageController::StageClear()
{
    AMissionManager* MM = GetMissionManager();
    if (!MM)
    {
        UE_LOG(LogTemp, Warning, TEXT("[StageController] MissionManager not found."));
        return;
    }

    ACharacter* Player = ResolvePlayer();
    if (!Player)
    {
        UE_LOG(LogTemp, Warning, TEXT("[StageController] Player not resolved."));
        return;
    }

    const float HpRatio = GetPlayerHpRatio(Player); // 0~1
    UE_LOG(LogTemp, Log, TEXT("[StageController] StageClear: HpRatio=%.3f, Threshold=%.3f"),
        HpRatio, EndHpThreshold);

    // EndHP �̼��� DT���� CheckType=OnStageEnd, TargetCount=1 �� �����Ǿ� �־�� ��(Ȯ��)
    if (HpRatio >= EndHpThreshold)
    {
        MM->HandleEvent("OnStageEnd", Player, /*Amount=*/1);
    }

    // TODO: Ŭ���� ����/����/�� ��ȯ �� �߰� ó�� ����
}

void AStageController::StageFail()
{
    UE_LOG(LogTemp, Log, TEXT("[StageController] StageFail"));
    // TODO: ���� ó��(UI, ��Ʈ���� ��)
}

void AStageController::OnClickMissionButton()
{
    //UKismetSystemLibrary::PrintString(this, TEXT("[SC] Mission button CLICKED"), true, true, FLinearColor::Yellow, 1.5f);

    if (MissionPanel)
    {
        MissionPanel->TogglePanel(); // ���̱�/�����
        //UKismetSystemLibrary::PrintString(this, TEXT("[SC] TogglePanel called"), true, true, FLinearColor::Green, 1.5f);
    }
   /* else
        UKismetSystemLibrary::PrintString(this, TEXT("[SC] MissionPanel is NULL"), true, true, FLinearColor::Red, 2.0f);*/
}

AMissionManager* AStageController::GetMissionManager() const
{
    // GameMode ���� ����, ���� ���忡�� �Ŵ����� ���� Ž�� (Ȯ��)
    if (UWorld* World = GetWorld())
    {
        for (TActorIterator<AMissionManager> It(World); It; ++It)
        {
            return *It; // ù ��° MissionManager ��ȯ
        }
    }
    return nullptr;
}

ACharacter* AStageController::ResolvePlayer() const
{
    // 1) ���� ���� �켱
    if (PlayerActor.IsValid())
    {
        return PlayerActor.Get();
    }

    // 2) �ڵ� Ž��: �÷��̾� ��Ʈ�ѵ� ĳ����
    if (bAutoDetectPlayer)
    {
        if (UWorld* World = GetWorld())
        {
            for (TActorIterator<ACharacter> It(World); It; ++It)
            {
                if (ACharacter* Ch = *It; Ch && Ch->IsPlayerControlled())
                {
                    return Ch;
                }
            }
        }
    }
    return nullptr;
}

float AStageController::GetPlayerHpRatio(ACharacter* InPlayer) const
{
    // HealthProvider �������̽� ���� �� �� �� ��� (Ȯ��)
    if (InPlayer && InPlayer->GetClass()->ImplementsInterface(UHealthProvider::StaticClass()))
    {
        const float Ratio = IHealthProvider::Execute_GetHpRatio(InPlayer);
        return FMath::Clamp(Ratio, 0.f, 1.f);
    }

    // �ӽ� ����: �������̽� �̱����̸� 1.0���� ���� (�׽�Ʈ��)
    UE_LOG(LogTemp, Warning, TEXT("[StageController] Player lacks HealthProvider. Assuming 1.0"));
    return 1.0f;
}
