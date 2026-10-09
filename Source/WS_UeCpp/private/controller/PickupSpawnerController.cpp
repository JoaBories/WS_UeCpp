// Fill out your copyright notice in the Description page of Project Settings.


#include "controller/PickupSpawnerController.h"

#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"

#include "gameplay/PickupSpawnerComponent.h"
#include "gameplay/MainCharacter.h"

UPickupSpawnerController::UPickupSpawnerController()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPickupSpawnerController::SetupInputComponentPickupSpawner(TObjectPtr<UInputComponent> InputComponent,
	AMainCharacter* MainCharacter)
{
	const bool bAllPointersChecked = InputComponent && MainCharacter && InputActionSpawnPickups;
	if (!bAllPointersChecked) return;
	
	// Get pickup spawner comp
	PickupSpawnerComponent = MainCharacter->FindComponentByClass<UPickupSpawnerComponent>();
	
	// Cast to Enhanced inputs
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent) return;
	
	// Bind input action
	EnhancedInputComponent->BindAction(InputActionSpawnPickups, ETriggerEvent::Triggered, this, &UPickupSpawnerController::OnSpawnPickup);
}

void UPickupSpawnerController::OnSpawnPickup(const FInputActionValue& InputActionValue)
{
	if (PickupSpawnerComponent.IsValid())
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		
		if (Value.X > 0)
		{
			PickupSpawnerComponent->SpawnNormalPickup();
		}
		else if (Value.X < 0)
		{
			PickupSpawnerComponent->SpawnThrowPickup();
		}
		else if (Value.Y > 0)
		{
			PickupSpawnerComponent->SpawnTakePickup();
		}
		else if (Value.Y < 0)
		{
			PickupSpawnerComponent->DebugSpawnerCount();
		}
	}
}
