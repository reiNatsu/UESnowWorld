#pragma once
#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Perception/AIPerceptionTypes.h"
#include "Components/BoxComponent.h"
#include "CHREnemy.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UCapsuleComponent;
class UAnimMontage;
class UAnimSequenceBase;
class UBoxComponent;


UENUM(BlueprintType)
enum class EPatrolMode : uint8
{
    None        UMETA(DisplayName = "None"),
    Patrol      UMETA(DisplayName = "Patrol"),       // 기존: 반경 순찰(랜덤/원형 등)
    LinePatrol  UMETA(DisplayName = "LinePatrol")    // 새로 추가: 정면 직선 왕복
};

// 가중치가 있는 애니메이션 옵션(몽타주 또는 단일 시퀀스 중 하나를 사용)
USTRUCT(BlueprintType)
struct FWeightedAnimOption
{
    GENERATED_BODY()

    // 둘 중 하나만 채워도 되고, 둘 다 비워두면 무시됩니다.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
    UAnimMontage* Montage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
    UAnimSequenceBase* Sequence = nullptr;

    // 확률 가중치. 전체 합에서 이 값의 비율로 뽑힘(정규화 불필요)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim", meta = (ClampMin = "0.0"))
    float Weight = 1.f;

    // 재생 배속(몽타주/시퀀스 공통)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim", meta = (ClampMin = "0.05"))
    float PlayRate = 1.f;
};

UCLASS()
class UESNOWWORLD_API ACHREnemy : public ABaseCharacter
{
    GENERATED_BODY()

public:
    ACHREnemy();

protected:
    virtual void BeginPlay() override;
    virtual void PossessedBy(AController* NewController) override;

    // === Perception(Sight) ===
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
    UAIPerceptionComponent* Perception = nullptr;

    UPROPERTY()
    UAISenseConfig_Sight* SightConfig = nullptr;

    UPROPERTY(EditAnywhere, Category = "AI|Sight") float SightRadius = 2000.f;
    UPROPERTY(EditAnywhere, Category = "AI|Sight") float LoseSightRadius = 2200.f;
    UPROPERTY(EditAnywhere, Category = "AI|Sight") float PeripheralVision = 85.f;



    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Patrol|Line")
    float LinePatrolWaitTime = 0.5f;      // 끝점에서 잠깐 대기
    // 스폰 기준점/방향
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Patrol|Runtime")
    FVector SpawnLocation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Patrol|Runtime")
    FVector SpawnForward;

    

    // === 공격 범위(캡슐) ===
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    UCapsuleComponent* AttackArea = nullptr;
    
    UPROPERTY(EditAnywhere, Category = "Combat") float AttackCapsuleHalfHeight = 100.f;
    UPROPERTY(EditAnywhere, Category = "Combat") float AttackCapsuleRadius = 120.f;
    UPROPERTY(EditAnywhere, Category = "Combat") UAnimMontage* AttackMontage = nullptr;
    UPROPERTY(EditAnywhere, Category = "Combat") float AttackCooldown = 1.5f;

    float LastAttackTime = -10000.f;

    // Perception 콜백
    UFUNCTION() void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

    // 공격 범위 콜백
    UFUNCTION() void OnAttackAreaBegin(UPrimitiveComponent* Comp, AActor* Other,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);
    UFUNCTION() void OnAttackAreaEnd(UPrimitiveComponent* Comp, AActor* Other,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    // === LinePatrol 유틸 ===
    FVector GetLinePatrolOutPoint() const;
    UFUNCTION(BlueprintPure, Category = "AI|Patrol|Line")
    FVector GetCurrentLineTarget() const;

    void FlipLineDirectionAfterDelay();

public:

    // --- 공격 프리셋(에디터에서 개수/확률 자유롭게) ---
    // 공격 패턴 설정(원하면 에디터에서 조절 가능)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Pattern", meta = (ClampMin = "1"))
    int32 NormalQuotaPerCycle = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Pattern", meta = (ClampMin = "1"))
    int32 SpecialQuotaPerCycle = 2;

    // 현재 사이클 상태
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Pattern")
    int32 NormalLeft = 5;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Pattern")
    int32 SpecialLeft = 2;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Pattern")
    bool bSpecialPhase = false; // false: 일반 공격 페이즈, true: 특수 공격 페이즈

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Attacks")
    TArray<FWeightedAnimOption> NormalAttacks;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Attacks")
    TArray<FWeightedAnimOption> SpecialAttacks;

    // 쿨다운은 공용으로 계속 쓰되, 원하면 별도로 분리 가능
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Attacks", meta = (ClampMin = "0.0"))
    float NormalAttackCooldown = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Attacks", meta = (ClampMin = "0.0"))
    float SpecialAttackCooldown = 3.0f;

    // BP/BTTask에서 직접 호출하기 편한 함수
    UFUNCTION(BlueprintCallable, Category = "Combat|Attacks")
    bool TryNormalAttack();

    UFUNCTION(BlueprintCallable, Category = "Combat|Attacks")
    bool TrySpecialAttack();

protected:
    // 내부 유틸
    int32 PickWeightedIndex(const TArray<FWeightedAnimOption>& Options) const;
    bool PlayWeightedAnim(const FWeightedAnimOption& Opt);

    float LastNormalAttackTime = -10000.f;
    float LastSpecialAttackTime = -10000.f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseEnemyData")
    FName _characterKey;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseEnemyData")
    class UDataTable* _pickupTable;

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Patrol")
    EPatrolMode PatrolMode = EPatrolMode::None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Patrol|Line")
    float LinePatrolDistance = 800.f;     // 정면으로 갈 거리
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    bool bDoPatrol = true;
    // 내부 상태(왕복 방향)
    bool bGoingOut = true;     // true면 바깥(정면 끝점)으로 가는 중
    FTimerHandle LineWaitTimer;


    UFUNCTION(BlueprintCallable, Category = "Combat") bool TryAttackTarget(AActor* Target);
    bool HasChaseTarget() const;

    // === BT(블루프린트) 보조 함수 ===
    UFUNCTION(BlueprintCallable, Category = "AI|Blackboard")
    void RefreshPatrolTargetOnBB();

    UFUNCTION(BlueprintCallable, Category = "AI|Patrol|Line")
    void ToggleLinePatrolDirection();

    UFUNCTION(BlueprintCallable, Category = "AI|Patrol")
    void SetPatrolMode(EPatrolMode NewMode);


    /** 애님노티파이에서 켜기/끄기 위해 BP에서도 호출 가능 + BP 오버라이드도 지원 */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
    void EnableHitbox();
    virtual void EnableHitbox_Implementation();

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
    void DisableHitbox();
    virtual void DisableHitbox_Implementation();

protected:
        /** 히트박스로 쓸 박스들(BP_CHREnemy 디테일에서 Right/LeftAttackBox 끌어다 넣기) */
        /*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack|Hitbox", meta = (AllowPrivateAccess = "true"))
        TArray<UBoxComponent*> AttackBoxes;*/

        // 히트박스 on/off 플래그 (BP에서 읽고/세팅할 수 있게)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack|Hitbox", meta = (AllowPrivateAccess = "true"))
    bool bHitBoxActive = false;

    // 이번 스윙에 이미 맞은 Actor 리스트 (BP에서 읽고/수정 가능해야 AddUnique/Contains에 쓸 수 있음)
    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Attack|Hitbox", meta = (AllowPrivateAccess = "true"))
    TArray<AActor*> HitActors;

    // 손 히트박스 컴포넌트 (BP 컴포넌트 트리에서 보이게)
    // CHREnemy.h (확인)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack|Hitbox", meta = (AllowPrivateAccess = "true"))
    UBoxComponent* AttackBoxRef = nullptr;


        /** 이 공격의 기본 데미지(원하면 BP에서 개별 세팅) */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
        float AttackDamage = 20.f;
        /** 박스 오버랩 콜백 */
        UFUNCTION()
        void OnAttackBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
            bool bFromSweep, const FHitResult& SweepResult);
};

