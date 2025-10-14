// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MissionPanel.generated.h"

class USizeBox;
class UVerticalBox;
class UButton;
class UTextBlock;
class UProgressBar;
class UUserWidget;
class AMissionManager;
struct FMissionRuntime; // forward (���Ǵ� cpp���� include)


/**
 * �������� �����̵� ��/�ƿ��Ǵ� �̼� �г�
 * - WBP_MissionPanel�� �� Ŭ������ �θ�� ����
 * - �ʼ� ���� �̸�: MissionList(VerticalBox), BtnClose(Button)
 * - �ʼ� �ִ� �̸�: SlideIn, SlideOut (UWidgetAnimation)
 * - ��Ʈ�� ����(WBP_MissionEntry): Text_Desc, Text_Progress, Progress(����)
 */

UCLASS()
class UESNOWWORLD_API UMissionPanel : public UUserWidget
{
	GENERATED_BODY()
	

public:
    UFUNCTION(BlueprintCallable) 
    void InitializeWithManager(AMissionManager* InManager);
    UFUNCTION(BlueprintCallable) 
    void ShowPanel();
    UFUNCTION(BlueprintCallable) 
    void HidePanel();
    UFUNCTION(BlueprintCallable) 
    void TogglePanel();
    UFUNCTION(BlueprintCallable) 
    void SetPanelWidthFraction(float Fraction = 0.3333f); // ȭ�� ���� ����(�⺻ 1/3)

protected:
    virtual void NativeConstruct() override;

    // ���� ���ε�
    UPROPERTY(meta = (BindWidget)) 
    USizeBox* PanelRoot = nullptr;   // �� ���� �г� ��Ʈ
    UPROPERTY(meta = (BindWidget)) 
    UVerticalBox* MissionList = nullptr;   // �� ����Ʈ �����̳�
    UPROPERTY(meta = (BindWidgetOptional)) 
    UButton* BtnClose = nullptr;

    UPROPERTY(EditAnywhere, Category = "Mission") 
    TSubclassOf<UUserWidget> MissionEntryClass;

    // ��Ÿ��
    UPROPERTY(Transient) 
    TMap<int32, UUserWidget*> EntryMap;
    UPROPERTY(Transient) 
    AMissionManager* Manager = nullptr;

    // �̺�Ʈ ���ε�
    UFUNCTION() void OnMissionUpdated(int32 MissionID, const FMissionRuntime& Runtime);
    UFUNCTION() void OnAllMissionsCompleted();

private:
    void BuildInitialEntries();
    void UpdateEntryWidget(UUserWidget* Entry, const FMissionRuntime& Runtime);
    void RecalculateOverallProgress(); // �ʿ� ������ ����ֵ� OK

//public:
//    /** Manager ���� + �ʱ� ��Ʈ�� ���� + ��������Ʈ ���ε� */
//    UFUNCTION(BlueprintCallable, Category = "Mission")
//    void InitializeWithManager(AMissionManager* InManager);
//
//    /** ����/�ݱ� ��ۿ� ���� �Լ� */
//    UFUNCTION(BlueprintCallable, Category = "Mission|UI")
//    void OpenPanel();
//
//    UFUNCTION(BlueprintCallable, Category = "Mission|UI")
//    void ClosePanel();
//
//protected:
//    virtual void NativeConstruct() override;
//
//    /** ����Ʈ �ڽ� (WBP���� BindWidget) */
//    UPROPERTY(meta = (BindWidget))
//    UVerticalBox* MissionList = nullptr;
//
//    /** �ݱ� ��ư (����: ������ �ڵ� ���ε���) */
//    UPROPERTY(meta = (BindWidgetOptional))
//    UButton* BtnClose = nullptr;
//
//    /** �����̵� �ִϸ��̼ǵ� (WBP�� �ִ� �̸��� ��ġ�ؾ� �ڵ� ���ε���) */
//    UPROPERTY(meta = (BindWidgetAnim), Transient)
//    UWidgetAnimation* SlideIn = nullptr;
//
//    UPROPERTY(meta = (BindWidgetAnim), Transient)
//    UWidgetAnimation* SlideOut = nullptr;
//
//    /** ��Ʈ�� ���� Ŭ���� (WBP_MissionEntry) */
//    UPROPERTY(EditAnywhere, Category = "Mission")
//    TSubclassOf<UUserWidget> MissionEntryClass;
//
//    /** �̼�ID �� ��Ʈ�� ���� ���� */
//    UPROPERTY(Transient)
//    TMap<int32, UUserWidget*> EntryMap;
//
//    /** ���� ������� Manager ������ */
//    UPROPERTY(Transient)
//    AMissionManager* Manager = nullptr;
//
//    /** ��������Ʈ ����: ���൵ ���� */
//    UFUNCTION()
//    void OnMissionUpdated(int32 MissionID, const FMissionRuntime& Runtime);
//
//    /** ��������Ʈ ����: ���� �Ϸ� */
//    UFUNCTION()
//    void OnAllMissionsCompleted();
//
//    UFUNCTION()
//    void OnSlideOutFinished();
//
//private:
//
//    /** ��Ʈ�� ����/�ʱ�ȭ */
//    void BuildInitialEntries();
//
//    /** ��Ʈ�� ���� ���� �ʵ� ����(Text_Desc, Text_Progress, Progress) */
//    void UpdateEntryWidget(UUserWidget* Entry, const FMissionRuntime& Runtime);
};
