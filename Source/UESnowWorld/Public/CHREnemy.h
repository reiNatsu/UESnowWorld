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
    Patrol      UMETA(DisplayName = "Patrol"),       // ����: �ݰ� ����(����/���� ��)
    LinePatrol  UMETA(DisplayName = "LinePatrol")    // ���� �߰�: ���� ���� �պ�
};

// ����ġ�� �ִ� �ִϸ��̼� �ɼ�(��Ÿ�� �Ǵ� ���� ������ �� �ϳ��� ���)
USTRUCT(BlueprintType)
struct FWeightedAnimOption
{
    GENERATED_BODY()

    // �� �� �ϳ��� ä���� �ǰ�, �� �� ����θ� ���õ˴ϴ�.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
    UAnimMontage* Montage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
    UAnimSequenceBase* Sequence = nullptr;

    // Ȯ�� ����ġ. ��ü �տ��� �� ���� ������ ����(����ȭ ���ʿ�)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim", meta = (ClampMin = "0.0"))
    float Weight = 1.f;

    // ��� ���(��Ÿ��/������ ����)
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
    float LinePatrolWaitTime = 0.5f;      // �������� ��� ���
    // ���� ������/����
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Patrol|Runtime")
    FVector SpawnLocation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Patrol|Runtime")
    FVector SpawnForward;

    

    // === ���� ����(ĸ��) ===
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    UCapsuleComponent* AttackArea = nullptr;
    
    UPROPERTY(EditAnywhere, Category = "Combat") float AttackCapsuleHalfHeight = 100.f;
    UPROPERTY(EditAnywhere, Category = "Combat") float AttackCapsuleRadius = 120.f;
    UPROPERTY(EditAnywhere, Category = "Combat") UAnimMontage* AttackMontage = nullptr;
    UPROPERTY(EditAnywhere, Category = "Combat") float AttackCooldown = 1.5f;

    float LastAttackTime = -10000.f;

    // Perception �ݹ�
    UFUNCTION() void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

    // ���� ���� �ݹ�
    UFUNCTION() void OnAttackAreaBegin(UPrimitiveComponent* Comp, AActor* Other,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);
    UFUNCTION() void OnAttackAreaEnd(UPrimitiveComponent* Comp, AActor* Other,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    // === LinePatrol ��ƿ ===
    FVector GetLinePatrolOutPoint() const;
    UFUNCTION(BlueprintPure, Category = "AI|Patrol|Line")
    FVector GetCurrentLineTarget() const;

    void FlipLineDirectionAfterDelay();

public:

    // --- ���� ������(�����Ϳ��� ����/Ȯ�� �����Ӱ�) ---
    // ���� ���� ����(���ϸ� �����Ϳ��� ���� ����)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Pattern", meta = (ClampMin = "1"))
    int32 NormalQuotaPerCycle = 5;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Pattern", meta = (ClampMin = "1"))
    int32 SpecialQuotaPerCycle = 2;

    // ���� ����Ŭ ����
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Pattern")
    int32 NormalLeft = 5;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Pattern")
    int32 SpecialLeft = 2;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat|Pattern")
    bool bSpecialPhase = false; // false: �Ϲ� ���� ������, true: Ư�� ���� ������

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Attacks")
    TArray<FWeightedAnimOption> NormalAttacks;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat|Attacks")
    TArray<FWeightedAnimOption> SpecialAttacks;

    // ��ٿ��� �������� ��� ����, ���ϸ� ������ �и� ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Attacks", meta = (ClampMin = "0.0"))
    float NormalAttackCooldown = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Attacks", meta = (ClampMin = "0.0"))
    float SpecialAttackCooldown = 3.0f;

    // BP/BTTask���� ���� ȣ���ϱ� ���� �Լ�
    UFUNCTION(BlueprintCallable, Category = "Combat|Attacks")
    bool TryNormalAttack();

    UFUNCTION(BlueprintCallable, Category = "Combat|Attacks")
    bool TrySpecialAttack();

protected:
    // ���� ��ƿ
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
    float LinePatrolDistance = 800.f;     // �������� �� �Ÿ�
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
    bool bDoPatrol = true;
    // ���� ����(�պ� ����)
    bool bGoingOut = true;     // true�� �ٱ�(���� ����)���� ���� ��
    FTimerHandle LineWaitTimer;


    UFUNCTION(BlueprintCallable, Category = "Combat") bool TryAttackTarget(AActor* Target);
    bool HasChaseTarget() const;

    // === BT(�������Ʈ) ���� �Լ� ===
    UFUNCTION(BlueprintCallable, Category = "AI|Blackboard")
    void RefreshPatrolTargetOnBB();

    UFUNCTION(BlueprintCallable, Category = "AI|Patrol|Line")
    void ToggleLinePatrolDirection();

    UFUNCTION(BlueprintCallable, Category = "AI|Patrol")
    void SetPatrolMode(EPatrolMode NewMode);


    /** �ִԳ�Ƽ���̿��� �ѱ�/���� ���� BP������ ȣ�� ���� + BP �������̵嵵 ���� */
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
    void EnableHitbox();
    virtual void EnableHitbox_Implementation();

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Combat")
    void DisableHitbox();
    virtual void DisableHitbox_Implementation();

protected:
        /** ��Ʈ�ڽ��� �� �ڽ���(BP_CHREnemy �����Ͽ��� Right/LeftAttackBox ����� �ֱ�) */
        /*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack|Hitbox", meta = (AllowPrivateAccess = "true"))
        TArray<UBoxComponent*> AttackBoxes;*/

        // ��Ʈ�ڽ� on/off �÷��� (BP���� �а�/������ �� �ְ�)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack|Hitbox", meta = (AllowPrivateAccess = "true"))
    bool bHitBoxActive = false;

    // �̹� ������ �̹� ���� Actor ����Ʈ (BP���� �а�/���� �����ؾ� AddUnique/Contains�� �� �� ����)
    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Attack|Hitbox", meta = (AllowPrivateAccess = "true"))
    TArray<AActor*> HitActors;

    // �� ��Ʈ�ڽ� ������Ʈ (BP ������Ʈ Ʈ������ ���̰�)
    // CHREnemy.h (Ȯ��)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack|Hitbox", meta = (AllowPrivateAccess = "true"))
    UBoxComponent* AttackBoxRef = nullptr;


        /** �� ������ �⺻ ������(���ϸ� BP���� ���� ����) */
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
        float AttackDamage = 20.f;
        /** �ڽ� ������ �ݹ� */
        UFUNCTION()
        void OnAttackBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
            bool bFromSweep, const FHitResult& SweepResult);
};

