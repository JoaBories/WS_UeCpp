// Fill out your copyright notice in the Description page of Project Settings.


#include "gameplay/PickupSpawnerComponent.h"

#include "Kismet/GameplayStatics.h"

UPickupSpawnerComponent::UPickupSpawnerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UPickupSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Get references
	PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
}

void UPickupSpawnerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	
	
	Super::EndPlay(EndPlayReason);
}

void UPickupSpawnerComponent::SpawnNormalPickup()
{
	if (!NormalPickup) return;
	
	SpawnPickup(NormalPickup);
}

void UPickupSpawnerComponent::SpawnThrowPickup()
{
	if (!ThrowPickup) return;
	
	SpawnPickup(ThrowPickup);
}

void UPickupSpawnerComponent::SpawnTakePickup()
{
	if (!TakePickup) return;
	
	SpawnPickup(TakePickup);
}

AActor* UPickupSpawnerComponent::SpawnPickup(UClass* PickupClass)
{
	const bool bCanSpawn = PickupClass && PlayerCameraManager.IsValid();
	if (!bCanSpawn) return nullptr;
	
	// Compute spawn location and rotation
	const FVector CameraLocation = PlayerCameraManager->GetCameraLocation();
	const FVector CameraForward = PlayerCameraManager->GetActorForwardVector();
	FVector PickupLocation = CameraLocation + (CameraForward * SpawnDistance);
	FRotator PickupRotation = PlayerCameraManager->GetCameraRotation();
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	return GetWorld()->SpawnActor(PickupClass, &PickupLocation, &PickupRotation, SpawnParams);
}

void UPickupSpawnerComponent::OnPickupDestroyed(UPickupComponent* PickupComponent)
{
	
}
