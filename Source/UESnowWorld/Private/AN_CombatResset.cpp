// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_CombatResset.h"
#include "CHRPlayer.h"

void UAN_CombatResset::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!MeshComp) return;

    if (auto* Player = Cast<ACHRPlayer>(MeshComp->GetOwner()))
    {
        //Player->OnAttackSectionEnd();   // 섹션 종료 시점 로직(콤보 전환 판단)
    }
}
