// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestSubsystem.h"
#include "Engine/Engine.h"
#include "QuestData.h"

void UQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (UWorld* W = GetWorld())
    {
        if (UObjectiveEventRouter* R = W->GetSubsystem<UObjectiveEventRouter>())
        {
            // Dynamic multicast → AddDynamic + UFUNCTION 핸들러
            R->OnObjectiveEvent.AddDynamic(this, &UQuestSubsystem::HandleObjectiveEvent);
        }
    }
}

void UQuestSubsystem::StartQuest(FName QuestId)
{
	if (!QuestTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("[QuestSubsystem] QuestTable is NULL!"));
		return;
	}

	FString Context;
	const FQuestRow* Row = QuestTable->FindRow<FQuestRow>(QuestId, Context);

	if (Row)
	{
		// FText → ToString()
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1, 3.f, FColor::Red,
				FString::Printf(TEXT("[QuestSubsystem] StartQuest: %s (%s)"),
					*QuestId.ToString(),
					*Row->Title.ToString()  // ★ 여기
				)
			);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[QuestSubsystem] QuestId not found: %s"), *QuestId.ToString());
	}
}

void UQuestSubsystem::DebugPrintQuest(FName QuestId)
{
    if (!QuestTable)
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("QuestTable is NULL!"));
        return;
    }

    FString Context;
    const FQuestRow* Row = QuestTable->FindRow<FQuestRow>(QuestId, Context);
    if (Row)
    {
        // FName/FText → ToString(), FString은 * 로 전달
        const FString TypeStr = UEnum::GetValueAsString(Row->Type); // FString
        const FString Msg = FString::Printf(
            TEXT("==== Quest Info ====\nID: %s\nTitle: %s\nType: %s\nDesc: %s"),
            *Row->QuestId.ToString(),     //  FName
            *Row->Title.ToString(),       //  FText
            *TypeStr,                     //  FString
            *Row->Description.ToString()  //  FText
        );

        if (GEngine)
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, Msg);
    }
    else
    {
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(
                -1, 3.f, FColor::Red,
                FString::Printf(TEXT("Quest not found: %s"), *QuestId.ToString())
            );
    }
}

UQuestSubsystem* UQuestSubsystem::Get(const UObject* WorldContextObject)
{
    if (!WorldContextObject) return nullptr;
    if (const UWorld* World = WorldContextObject->GetWorld())
        if (UGameInstance* GI = World->GetGameInstance())
            return GI->GetSubsystem<UQuestSubsystem>();
    return nullptr;
}

void UQuestSubsystem::HandleObjectiveEvent(const FObjectiveEvent& E)
{
    // 일단 디버그 출력
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1, 3.f, FColor::Cyan,
            FString::Printf(TEXT("[QuestEvt] Type=%d  Key=%s  +%d"),
                (int32)E.Type, *E.TargetKey.ToString(), E.DeltaCount));
    }

    // TODO: 여기서 현재 활성 목표와 매칭 → 진행도 갱신 로직 연결
}
