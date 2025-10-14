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

    // �̺�Ʈ ����
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

    //// ����Ʈ �ʱ�ȭ
    //MissionList->ClearChildren();
    //EntryMap.Empty();

    //// �ʱ� ��Ʈ�� ����
    //BuildInitialEntries();

    //// ��������Ʈ ���ε�
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
//    // �����ֱ� + SlideIn
//    SetVisibility(ESlateVisibility::Visible);
//
//    if (SlideIn)
//    {
//        PlayAnimation(SlideIn, 0.f, 1);
//    }
//    else
//    {
//        // �ִ� ������ �׳� ���̰Ը�
//    }
//}
//
//void UMissionPanel::ClosePanel()
//{
//    if (SlideOut)
//    {
//        // �ߺ� ���ε� ����
//        UnbindAllFromAnimationFinished(SlideOut);
//
//        // ���� �� ȣ��� ��������Ʈ ���ε�
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
//    //    // �ִ� ���� �� Collapsed�� �ٲٰ� ������ Dynamic Delegate ���
//    //    float Duration = GetAnimationCurrentTime(SlideOut);
//    //    PlayAnimation(SlideOut, 0.f, 1);
//
//    //    // ������ Ÿ�̸ӷ� Collapsed ó�� (�ִ� ���� 0.3s ���� �� �ణ ����)
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
//    // �츮�� SlideOut���� ���ε������� ���� ���� ���ʿ�
//    SetVisibility(ESlateVisibility::Collapsed);
//    UnbindAllFromAnimationFinished(SlideOut); // ����ϰ� ����
//}

void UMissionPanel::SetPanelWidthFraction(float Fraction)
{
    if (!PanelRoot) return;

    Fraction = FMath::Clamp(Fraction, 0.1f, 0.5f); // 10%~50% ����
    int32 ViewX = 1280, ViewY = 720;
    if (APlayerController* PC = GetOwningPlayer())
    {
        PC->GetViewportSize(ViewX, ViewY);
    }

    const float W = ViewX * Fraction;   // ���� 1/3 ����
    const float H = ViewY * 0.60f;      // ȭ�� 60%�� ��� (���ϸ� 0.7 ������)

    PanelRoot->SetWidthOverride(W);
    PanelRoot->SetHeightOverride(H);    // ������ ���� Ȯ�� ����
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

    // ������ ����(�������Ʈ������ Collapsed�� ����)
    if (PanelRoot)
        PanelRoot->SetVisibility(ESlateVisibility::Collapsed);

    //Super::NativeConstruct();

    //// �ݱ� ��ư Ŭ�� �� SlideOut + Collapsed ó��
    //if (BtnClose)
    //{
    //    BtnClose->OnClicked.Clear();
    //    BtnClose->OnClicked.AddDynamic(this, &UMissionPanel::ClosePanel);
    //}

    // �⺻�� ���� ���� (BP���� Collapsed�� �����ص� ��)
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

    RecalculateOverallProgress(); // �ʿ� ������ ���� ����� ����

    //if (UUserWidget** Found = EntryMap.Find(MissionID))
    //{
    //    UpdateEntryWidget(*Found, Runtime);
    //}
    //else
    //{
    //    // (����) ��Ÿ�� ���� ���ο� �̼��� �߰��Ǵ� ��츦 ���
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
    // ���� �Ϸ� �� ������ �ʿ��ϸ� ���⼭ ó�� (�佺Ʈ/����/�ڵ� �ݱ� ��)
  // ��: �ڵ� �ݱ�
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

    //// ActiveMissions�� ���� ��Ʈ�� ���� �� �ʱ� ǥ��
    //for (const auto& Pair : Manager->ActiveMissions)
    //{
    //    const int32 MissionID = Pair.Key;
    //    const FMissionRuntime& Runtime = Pair.Value;

    //    UUserWidget* Entry = CreateWidget<UUserWidget>(GetWorld(), MissionEntryClass);
    //    if (!Entry)
    //        continue;

    //    // ����Ʈ�� �߰�
    //   /* if (UVerticalBoxSlot* Slot = MissionList->AddChildToVerticalBox(Entry))
    //    {
    //        Slot->SetPadding(FMargin(6.f, 6.f));
    //    }*/
    //    if (UVerticalBoxSlot* EntrySlot = MissionList->AddChildToVerticalBox(Entry))
    //    {
    //        EntrySlot->SetPadding(FMargin(6.f, 6.f));
    //    }
    //    // ���� ����
    //    EntryMap.Add(MissionID, Entry);

    //    // �ʱ� ǥ�� (0/n)
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

    //// �̸����� ���� ���� ã�� (WBP_MissionEntry ���� �̸��� ��Ȯ�� ����� ��)
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

    //// �Ϸ� �� ����/���� �� ������ ����
    //const float Opacity = Runtime.bCompleted ? 0.7f : 1.0f;
    //Entry->SetRenderOpacity(Opacity);
}

void UMissionPanel::RecalculateOverallProgress()
{
    // ��ü ���൵ �� �� ���� ����ֵ� ��.
}
