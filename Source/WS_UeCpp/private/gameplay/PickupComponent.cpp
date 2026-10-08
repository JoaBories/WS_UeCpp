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

void UPickupComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearDestructionTimer();
	
	// For EndPIay and other similar function cal led when an object is destructed
	// It 's important to call them at the end
	Super::EndPlay(EndPlayReason);
}

void UPickupComponent::StartPickupDestructionTimer()
{
	const float DestructionTime = PickupStruct.DestructionTime;
	
	// Prepare timer
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(DestructionTimerHandle);
	TimerManager.SetTimer(DestructionTimerHandle, this, &UPickupComponent::DestroyPickup, DestructionTime, false);
}

void UPickupComponent::ClearDestructionTimer()
{
	// Clear timer
    FTimerManager& TimerManager = GetWorld()->GetTimerManager();
    TimerManager.ClearTimer(DestructionTimerHandle);
}

void UPickupComponent::DestroyPickup()
{
	ClearDestructionTimer();
	PickupDestroy.Broadcast();
	GetOwner()->Destroy();
}
