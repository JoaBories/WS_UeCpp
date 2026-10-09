// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM()
enum class EPickupType : uint8
{
	None UMETA(Hidden),
	Normal,
	DestroyAfterTake,
	DestroyAfterThrow,
	MAX UMETA(Hidden)
};
