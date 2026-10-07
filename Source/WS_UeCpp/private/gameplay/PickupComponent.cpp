// Fill out your copyright notice in the Description page of Project Settings.

#include "gameplay/PickupComponent.h"

UPickupComponent::UPickupComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

EPickupType UPickupComponent::GetPickupType() const
{
	return PickupStruct.PickupType;
}

void UPickupComponent::StartPickupDestructionTimer()
{
	const float DestructionTime = PickupStruct.DestructionTime;
	
	// Prepare timer
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(DestructionTimerHandle);
	TimerManager.SetTimer(DestructionTimerHandle, this, &UPickupComponent::DestroyPickup, DestructionTime, false);
}

void UPickupComponent::DestroyPickup()
{
	// Clear timer
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(DestructionTimerHandle);
	
	GetOwner()->Destroy();
}
