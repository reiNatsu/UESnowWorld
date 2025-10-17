// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 블루프린트 인터페이스(BPI)의 함수를
 * C++에서 안전하게 호출하기 위한 헬퍼 클래스.
 *
 * 사용 예시:
 *   float Attack = 0.f;
 *   UBPInterfaceCallHelper::CallFloatGetter(SomeActor, TEXT("GetAttackStat"), Attack);
 */

class BPIInterfaceCallHelper
{
    /** BPI에서 입력 없음 → float 반환 함수 호출 */
    static bool CallFloatGetter(UObject* Target, const FName FuncName, float& OutValue)
    {
        if (!IsValid(Target)) return false;

        // 함수 존재 여부 체크
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

    /** BPI에서 입력 없음 → bool 반환 함수 호출 */
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
