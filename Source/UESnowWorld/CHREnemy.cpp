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
// 유틸 함수
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
    AttackArea->SetRelativeLocation(FVector(80.f, 0.f, 60.f)); // 전방/상향 약간

    // 손 히트박스(※ 반드시 생성자에서!)
    //RightAttackBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RightAttackBox"));
    //RightAttackBox->SetupAttachment(GetMesh()); // 필요하면 소켓명 지정
    //// RightAttackBox->SetupAttachment(GetMesh(), TEXT("hand_rSocket"));
    //RightAttackBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 기본 OFF
    //RightAttackBox->SetGenerateOverlapEvents(true);
    //RightAttackBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    //RightAttackBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    
    NormalLeft = NormalQuotaPerCycle;
    SpecialLeft = SpecialQuotaPerCycle;

   
}

// -------------------- 공격 트리거 Start--------------------
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
// -------------------- 공격 트리거 End--------------------
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
            sMesh->SetPlayRate(Opt.PlayRate); // 필요시
            return true;
        }
    }
    return false;
}

void ACHREnemy::BeginPlay()
{
    Super::BeginPlay();



    // 스폰 기준 정보 기록 (LinePatrol에 사용)
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

    // 손 히트박스 Overlap 바인딩
    if (AttackBoxRef)
    {
        // 기본은 꺼둠
        AttackBoxRef->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        AttackBoxRef->SetGenerateOverlapEvents(true);

        // 필요에 맞게 충돌 응답 설정(프로젝트 규칙에 맞게 조정)
        // 최소한 Pawn과 Overlap 하도록:
        AttackBoxRef->SetCollisionResponseToAllChannels(ECR_Ignore);
        AttackBoxRef->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

        // 소켓에 붙이고 싶으면 BP 쪽에서 Attach 해도 되고(권장),
        // C++에서 강제하려면:
        // RightAttackBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("hand_rSocket"));

        // 델리게이트
        AttackBoxRef->OnComponentBeginOverlap.AddDynamic(this, &ACHREnemy::OnAttackBoxBeginOverlap);
    }

    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

    // BP에서 AttackBoxes 배열에 넣어준 박스들을 기본 설정 + 델리게이트 바인딩
    //for (UBoxComponent* Box : AttackBoxes)
    //{
    //    if (!Box) continue;

    //    Box->SetGenerateOverlapEvents(true);
    //    Box->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 기본은 꺼둠
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
        // AI는 보통 AIController를 Owner로 사용
        AbilitySystem->InitAbilityActorInfo(NewController, this);

        // (선택) 서버에서 기본 능력/스탯 적용
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
    //            ProjectToNav(GetWorld(), Target, Target); // Nav 위로 흡착
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

    // 서비스 없이도 동작하도록, 즉시 BB PatrolTarget 갱신
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

    // 1) 현재 페이즈에서 먼저 시도
    if (bSpecialPhase)
    {
        // 특수 공격
        bDidAttack = TrySpecialAttack();              // 배열 기반(가중치) 선택 재생
        if (!bDidAttack)
        {
            // 특수 데이터가 없거나 실패했으면 일반으로 폴백
            bDidAttack = TryNormalAttack();
        }

        if (bDidAttack)
        {
            LastAttackTime = Now;
            // 카운트 다운 & 페이즈 전환
            if (--SpecialLeft <= 0)
            {
                bSpecialPhase = false;
                NormalLeft = FMath::Max(1, NormalQuotaPerCycle);
                SpecialLeft = FMath::Max(1, SpecialQuotaPerCycle); // 다음 특수 페이즈를 위해 리필
            }
            return true;
        }
    }
    else
    {
        // 일반 공격
        bDidAttack = TryNormalAttack();
        if (!bDidAttack)
        {
            // 일반 데이터 없거나 실패 시 특수로 폴백
            bDidAttack = TrySpecialAttack();
        }

        if (bDidAttack)
        {
            LastAttackTime = Now;
            // 카운트 다운 & 페이즈 전환
            if (--NormalLeft <= 0)
            {
                bSpecialPhase = true;
                SpecialLeft = FMath::Max(1, SpecialQuotaPerCycle);
                NormalLeft = FMath::Max(1, NormalQuotaPerCycle); // 다음 일반 페이즈를 위해 리필
            }
            return true;
        }
    }

    // 여기까지 오면 둘 다 실패(재생할 애님 없음 등)
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
                //ProjectToNav(GetWorld(), Target, Target);   //  보정

                 // ★ NavMesh 위로 흡착 (ProjectToNav 대체)
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
                // 랜덤순찰은 BT에서 TargetPosition을 갱신한 뒤,
                // 필요 시 PatrolTarget <- TargetPosition 으로 복사해서 MoveTo에 사용해도 됨.
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
    // BT Sequence: MoveTo(PatrolTarget) → Wait(LinePatrolWaitTime) → ToggleLinePatrolDirection()
    //if (GEngine)
    //{
    //    // Key: -1 = 새 메시지, 지속시간: 5초, 색상: 초록
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
                //ProjectToNav(GetWorld(), Target, Target);   // 보정
                 // ★ NavMesh 위로 흡착 (ProjectToNav 대체)
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

    // 모드 바뀌면 타겟 재설정
    RefreshPatrolTargetOnBB();
}

// ====== 히트박스 on/off (BP에서 호출 가능) ======
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

// ====== 손 히트박스 Overlap ======
void ACHREnemy::OnAttackBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!bHitBoxActive) return;
    if (!IsValid(OtherActor) || OtherActor == this) return;

    // (선택) Pawn/Character만 히트
    //if (!OtherActor->IsA<APawn>()) return;
   
    // Player만 타격 (BaseCharacter로 필터 가능)
    ABaseCharacter* Victim = Cast<ABaseCharacter>(OtherActor);
    if (!Victim || Victim->GetTeam() != ETeamType::Player) return;

    // 한 스윙 중복타 방지
    if (HitActors.Contains(OtherActor)) 
        return;
    
    HitActors.AddUnique(OtherActor);

    // (선택) 팀/진영 필터
    // if (IsSameTeam(OtherActor)) return;

    // ApplyDamage → Victim.TakeDamage 호출됨
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

    //// 한 스윙 동안 중복타 방지
    //if (HitActors.Contains(OtherActor)) return;
    //HitActors.Add(OtherActor);

    //// 데미지 주기
    //AController* InstigatorController = GetController();
    //UGameplayStatics::ApplyDamage(OtherActor, AttackDamage, InstigatorController, this, nullptr);
}
