#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PoolingObject.h"


#include "ScriptCharacter.generated.h"


DECLARE_DELEGATE(DFuncSample);
DECLARE_DYNAMIC_DELEGATE_OneParam(FDDynamicSample, int32, intvalue);

UCLASS()
class UESNOWWORLD_API AScriptCharacter : public ACharacter , public IPoolingObject
{
	GENERATED_BODY() 
	
public:
	UPROPERTY(BlueprintReadWrite)
	float _sampleFloatValue;

	UFUNCTION(BlueprintCallable)
	void SampleFunc();

	void OnlyScriptFuncton();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AScriptCharacter> _sampleScriptCharacter;

	DFuncSample DFunc;
	UPROPERTY(BlueprintReadWrite)
	FDDynamicSample DynamicSample;
public:
	// Sets default values for this character's properties
	AScriptCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
