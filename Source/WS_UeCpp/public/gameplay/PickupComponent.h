// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "gameplay/PickupEnum.h"

#include "PickupComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickupDestroyedDelegate, UPickupComponent*, PickupComponent);

USTRUCT(BlueprintType)
struct FPickupStruct
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	EPickupType PickupType = EPickupType::None;
	UPROPERTY(EditAnywhere, meta = (EditCondition = "(PickupType == EPickupType::DestroyAfterPickup) || (PickupType == EPickupType::DestroyAfterThrow)"))
	float DestructionTime = 5.0f;
};

UCLASS( ClassGroup=("Pickup"), meta=(BlueprintSpawnableComponent) )
class WS_UECPP_API UPickupComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPickupComponent();
	EPickupType GetPickupType() const;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:
	FOnPickupDestroyedDelegate PickupDestroy;
	
protected:	
	UPROPERTY(EditAnywhere, Category = "Pickup")
	FPickupStruct PickupStruct;
	
#pragma region Destruction
protected:
	FTimerHandle DestructionTimerHandle;

public:
	void StartPickupDestructionTimer();
	void ClearDestructionTimer();

protected:
	void DestroyPickup();
#pragma endregion
};