// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GravityGunDataAsset.generated.h"

/**
 * Some datas
 */
UCLASS()
class WS_UECPP_API UGravityGunDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Throw", meta = (ClampMin = "0.0", ClampMax = "10000.0", Units = "CentimetersPerSecond"))
	float PickupThrowForce = 3000.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Throw")
	FVector PickupAngularForce = FVector(2000.0f, 2000.0f, 2000.0f);
};
