// Fill out your copyright notice in the Description page of Project Settings.


#include "controller/GravityGunController.h"

#include "gameplay/GravityGunComponent.h"
#include "gameplay/MainCharacter.h"

#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"

UGravityGunController::UGravityGunController()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGravityGunController::SetupInputComponentGravityGun(TObjectPtr<UInputComponent> InputComponent, 
	AMainCharacter* MainCharacter)
{
	const bool bAllPointersChecked = InputComponent && MainCharacter && InputActionTake && InputActionThrow && InputActionUpdateReach;
	if (!bAllPointersChecked) return;
	
	// Get gravity gun comp
	GravityGunComponent = MainCharacter->FindComponentByClass<UGravityGunComponent>();
	
	// Cast to Enhanced inputs
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent) return;
	
	// Bind input action
	EnhancedInputComponent->BindAction(InputActionTake, ETriggerEvent::Triggered, this, &UGravityGunController::OnTakeObject);
	EnhancedInputComponent->BindAction(InputActionThrow, ETriggerEvent::Triggered, this, &UGravityGunController::OnThrowObject);
	EnhancedInputComponent->BindAction(InputActionUpdateReach, ETriggerEvent::Triggered, this, &UGravityGunController::OnUpdateReach);
}

void UGravityGunController::OnTakeObject(const FInputActionValue& Value)
{
	if (GravityGunComponent.IsValid()) GravityGunComponent->OnTakeObjectInputPressed();
}

void UGravityGunController::OnThrowObject(const FInputActionValue& Value)
{
	if (GravityGunComponent.IsValid()) GravityGunComponent->OnThrowObjectInputPressed();
}

void UGravityGunController::OnUpdateReach(const FInputActionValue& Value)
{
	if (GravityGunComponent.IsValid())
	{
		const float FloatValue = Value.Get<float>();
		GravityGunComponent->OnUpdateReach(FloatValue);
	}
}
