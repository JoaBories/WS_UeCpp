// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

/**
 * Main player character of ue c++ workshop 
 */
UCLASS(Abstract)
class WS_UECPP_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMainCharacter();
};