// Fill out your copyright notice in the Description page of Project Settings.


#include "MissionPanel.h"
#include "MissionManager.h"
#include "MissionRuntime.h"

#include "Kismet/KismetSystemLibrary.h"

#include "Components/SizeBox.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UMissionPanel::InitializeWithManager(AMissionManager* InManager)
{
    Manager = InManager;
    if (!PanelRoot || !MissionList || !MissionEntryClass || !Manager)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MissionPanel] Init missing refs"));
        return;
    }

    MissionList->ClearChildren();
    EntryMap.Empty();

    BuildInitialEntries();

    // 이벤트 연결
    Manager->OnMissionProgressUpdated.AddDynamic(this, &UMissionPanel::OnMissionUpdated);
    Manager->OnAllMissionsCompleted.AddDynamic(this, &UMissionPanel::OnAllMissionsCompleted);
    //Manager = InManager;

    //if (!MissionList || !MissionEntryClass || !Manager)
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("[MissionPanel] InitializeWithManager: Missing refs (List:%s, EntryClass:%s, Manager:%s)"),
    //        MissionList ? TEXT("OK") : TEXT("NULL"),
    //        *GetNameSafe(MissionEntryClass),
    //        *GetNameSafe(Manager));
    //    return;
    //}

    //// 리스트 초기화
    //MissionList->ClearChildren();
    //EntryMap.Empty();

    //// 초기 엔트리 구성
    //BuildInitialEntries();

    //// 델리게이트 바인딩
    //Manager->OnMissionProgressUpdated.AddDynamic(this, &UMissionPanel::OnMissionUpdated);
    //Manager->OnAllMissionsCompleted.AddDynamic(this, &UMissionPanel::OnAllMissionsCompleted);
}

void UMissionPanel::ShowPanel()
{
    if (PanelRoot)
        PanelRoot->SetVisibility(ESlateVisibility::Visible);
}

void UMissionPanel::HidePanel()
{
    if (PanelRoot)
        PanelRoot->SetVisibility(ESlateVisibility::Collapsed);
}

void UMissionPanel::TogglePanel()
{
    if (!PanelRoot) return;
    const ESlateVisibility Now = PanelRoot->GetVisibility();
    PanelRoot->SetVisibility(Now == ESlateVisibility::Collapsed ? ESlateVisibility::Visible
        : ESlateVisibility::Collapsed);
}

//void UMissionPanel::OpenPanel()
//{
//    // 보여주기 + SlideIn
//    SetVisibility(ESlateVisibility::Visible);
//
//    if (SlideIn)
//    {
//        PlayAnimation(SlideIn, 0.f, 1);
//    }
//    else
//    {
//        // 애니 없으면 그냥 보이게만
//    }
//}
//
//void UMissionPanel::ClosePanel()
//{
//    if (SlideOut)
//    {
//        // 중복 바인딩 방지
//        UnbindAllFromAnimationFinished(SlideOut);
//
//        // 종료 시 호출될 델리게이트 바인딩
//        FWidgetAnimationDynamicEvent EndDelegate;
//        EndDelegate.BindDynamic(this, &UMissionPanel::OnSlideOutFinished);
//        BindToAnimationFinished(SlideOut, EndDelegate);
//
//        PlayAnimation(SlideOut, 0.f, 1);
//    }
//    else
//    {
//        SetVisibility(ESlateVisibility::Collapsed);
//    }
//    //if (SlideOut)
//    //{
//    //    // 애니 종료 후 Collapsed로 바꾸고 싶으면 Dynamic Delegate 사용
//    //    float Duration = GetAnimationCurrentTime(SlideOut);
//    //    PlayAnimation(SlideOut, 0.f, 1);
//
//    //    // 간단히 타이머로 Collapsed 처리 (애니 길이 0.3s 가정 시 약간 여유)
//    //    const float CollapseDelay = SlideOut->GetEndTime() + 0.01f;
//    //    if (UWorld* World = GetWorld())
//    //    {
//    //        FTimerHandle Timer;
//    //        World->GetTimerManager().SetTimer(
//    //            Timer,
//    //            FTimerDelegate::CreateWeakLambda(this, [this]()
//    //                {
//    //                    this->SetVisibility(ESlateVisibility::Collapsed);
//    //                }),
//    //            CollapseDelay,
//    //            false
//    //        );
//    //    }
//    //}
//    //else
//    //{
//    //    SetVisibility(ESlateVisibility::Collapsed);
//    //}
//}


//void UMissionPanel::OnSlideOutFinished()
//{
//    // 우리가 SlideOut에만 바인딩했으니 별도 구분 불필요
//    SetVisibility(ESlateVisibility::Collapsed);
//    UnbindAllFromAnimationFinished(SlideOut); // 깔끔하게 해제
//}

void UMissionPanel::SetPanelWidthFraction(float Fraction)
{
    if (!PanelRoot) return;

    Fraction = FMath::Clamp(Fraction, 0.1f, 0.5f); // 10%~50% 사이
    int32 ViewX = 1280, ViewY = 720;
    if (APlayerController* PC = GetOwningPlayer())
    {
        PC->GetViewportSize(ViewX, ViewY);
    }

    const float W = ViewX * Fraction;   // 좌측 1/3 정도
    const float H = ViewY * 0.60f;      // 화면 60%만 사용 (원하면 0.7 등으로)

    PanelRoot->SetWidthOverride(W);
    PanelRoot->SetHeightOverride(H);    // 과도한 세로 확장 방지
}

void UMissionPanel::NativeConstruct()
{
    Super::NativeConstruct();

    if (BtnClose)
    {
        BtnClose->OnClicked.Clear();
        BtnClose->OnClicked.AddDynamic(this, &UMissionPanel::HidePanel);
    }

    /*UKismetSystemLibrary::PrintString(this,
        FString::Printf(TEXT("[Panel] BindCheck PanelRoot=%s, MissionList=%s, EntryClass=%s"),
            PanelRoot ? TEXT("OK") : TEXT("NULL"),
            MissionList ? TEXT("OK") : TEXT("NULL"),
            *GetNameSafe(MissionEntryClass)),
        true, true, PanelRoot && MissionList ? FLinearColor::Green : FLinearColor::Red, 2.f);*/

    // 시작은 숨김(블루프린트에서도 Collapsed로 설정)
    if (PanelRoot)
        PanelRoot->SetVisibility(ESlateVisibility::Collapsed);

    //Super::NativeConstruct();

    //// 닫기 버튼 클릭 → SlideOut + Collapsed 처리
    //if (BtnClose)
    //{
    //    BtnClose->OnClicked.Clear();
    //    BtnClose->OnClicked.AddDynamic(this, &UMissionPanel::ClosePanel);
    //}

    // 기본은 숨김 권장 (BP에서 Collapsed로 시작해도 됨)
    // SetVisibility(ESlateVisibility::Collapsed);
}

void UMissionPanel::OnMissionUpdated(int32 MissionID, const FMissionRuntime& Runtime)
{
    if (UUserWidget** Found = EntryMap.Find(MissionID))
    {
        UpdateEntryWidget(*Found, Runtime);
    }
    else
    {
        UUserWidget* Entry = CreateWidget<UUserWidget>(GetWorld(), MissionEntryClass);
        if (!Entry) return;

        if (UVerticalBoxSlot* S = MissionList->AddChildToVerticalBox(Entry))
        {
            S->SetPadding(FMargin(8.f, 6.f));
        }
        EntryMap.Add(MissionID, Entry);
        UpdateEntryWidget(Entry, Runtime);
    }

    RecalculateOverallProgress(); // 필요 없으면 내용 비워도 무방

    //if (UUserWidget** Found = EntryMap.Find(MissionID))
    //{
    //    UpdateEntryWidget(*Found, Runtime);
    //}
    //else
    //{
    //    // (예외) 런타임 도중 새로운 미션이 추가되는 경우를 대비
    //    UUserWidget* Entry = CreateWidget<UUserWidget>(GetWorld(), MissionEntryClass);
    //    if (!Entry) return;

    //   /* if (UVerticalBoxSlot* Slot = MissionList->AddChildToVerticalBox(Entry))
    //    {
    //        Slot->SetPadding(FMargin(6.f, 6.f));
    //    }*/
    //    if (UVerticalBoxSlot* EntrySlot = MissionList->AddChildToVerticalBox(Entry))
    //    {
    //        EntrySlot->SetPadding(FMargin(6.f, 6.f));
    //    }
    //    EntryMap.Add(MissionID, Entry);
    //    UpdateEntryWidget(Entry, Runtime);
    //}
}

void UMissionPanel::OnAllMissionsCompleted()
{
    // 전부 완료 시 연출이 필요하면 여기서 처리 (토스트/사운드/자동 닫기 등)
  // 예: 자동 닫기
    //ClosePanel();
    RecalculateOverallProgress();
}

void UMissionPanel::BuildInitialEntries()
{
    if (!Manager || !MissionList || !MissionEntryClass) return;

    const int32 Count = Manager->ActiveMissions.Num();
    /*UKismetSystemLibrary::PrintString(this,
        FString::Printf(TEXT("[Panel] BuildInitialEntries: %d"), Count),
        true, true, Count > 0 ? FLinearColor::Green : FLinearColor::Red, 2.f);*/

    for (const auto& Pair : Manager->ActiveMissions)
    {
        const int32 MissionID = Pair.Key;
        const FMissionRuntime& Runtime = Pair.Value;

        UUserWidget* Entry = CreateWidget<UUserWidget>(GetWorld(), MissionEntryClass);
        if (!Entry) continue;

        if (UVerticalBoxSlot* S = MissionList->AddChildToVerticalBox(Entry))
        {
            S->SetHorizontalAlignment(HAlign_Fill);
            S->SetPadding(FMargin(8.f, 6.f));
        }

        EntryMap.Add(MissionID, Entry);
        UpdateEntryWidget(Entry, Runtime);

        /*UKismetSystemLibrary::PrintString(this,
            FString::Printf(TEXT("[Panel] Add #%d (%s)"), MissionID, *Runtime.Description),
            true, true, FLinearColor::Yellow, 1.5f);*/
    }

    //// ActiveMissions를 돌며 엔트리 생성 및 초기 표시
    //for (const auto& Pair : Manager->ActiveMissions)
    //{
    //    const int32 MissionID = Pair.Key;
    //    const FMissionRuntime& Runtime = Pair.Value;

    //    UUserWidget* Entry = CreateWidget<UUserWidget>(GetWorld(), MissionEntryClass);
    //    if (!Entry)
    //        continue;

    //    // 리스트에 추가
    //   /* if (UVerticalBoxSlot* Slot = MissionList->AddChildToVerticalBox(Entry))
    //    {
    //        Slot->SetPadding(FMargin(6.f, 6.f));
    //    }*/
    //    if (UVerticalBoxSlot* EntrySlot = MissionList->AddChildToVerticalBox(Entry))
    //    {
    //        EntrySlot->SetPadding(FMargin(6.f, 6.f));
    //    }
    //    // 맵핑 저장
    //    EntryMap.Add(MissionID, Entry);

    //    // 초기 표시 (0/n)
    //    UpdateEntryWidget(Entry, Runtime);
    //}
}

void UMissionPanel::UpdateEntryWidget(UUserWidget* Entry, const FMissionRuntime& Runtime)
{
    if (!Entry) return;
    if (UTextBlock* Desc = Cast<UTextBlock>(Entry->GetWidgetFromName(TEXT("Text_Desc"))))
        Desc->SetText(FText::FromString(Runtime.Description));
    
    if (UTextBlock* Prog = Cast<UTextBlock>(Entry->GetWidgetFromName(TEXT("Text_Progress"))))
        Prog->SetText(FText::FromString(FString::Printf(TEXT("(%d/%d)"), Runtime.CurrentCount, Runtime.TargetCount)));

    Entry->SetRenderOpacity(Runtime.bCompleted ? 0.8f : 1.0f);


    UKismetSystemLibrary::PrintString(this,
        FString::Printf(TEXT("[Panel] Update: %s | Desc=%s Prog=%s"),
            *GetNameSafe(Entry),
            Entry->GetWidgetFromName(TEXT("Text_Desc")) ? TEXT("OK") : TEXT("NULL"),
            Entry->GetWidgetFromName(TEXT("Text_Progress")) ? TEXT("OK") : TEXT("NULL")),
        true, true, FLinearColor::Green, 1.5f);
    //if (!Entry) return;

    //// 이름으로 하위 위젯 찾기 (WBP_MissionEntry 내부 이름을 정확히 맞춰야 함)
    //if (UTextBlock* Desc = Cast<UTextBlock>(Entry->GetWidgetFromName(TEXT("Text_Desc"))))
    //{
    //    Desc->SetText(FText::FromString(Runtime.Description));
    //}

    //if (UTextBlock* Prog = Cast<UTextBlock>(Entry->GetWidgetFromName(TEXT("Text_Progress"))))
    //{
    //    Prog->SetText(FText::FromString(FString::Printf(TEXT("(%d/%d)"), Runtime.CurrentCount, Runtime.TargetCount)));
    //}

    //if (UProgressBar* Bar = Cast<UProgressBar>(Entry->GetWidgetFromName(TEXT("Progress"))))
    //{
    //    const float Ratio = (Runtime.TargetCount > 0) ? (float)Runtime.CurrentCount / (float)Runtime.TargetCount : 0.f;
    //    Bar->SetPercent(Ratio);
    //}

    //// 완료 시 투명도/색상 등 간단한 연출
    //const float Opacity = Runtime.bCompleted ? 0.7f : 1.0f;
    //Entry->SetRenderOpacity(Opacity);
}

void UMissionPanel::RecalculateOverallProgress()
{
    // 전체 진행도 바 안 쓰면 비워둬도 됨.
}
