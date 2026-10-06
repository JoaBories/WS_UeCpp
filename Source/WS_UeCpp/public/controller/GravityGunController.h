// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GravityGunController.generated.h"


UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WS_UECPP_API UGravityGunController : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGravityGunController();
};
