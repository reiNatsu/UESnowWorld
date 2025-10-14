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
struct FMissionRuntime; // forward (정의는 cpp에서 include)


/**
 * 좌측에서 슬라이드 인/아웃되는 미션 패널
 * - WBP_MissionPanel에 이 클래스를 부모로 지정
 * - 필수 위젯 이름: MissionList(VerticalBox), BtnClose(Button)
 * - 필수 애니 이름: SlideIn, SlideOut (UWidgetAnimation)
 * - 엔트리 위젯(WBP_MissionEntry): Text_Desc, Text_Progress, Progress(선택)
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
    void SetPanelWidthFraction(float Fraction = 0.3333f); // 화면 폭의 비율(기본 1/3)

protected:
    virtual void NativeConstruct() override;

    // 위젯 바인딩
    UPROPERTY(meta = (BindWidget)) 
    USizeBox* PanelRoot = nullptr;   // ← 좌측 패널 루트
    UPROPERTY(meta = (BindWidget)) 
    UVerticalBox* MissionList = nullptr;   // ← 리스트 컨테이너
    UPROPERTY(meta = (BindWidgetOptional)) 
    UButton* BtnClose = nullptr;

    UPROPERTY(EditAnywhere, Category = "Mission") 
    TSubclassOf<UUserWidget> MissionEntryClass;

    // 런타임
    UPROPERTY(Transient) 
    TMap<int32, UUserWidget*> EntryMap;
    UPROPERTY(Transient) 
    AMissionManager* Manager = nullptr;

    // 이벤트 바인딩
    UFUNCTION() void OnMissionUpdated(int32 MissionID, const FMissionRuntime& Runtime);
    UFUNCTION() void OnAllMissionsCompleted();

private:
    void BuildInitialEntries();
    void UpdateEntryWidget(UUserWidget* Entry, const FMissionRuntime& Runtime);
    void RecalculateOverallProgress(); // 필요 없으면 비워둬도 OK

//public:
//    /** Manager 주입 + 초기 엔트리 생성 + 델리게이트 바인딩 */
//    UFUNCTION(BlueprintCallable, Category = "Mission")
//    void InitializeWithManager(AMissionManager* InManager);
//
//    /** 열기/닫기 토글용 공개 함수 */
//    UFUNCTION(BlueprintCallable, Category = "Mission|UI")
//    void OpenPanel();
//
//    UFUNCTION(BlueprintCallable, Category = "Mission|UI")
//    void ClosePanel();
//
//protected:
//    virtual void NativeConstruct() override;
//
//    /** 리스트 박스 (WBP에서 BindWidget) */
//    UPROPERTY(meta = (BindWidget))
//    UVerticalBox* MissionList = nullptr;
//
//    /** 닫기 버튼 (선택: 있으면 자동 바인딩됨) */
//    UPROPERTY(meta = (BindWidgetOptional))
//    UButton* BtnClose = nullptr;
//
//    /** 슬라이드 애니메이션들 (WBP의 애니 이름과 일치해야 자동 바인딩됨) */
//    UPROPERTY(meta = (BindWidgetAnim), Transient)
//    UWidgetAnimation* SlideIn = nullptr;
//
//    UPROPERTY(meta = (BindWidgetAnim), Transient)
//    UWidgetAnimation* SlideOut = nullptr;
//
//    /** 엔트리 위젯 클래스 (WBP_MissionEntry) */
//    UPROPERTY(EditAnywhere, Category = "Mission")
//    TSubclassOf<UUserWidget> MissionEntryClass;
//
//    /** 미션ID → 엔트리 위젯 매핑 */
//    UPROPERTY(Transient)
//    TMap<int32, UUserWidget*> EntryMap;
//
//    /** 현재 사용중인 Manager 포인터 */
//    UPROPERTY(Transient)
//    AMissionManager* Manager = nullptr;
//
//    /** 델리게이트 수신: 진행도 갱신 */
//    UFUNCTION()
//    void OnMissionUpdated(int32 MissionID, const FMissionRuntime& Runtime);
//
//    /** 델리게이트 수신: 전부 완료 */
//    UFUNCTION()
//    void OnAllMissionsCompleted();
//
//    UFUNCTION()
//    void OnSlideOutFinished();
//
//private:
//
//    /** 엔트리 생성/초기화 */
//    void BuildInitialEntries();
//
//    /** 엔트리 위젯 내부 필드 갱신(Text_Desc, Text_Progress, Progress) */
//    void UpdateEntryWidget(UUserWidget* Entry, const FMissionRuntime& Runtime);
};
