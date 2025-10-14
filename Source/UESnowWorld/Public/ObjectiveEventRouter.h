// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "QuestTypes.h"  
#include "ObjectiveEventRouter.generated.h"

/**
* �̺�Ʈ ��ȣ �����(��� ��� ����)

���� ���� �������� �߻��ϴ� ���ൿ �̺�Ʈ��(ObjectiveEvent) ����
��ε�ĳ��Ʈ�� �������ִ� �ý���.
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

    // (�ɼ�) BP���� ���� �򵵷� ����
    UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Quest")
    static UObjectiveEventRouter* Get(const UObject* WorldContextObject)
    {
        if (!WorldContextObject) return nullptr;
        if (const UWorld* W = WorldContextObject->GetWorld())
            return W->GetSubsystem<UObjectiveEventRouter>();
        return nullptr;
    }
};
