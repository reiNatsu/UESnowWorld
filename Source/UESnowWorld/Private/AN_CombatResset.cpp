// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_CombatResset.h"
#include "CHRPlayer.h"

void UAN_CombatResset::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!MeshComp) return;

    if (auto* Player = Cast<ACHRPlayer>(MeshComp->GetOwner()))
    {
        //Player->OnAttackSectionEnd();   // ���� ���� ���� ����(�޺� ��ȯ �Ǵ�)
    }
}
