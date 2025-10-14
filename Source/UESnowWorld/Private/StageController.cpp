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

    // 시작 표시
    //UKismetSystemLibrary::PrintString(this, TEXT("[SC] BeginPlay"), true, true, FLinearColor::Yellow, 2.f);

    // 로컬 플레이어 컨트롤러 확보
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC) 
    {
        //UKismetSystemLibrary::PrintString(this, TEXT("[SC] No PlayerController"), true, true, FLinearColor::Red, 3.f);
        return;
    }

    // 2) 미션 패널 생성 & 기본 세팅
    if (MissionPanelClass)
    {
        MissionPanel = CreateWidget<UMissionPanel>(PC, MissionPanelClass);
        if (MissionPanel)
        {
            MissionPanel->AddToViewport(10);          // z-order
            MissionPanel->SetPanelWidthFraction(0.3333f); // 화면의 1/3 폭

            //// 매니저 참조 없으면 월드에서 1개 찾아 연결
            //AMissionManager* MM = MissionManagerRef;
            //if (!MM)
            //{
            //    for (TActorIterator<AMissionManager> It(GetWorld()); It; ++It) { MM = *It; break; }
            //}
            //if (MM)
            //{
            //    UKismetSystemLibrary::PrintString(
            //        this,
            //        FString::Printf(TEXT("[SC] Found MissionManager: %s"), *GetNameSafe(MM)), // ← 여기!
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

            // 시작은 숨김 (WBP에서도 PanelRoot=접힘으로 해뒀다면 이 줄은 안전장치)
            MissionPanel->HidePanel();
        }
        else
        {
           // UKismetSystemLibrary::PrintString(this, TEXT("[SC] Panel create FAILED"), true, true, FLinearColor::Red, 3.f);
        }
    }

    // 3) MissionManager 찾기 (레벨 배치 or 참조)
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
        // 필요하면 여기서 스폰 로직 추가 가능 (지금은 레벨 배치 전제)
    }

    // ----------------------------------------------------------------------------------------------
   // 4) (핵심) 액티브 미션이 비어 있으면, DT에서 랜덤 최대 5개 뽑아 초기화
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
            // 인덱스 섞기 (UE5.5: Fisher-Yates)
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
    // 5) 패널에 매니저 연결 (초기 목록 생성 & 델리게이트 바인딩)
    if (MissionPanel && MM)
    {
        MissionPanel->InitializeWithManager(MM);
        //UKismetSystemLibrary::PrintString(this, TEXT("[SC] Panel InitializeWithManager OK"), true, true, FLinearColor::Green, 2.f);
    }


    // ----------------------------------------------------------------------------------------------
   // 6) "미션 보기" 버튼 생성 +OnClicked 토글 바인딩
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
    // (선택) 모든 미션 완료 시 자동 클리어
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

    // EndHP 미션은 DT에서 CheckType=OnStageEnd, TargetCount=1 로 구성되어 있어야 함(확실)
    if (HpRatio >= EndHpThreshold)
    {
        MM->HandleEvent("OnStageEnd", Player, /*Amount=*/1);
    }

    // TODO: 클리어 연출/보상/씬 전환 등 추가 처리 지점
}

void AStageController::StageFail()
{
    UE_LOG(LogTemp, Log, TEXT("[StageController] StageFail"));
    // TODO: 실패 처리(UI, 리트라이 등)
}

void AStageController::OnClickMissionButton()
{
    //UKismetSystemLibrary::PrintString(this, TEXT("[SC] Mission button CLICKED"), true, true, FLinearColor::Yellow, 1.5f);

    if (MissionPanel)
    {
        MissionPanel->TogglePanel(); // 보이기/숨기기
        //UKismetSystemLibrary::PrintString(this, TEXT("[SC] TogglePanel called"), true, true, FLinearColor::Green, 1.5f);
    }
   /* else
        UKismetSystemLibrary::PrintString(this, TEXT("[SC] MissionPanel is NULL"), true, true, FLinearColor::Red, 2.0f);*/
}

AMissionManager* AStageController::GetMissionManager() const
{
    // GameMode 의존 없이, 현재 월드에서 매니저를 직접 탐색 (확실)
    if (UWorld* World = GetWorld())
    {
        for (TActorIterator<AMissionManager> It(World); It; ++It)
        {
            return *It; // 첫 번째 MissionManager 반환
        }
    }
    return nullptr;
}

ACharacter* AStageController::ResolvePlayer() const
{
    // 1) 수동 지정 우선
    if (PlayerActor.IsValid())
    {
        return PlayerActor.Get();
    }

    // 2) 자동 탐색: 플레이어 컨트롤된 캐릭터
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
    // HealthProvider 인터페이스 구현 시 그 값 사용 (확실)
    if (InPlayer && InPlayer->GetClass()->ImplementsInterface(UHealthProvider::StaticClass()))
    {
        const float Ratio = IHealthProvider::Execute_GetHpRatio(InPlayer);
        return FMath::Clamp(Ratio, 0.f, 1.f);
    }

    // 임시 폴백: 인터페이스 미구현이면 1.0으로 간주 (테스트용)
    UE_LOG(LogTemp, Warning, TEXT("[StageController] Player lacks HealthProvider. Assuming 1.0"));
    return 1.0f;
}
