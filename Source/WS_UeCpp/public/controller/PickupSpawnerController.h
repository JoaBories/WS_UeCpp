// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PickupSpawnerController.generated.h"

struct FInputActionValue;
class AMainCharacter;
class UPickupSpawnerComponent;
class UInputAction;

UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WS_UECPP_API UPickupSpawnerController : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPickupSpawnerController();

protected:
	TWeakObjectPtr<UPickupSpawnerComponent> PickupSpawnerComponent;
	
#pragma region Input
protected:
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	TObjectPtr<UInputAction> InputActionSpawnPickups = nullptr;
	
public:
	void SetupInputComponentPickupSpawner(TObjectPtr<UInputComponent> InputComponent, AMainCharacter* MainCharacter);
	
protected:
	void OnSpawnPickup(const FInputActionValue& InputActionValue);
#pragma endregion
		
};
