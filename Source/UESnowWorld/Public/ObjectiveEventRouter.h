// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "QuestTypes.h"  
#include "ObjectiveEventRouter.generated.h"

/**
* 이벤트 신호 라우터(통신 허브 역할)

게임 월드 전역에서 발생하는 ‘행동 이벤트’(ObjectiveEvent) 들을
브로드캐스트로 전달해주는 시스템.
*/

USTRUCT(BlueprintType)
struct FObjectiveEvent
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) EObjectiveType Type = EObjectiveType::CustomEvent;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName TargetKey = NAME_None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 DeltaCount = 1;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) AActor* Source = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectiveEvent, const FObjectiveEvent&, Event);


UCLASS()
class UESNOWWORLD_API UObjectiveEventRouter : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
    UPROPERTY(BlueprintAssignable, Category = "Quest")
    FOnObjectiveEvent OnObjectiveEvent;

    UFUNCTION(BlueprintCallable, Category = "Quest")
    void BroadcastObjectiveEvent(const FObjectiveEvent& Evt) { OnObjectiveEvent.Broadcast(Evt); }

    // (옵션) BP에서 쉽게 얻도록 헬퍼
    UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Quest")
    static UObjectiveEventRouter* Get(const UObject* WorldContextObject)
    {
        if (!WorldContextObject) return nullptr;
        if (const UWorld* W = WorldContextObject->GetWorld())
            return W->GetSubsystem<UObjectiveEventRouter>();
        return nullptr;
    }
};
