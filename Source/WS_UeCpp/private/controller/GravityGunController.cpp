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
	
	EnhancedInputComponent->BindAction(InputActionThrow, ETriggerEvent::Started, this, &UGravityGunController::OnThrowObjectPressed);
	EnhancedInputComponent->BindAction(InputActionThrow, ETriggerEvent::Completed, this, &UGravityGunController::OnThrowObjectReleased);
	
	EnhancedInputComponent->BindAction(InputActionDestroy, ETriggerEvent::Triggered, this, &UGravityGunController::OnDestroyObject);
	
	EnhancedInputComponent->BindAction(InputActionAdditionalMult, ETriggerEvent::Started, this, &UGravityGunController::OnAdditionalMultPressed);
	EnhancedInputComponent->BindAction(InputActionAdditionalMult, ETriggerEvent::Completed, this, &UGravityGunController::OnAdditionalMultReleased);
	
	EnhancedInputComponent->BindAction(InputActionUpdateReach, ETriggerEvent::Triggered, this, &UGravityGunController::OnUpdateReach);
}

void UGravityGunController::OnTakeObject()
{
	if (GravityGunComponent.IsValid()) GravityGunComponent->OnTakeObject();
}

void UGravityGunController::OnThrowObjectPressed()
{
	if (GravityGunComponent.IsValid()) GravityGunComponent->OnThrowObjectPressed();
}

void UGravityGunController::OnThrowObjectReleased()
{
	if (GravityGunComponent.IsValid()) GravityGunComponent->OnThrowObjectReleased();
}

void UGravityGunController::OnDestroyObject()
{
	if (GravityGunComponent.IsValid()) GravityGunComponent->OnDestroyObject();
}

void UGravityGunController::OnAdditionalMultPressed()
{
	if (GravityGunComponent.IsValid()) GravityGunComponent->OnAdditionalMult(true);
}

void UGravityGunController::OnAdditionalMultReleased()
{
	if (GravityGunComponent.IsValid()) GravityGunComponent->OnAdditionalMult(false);
}

void UGravityGunController::OnUpdateReach(const FInputActionValue& Value)
{
	if (GravityGunComponent.IsValid())
	{
		const float FloatValue = Value.Get<float>();
		GravityGunComponent->OnUpdateReach(FloatValue);
	}
}
