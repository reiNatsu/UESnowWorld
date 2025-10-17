#include "CHREnemy.h"
#include "AI/EnemyTypes.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "Animation/AnimSequenceBase.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

static UBlackboardComponent* GetBB(AAIController* AIC)
{
    return AIC ? AIC->GetBlackboardComponent() : nullptr;
}
// ��ƿ �Լ�
/*static bool ProjectToNav(UWorld* World, const FVector& In, FVector& Out)
{
    if (UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World))
    {
        FNavLocation NL;
        if (Nav->ProjectPointToNavigation(In, NL, FVector(200, 200, 400)))
        {
            Out = NL.Location;
            return true;
        }
    }
    Out = In;
    return false;
}*/

ACHREnemy::ACHREnemy()
{
    PrimaryActorTick.bCanEverTick = false;
    Team = ETeamType::Enemy;

    // Perception
    Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

    SightConfig->SightRadius = SightRadius;
    SightConfig->LoseSightRadius = LoseSightRadius;
    SightConfig->PeripheralVisionAngleDegrees = PeripheralVision;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

    Perception->ConfigureSense(*SightConfig);
    Perception->SetDominantSense(UAISense_Sight::StaticClass());

    // Attack capsule
    AttackArea = CreateDefaultSubobject<UCapsuleComponent>(TEXT("AttackArea"));
    AttackArea->SetupAttachment(GetMesh());
    AttackArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    AttackArea->SetCollisionObjectType(ECC_WorldDynamic);
    AttackArea->SetCollisionResponseToAllChannels(ECR_Ignore);
    AttackArea->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    AttackArea->SetGenerateOverlapEvents(true);
    AttackArea->SetCapsuleHalfHeight(AttackCapsuleHalfHeight);
    AttackArea->SetCapsuleRadius(AttackCapsuleRadius);
    AttackArea->SetRelativeLocation(FVector(80.f, 0.f, 60.f)); // ����/���� �ణ

    // �� ��Ʈ�ڽ�(�� �ݵ�� �����ڿ���!)
    //RightAttackBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightAttackBox"));
    //RightAttackBox->SetupAttachment(GetMesh()); // �ʿ��ϸ� ���ϸ� ����
    //// RightAttackBox->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
    //RightAttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); // �⺻ OFF
    //RightAttackBox->SetGenerateOverlapEvents(true);
    //RightAttackBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    //RightAttackBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    
    NormalLeft = NormalQuotaPerCycle;
    SpecialLeft = SpecialQuotaPerCycle;

   
}

// -------------------- ���� Ʈ���� Start--------------------
bool ACHREnemy::TryNormalAttack()
{
    const float Now = GetWorld()->GetTimeSeconds();
    if (Now - LastNormalAttackTime < NormalAttackCooldown) return false;

    const int32 Idx = PickWeightedIndex(NormalAttacks);
    if (Idx == INDEX_NONE) return false;

    if (PlayWeightedAnim(NormalAttacks[Idx]))
    {
        LastNormalAttackTime = Now;
        return true;
    }
    return false;
}

bool ACHREnemy::TrySpecialAttack()
{
    const float Now = GetWorld()->GetTimeSeconds();
    if (Now - LastSpecialAttackTime < SpecialAttackCooldown) return false;

    const int32 Idx = PickWeightedIndex(SpecialAttacks);
    if (Idx == INDEX_NONE) return false;

    if (PlayWeightedAnim(SpecialAttacks[Idx]))
    {
        LastSpecialAttackTime = Now;
        return true;
    }
    return false;
}
// -------------------- ���� Ʈ���� End--------------------
int32 ACHREnemy::PickWeightedIndex(const TArray<FWeightedAnimOption>& Options) const
{
    double Sum = 0.0;
    for (const auto& O : Options)
    {
        if ((O.Montage || O.Sequence) && O.Weight > 0.f)
        {
            Sum += O.Weight;
        }
    }
    if (Sum <= 0.0) return INDEX_NONE;

    const double R = FMath::FRandRange(0.0, Sum);
    double Acc = 0.0;
    for (int32 i = 0; i < Options.Num(); ++i)
    {
        const auto& O = Options[i];
        if (!(O.Montage || O.Sequence) || O.Weight <= 0.f) continue;

        Acc += O.Weight;
        if (R <= Acc) return i;
    }
    return INDEX_NONE;
   // return int32();
}

bool ACHREnemy::PlayWeightedAnim(const FWeightedAnimOption& Opt)
{
    if (Opt.Montage)
    {
        PlayAnimMontage(Opt.Montage, Opt.PlayRate);
        return true;
    }
    if (Opt.Sequence)
    {
        if (USkeletalMeshComponent* sMesh = GetMesh())
        {
            sMesh->PlayAnimation(Opt.Sequence, /*bLoop=*/false);
            sMesh->SetPlayRate(Opt.PlayRate); // �ʿ��
            return true;
        }
    }
    return false;
}

void ACHREnemy::BeginPlay()
{
    Super::BeginPlay();



    // ���� ���� ���� ��� (LinePatrol�� ���)
    SpawnLocation = GetActorLocation();
    SpawnForward = GetActorForwardVector().GetSafeNormal();

    if (Perception)
    {
        Perception->OnTargetPerceptionUpdated.AddDynamic(this, &ACHREnemy::OnPerceptionUpdated);
    }
    if (AttackArea)
    {
        AttackArea->OnComponentBeginOverlap.AddDynamic(this, &ACHREnemy::OnAttackAreaBegin);
        AttackArea->OnComponentEndOverlap.AddDynamic(this, &ACHREnemy::OnAttackAreaEnd);
    }

    // �� ��Ʈ�ڽ� Overlap ���ε�
    if (AttackBoxRef)
    {
        // �⺻�� ����
        AttackBoxRef->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        AttackBoxRef->SetGenerateOverlapEvents(true);

        // �ʿ信 �°� �浹 ���� ����(������Ʈ ��Ģ�� �°� ����)
        // �ּ��� Pawn�� Overlap �ϵ���:
        AttackBoxRef->SetCollisionResponseToAllChannels(ECR_Ignore);
        AttackBoxRef->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

        // ���Ͽ� ���̰� ������ BP �ʿ��� Attach �ص� �ǰ�(����),
        // C++���� �����Ϸ���:
        // RightAttackBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("hand_rSocket"));

        // ��������Ʈ
        AttackBoxRef->OnComponentBeginOverlap.AddDynamic(this, &ACHREnemy::OnAttackBoxBeginOverlap);
    }

    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

    // BP���� AttackBoxes �迭�� �־��� �ڽ����� �⺻ ���� + ��������Ʈ ���ε�
    //for (UBoxComponent* Box : AttackBoxes)
    //{
    //    if (!Box) continue;

    //    Box->SetGenerateOverlapEvents(true);
    //    Box->SetCollisionEnabled(ECollisionEnabled::NoCollision); // �⺻�� ����
    //    Box->OnComponentBeginOverlap.AddDynamic(this, &ACHREnemy::OnAttackBoxBeginOverlap);
    //}
    
    if (AbilitySystem && GetController())
    {
        AbilitySystem->InitAbilityActorInfo(GetController(), this);
    }
   
}

void ACHREnemy::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    if (AbilitySystem && NewController)
    {
        // AI�� ���� AIController�� Owner�� ���
        AbilitySystem->InitAbilityActorInfo(NewController, this);

        // (����) �������� �⺻ �ɷ�/���� ����
        // if (HasAuthority())
        // {
        //     // AbilitySystem->GiveAbility(...);
        //     // AbilitySystem->ApplyGameplayEffectToSelf(...);
        // }
    }

    /*if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
    {
        ASC->InitAbilityActorInfo(NewController, this);
    }*/

    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("0"));
    //if (AAIController* AIC = Cast<AAIController>(NewController))
    //{
    //    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("1"));
    //  
    //    if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
    //    {
    //        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("2"));
    //        BB->SetValueAsBool(EnemyBB::DoPatrol, bDoPatrol);
    //        BB->SetValueAsEnum(EnemyBB::PatrolMode, static_cast<uint8>(PatrolMode));

    //        if (PatrolMode == EPatrolMode::LinePatrol)
    //        {
    //            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("2-2"));
    //            bGoingOut = true;
    //            FVector Target = GetLinePatrolOutPoint();
    //            ProjectToNav(GetWorld(), Target, Target); // Nav ���� ����
    //            BB->SetValueAsVector(EnemyBB::PatrolTarget, Target);

    //            if (GEngine)
    //            {
    //                FString Msg = FString::Printf(TEXT("[Enemy] PatrolTarget set to %s"), *Target.ToString());
    //                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Msg);
    //            }
    //        }
    //        else if (PatrolMode == EPatrolMode::Patrol)
    //        {
    //            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("2-1"));
    //            BB->SetValueAsVector(EnemyBB::PatrolTarget, SpawnLocation);
    //            BB->SetValueAsVector(EnemyBB::TargetPosition, SpawnLocation);
    //        }
    //        else
    //        {
    //            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("2-0"));
    //            BB->ClearValue(EnemyBB::PatrolTarget);
    //        }
    //    }
    //}
}

void ACHREnemy::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    const ABaseCharacter* BC = Cast<ABaseCharacter>(Actor);
    if (!BC || BC->GetTeam() != ETeamType::Player) return;

    if (AAIController* AIC = Cast<AAIController>(GetController()))
    {
        if (UBlackboardComponent* BB = GetBB(AIC))
        {
            if (Stimulus.WasSuccessfullySensed())
            {
                BB->SetValueAsBool(EnemyBB::IsInSight, true);
                BB->SetValueAsObject(EnemyBB::TargetActor, Actor);
                GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
            }
            else
            {
                BB->SetValueAsBool(EnemyBB::IsInSight, false);
                BB->ClearValue(EnemyBB::TargetActor);
                GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
            }
        }
    }
}

void ACHREnemy::OnAttackAreaBegin(UPrimitiveComponent* Comp, AActor* Other,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
    const ABaseCharacter* BC = Cast<ABaseCharacter>(Other);
    if (!BC || BC->GetTeam() != ETeamType::Player) return;

    if (AAIController* AIC = Cast<AAIController>(GetController()))
    {
        if (UBlackboardComponent* BB = GetBB(AIC))
        {
            BB->SetValueAsBool(EnemyBB::InAttackRange, true);
        }
    }
}

void ACHREnemy::OnAttackAreaEnd(UPrimitiveComponent* Comp, AActor* Other,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    const ABaseCharacter* BC = Cast<ABaseCharacter>(Other);
    if (!BC || BC->GetTeam() != ETeamType::Player) return;

    if (AAIController* AIC = Cast<AAIController>(GetController()))
    {
        if (UBlackboardComponent* BB = GetBB(AIC))
        {
            BB->SetValueAsBool(EnemyBB::InAttackRange, false);
        }
    }
}

FVector ACHREnemy::GetLinePatrolOutPoint() const
{
    return SpawnLocation + SpawnForward * LinePatrolDistance;
    //return FVector();
}

FVector ACHREnemy::GetCurrentLineTarget() const
{
    return bGoingOut ? GetLinePatrolOutPoint() : SpawnLocation;
    //return FVector();
}

void ACHREnemy::FlipLineDirectionAfterDelay()
{
    bGoingOut = !bGoingOut;

    // ���� ���̵� �����ϵ���, ��� BB PatrolTarget ����
    if (AAIController* AIC = Cast<AAIController>(GetController()))
    {
        if (UBlackboardComponent* BB = GetBB(AIC))
        {
            const uint8 ModeVal = BB->GetValueAsEnum(EnemyBB::PatrolMode);
            if (ModeVal == static_cast<uint8>(EPatrolMode::LinePatrol))
            {
                BB->SetValueAsVector(EnemyBB::PatrolTarget, GetCurrentLineTarget());
            }
        }
    }
}

bool ACHREnemy::TryAttackTarget(AActor* Target)
{
    if (!Target) return false;

    const float Now = GetWorld()->GetTimeSeconds();
    if (Now - LastAttackTime < AttackCooldown) return false;

    bool bDidAttack = false;

    // 1) ���� ������� ���� �õ�
    if (bSpecialPhase)
    {
        // Ư�� ����
        bDidAttack = TrySpecialAttack();              // �迭 ���(����ġ) ���� ���
        if (!bDidAttack)
        {
            // Ư�� �����Ͱ� ���ų� ���������� �Ϲ����� ����
            bDidAttack = TryNormalAttack();
        }

        if (bDidAttack)
        {
            LastAttackTime = Now;
            // ī��Ʈ �ٿ� & ������ ��ȯ
            if (--SpecialLeft <= 0)
            {
                bSpecialPhase = false;
                NormalLeft = FMath::Max(1, NormalQuotaPerCycle);
                SpecialLeft = FMath::Max(1, SpecialQuotaPerCycle); // ���� Ư�� ����� ���� ����
            }
            return true;
        }
    }
    else
    {
        // �Ϲ� ����
        bDidAttack = TryNormalAttack();
        if (!bDidAttack)
        {
            // �Ϲ� ������ ���ų� ���� �� Ư���� ����
            bDidAttack = TrySpecialAttack();
        }

        if (bDidAttack)
        {
            LastAttackTime = Now;
            // ī��Ʈ �ٿ� & ������ ��ȯ
            if (--NormalLeft <= 0)
            {
                bSpecialPhase = true;
                SpecialLeft = FMath::Max(1, SpecialQuotaPerCycle);
                NormalLeft = FMath::Max(1, NormalQuotaPerCycle); // ���� �Ϲ� ����� ���� ����
            }
            return true;
        }
    }

    // ������� ���� �� �� ����(����� �ִ� ���� ��)
    return false;

    /*if (!AttackMontage || !Target) return false;

    const float Now = GetWorld()->GetTimeSeconds();
    if (Now - LastAttackTime < AttackCooldown) return false;

    PlayAnimMontage(AttackMontage);
    LastAttackTime = Now;
    return true;*/
}

bool ACHREnemy::HasChaseTarget() const
{
    if (const AAIController* AIC = Cast<AAIController>(GetController()))
    {
        if (const UBlackboardComponent* BB = AIC->GetBlackboardComponent())
        {
            return BB->GetValueAsObject(EnemyBB::TargetActor) != nullptr;
        }
    }
    return false;
}

void ACHREnemy::RefreshPatrolTargetOnBB()
{
    if (AAIController* AIC = Cast<AAIController>(GetController()))
    {
        if (UBlackboardComponent* BB = GetBB(AIC))
        {
            if (PatrolMode == EPatrolMode::LinePatrol)
            {
                FVector Target = GetCurrentLineTarget();
                //ProjectToNav(GetWorld(), Target, Target);   //  ����

                 // �� NavMesh ���� ���� (ProjectToNav ��ü)
                if (UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
                {
                    FNavLocation NL;
                    if (Nav->ProjectPointToNavigation(Target, NL, FVector(200.f, 200.f, 400.f)))
                    {
                        Target = NL.Location;
                    }
                }

                BB->SetValueAsVector(EnemyBB::PatrolTarget, Target);
                //BB->SetValueAsVector(EnemyBB::PatrolTarget, GetCurrentLineTarget());
            }
            else if (PatrolMode == EPatrolMode::Patrol)
            {
                // ���������� BT���� TargetPosition�� ������ ��,
                // �ʿ� �� PatrolTarget <- TargetPosition ���� �����ؼ� MoveTo�� ����ص� ��.
                const FVector Cur = BB->GetValueAsVector(EnemyBB::TargetPosition);
                BB->SetValueAsVector(EnemyBB::PatrolTarget, Cur.IsNearlyZero() ? SpawnLocation : Cur);
            }
            else
            {
                BB->ClearValue(EnemyBB::PatrolTarget);
            }
        }
    }
}

void ACHREnemy::ToggleLinePatrolDirection()
{
    // BT Sequence: MoveTo(PatrolTarget) �� Wait(LinePatrolWaitTime) �� ToggleLinePatrolDirection()
    //if (GEngine)
    //{
    //    // Key: -1 = �� �޽���, ���ӽð�: 5��, ����: �ʷ�
    //    GEngine->AddOnScreenDebugMessage(
    //        -1,
    //        5.f,
    //        FColor::Purple,
    //        TEXT("ToggleLinePatrolDirection()CPP")
    //    );
    //}
    
    bGoingOut = !bGoingOut;

    if (AAIController* AIC = Cast<AAIController>(GetController()))
    {
        if (UBlackboardComponent* BB = GetBB(AIC))
        {
            if (static_cast<uint8>(PatrolMode) == static_cast<uint8>(EPatrolMode::LinePatrol))
            {
                FVector Target = GetCurrentLineTarget();
                //ProjectToNav(GetWorld(), Target, Target);   // ����
                 // �� NavMesh ���� ���� (ProjectToNav ��ü)
                if (UNavigationSystemV1* Nav = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
                {
                    FNavLocation NL;
                    if (Nav->ProjectPointToNavigation(Target, NL, FVector(200.f, 200.f, 400.f)))
                    {
                        Target = NL.Location;
                    }
                }

                BB->SetValueAsVector(EnemyBB::PatrolTarget, Target);
            }
            //BB->SetValueAsVector(EnemyBB::PatrolTarget, GetCurrentLineTarget());
        }
    }
    DrawDebugSphere(GetWorld(), GetCurrentLineTarget(), 25.f, 12, FColor::Red, false, 2.f);
}

void ACHREnemy::SetPatrolMode(EPatrolMode NewMode)
{
    PatrolMode = NewMode;

    if (AAIController* AIC = Cast<AAIController>(GetController()))
    {
        if (UBlackboardComponent* BB = GetBB(AIC))
        {
            BB->SetValueAsEnum(EnemyBB::PatrolMode, static_cast<uint8>(PatrolMode));
        }
    }

    // ��� �ٲ�� Ÿ�� �缳��
    RefreshPatrolTargetOnBB();
}

// ====== ��Ʈ�ڽ� on/off (BP���� ȣ�� ����) ======
void ACHREnemy::EnableHitbox_Implementation()
{
    bHitBoxActive = true;
    HitActors.Reset();
    if (AttackBoxRef)
        AttackBoxRef->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    
}
void ACHREnemy::DisableHitbox_Implementation()
{
    bHitBoxActive = false;
    if (AttackBoxRef)
        AttackBoxRef->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// ====== �� ��Ʈ�ڽ� Overlap ======
void ACHREnemy::OnAttackBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!bHitBoxActive) return;
    if (!IsValid(OtherActor) || OtherActor == this) return;

    // (����) Pawn/Character�� ��Ʈ
    //if (!OtherActor->IsA<APawn>()) return;
   
    // Player�� Ÿ�� (BaseCharacter�� ���� ����)
    ABaseCharacter* Victim = Cast<ABaseCharacter>(OtherActor);
    if (!Victim || Victim->GetTeam() != ETeamType::Player) return;

    // �� ���� �ߺ�Ÿ ����
    if (HitActors.Contains(OtherActor)) 
        return;
    
    HitActors.AddUnique(OtherActor);

    // (����) ��/���� ����
    // if (IsSameTeam(OtherActor)) return;

    // ApplyDamage �� Victim.TakeDamage ȣ���
    TSubclassOf<UDamageType> DmgType = UDamageType::StaticClass();
    AController* InstigatorController = GetController();
    UGameplayStatics::ApplyDamage(
        OtherActor,
        AttackDamage,
        InstigatorController,
        this,
        DmgType
    );
    //if (!bHitBoxActive) return;
    //if (!OtherActor || OtherActor == this) return;

    //// �� ���� ���� �ߺ�Ÿ ����
    //if (HitActors.Contains(OtherActor)) return;
    //HitActors.Add(OtherActor);

    //// ������ �ֱ�
    //AController* InstigatorController = GetController();
    //UGameplayStatics::ApplyDamage(OtherActor, AttackDamage, InstigatorController, this, nullptr);
}
