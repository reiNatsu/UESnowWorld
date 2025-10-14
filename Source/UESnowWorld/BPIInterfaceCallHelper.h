// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * �������Ʈ �������̽�(BPI)�� �Լ���
 * C++���� �����ϰ� ȣ���ϱ� ���� ���� Ŭ����.
 *
 * ��� ����:
 *   float Attack = 0.f;
 *   UBPInterfaceCallHelper::CallFloatGetter(SomeActor, TEXT("GetAttackStat"), Attack);
 */

class BPIInterfaceCallHelper
{
    /** BPI���� �Է� ���� �� float ��ȯ �Լ� ȣ�� */
    static bool CallFloatGetter(UObject* Target, const FName FuncName, float& OutValue)
    {
        if (!IsValid(Target)) return false;

        // �Լ� ���� ���� üũ
        if (UFunction* Func = Target->FindFunction(FuncName))
        {
            struct FNoParam_RetFloat
            {
                float ReturnValue;
            } Params;

            FMemory::Memzero(&Params, sizeof(Params));
            Target->ProcessEvent(Func, &Params);

            OutValue = Params.ReturnValue;
            return true;
        }
        return false;
    }

    /** BPI���� �Է� ���� �� bool ��ȯ �Լ� ȣ�� */
    static bool CallBoolGetter(UObject* Target, const FName FuncName, bool& OutValue)
    {
        if (!IsValid(Target)) return false;

        if (UFunction* Func = Target->FindFunction(FuncName))
        {
            struct FNoParam_RetBool
            {
                bool ReturnValue;
            } Params;

            FMemory::Memzero(&Params, sizeof(Params));
            Target->ProcessEvent(Func, &Params);

            OutValue = Params.ReturnValue;
            return true;
        }
        return false;
    }
};
