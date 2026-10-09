// Fill out your copyright notice in the Description page of Project Settings.


#include "gameplay/PickupSpawnerComponent.h"

#include "gameplay/PickupComponent.h"
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
	
	// Count and bind already spawned pickups
	CountAndBindPickups(NormalPickup, NormalPickupCount);
	CountAndBindPickups(ThrowPickup, ThrowPickupCount);
	CountAndBindPickups(TakePickup, TakePickupCount);
}

void UPickupSpawnerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unbind pickups
	UnbindPickups(NormalPickup);
	UnbindPickups(ThrowPickup);
	UnbindPickups(TakePickup);
	
	Super::EndPlay(EndPlayReason);
}

void UPickupSpawnerComponent::SpawnNormalPickup()
{
	// Check Class
	if (!NormalPickup) return;
	
	//Check Cap
	const bool bIsCapReached = TotalPickupCount >= TotalPickupCap || NormalPickupCount >= NormalPickupCap;
	if (bIsCapReached)
	{
		UE_LOG(LogTemp, Warning, TEXT("Too much Normal pickups or too much pickups, consider increasing total or Normal pickup cap"));
		return;
	}
	
	// Spawn pickup
	AActor* SpawnedActor = SpawnAndBindPickup(NormalPickup);
	if (!SpawnedActor) return;
	
	NormalPickupCount++;
	TotalPickupCount++;
}

void UPickupSpawnerComponent::SpawnThrowPickup()
{
	// Check Class
	if (!ThrowPickup) return;
	
	//Check Cap
	const bool bIsCapReached = TotalPickupCount >= TotalPickupCap || ThrowPickupCount >= ThrowPickupCap;
	if (bIsCapReached)
	{
		UE_LOG(LogTemp, Warning, TEXT("Too much Throw pickups or too much pickups, consider increasing total or Throw pickup cap"));
		return;
	}
	
	// Spawn pickup
	AActor* SpawnedActor = SpawnAndBindPickup(ThrowPickup);
	if (!SpawnedActor) return;
	
	ThrowPickupCount++;
	TotalPickupCount++;
}

void UPickupSpawnerComponent::SpawnTakePickup()
{
	// Check Class
	if (!TakePickup) return;
	
	//Check Cap
	const bool bIsCapReached = TotalPickupCount >= TotalPickupCap || TakePickupCount >= TakePickupCap;
	if (bIsCapReached)
	{
		UE_LOG(LogTemp, Warning, TEXT("Too much Take pickups or too much pickups, consider increasing total or Take pickup cap"));
		return;
	}
	
	// Spawn pickup
	AActor* SpawnedActor = SpawnAndBindPickup(TakePickup);
	if (!SpawnedActor) return;
	
	// Count Pickup
	TakePickupCount++;
	TotalPickupCount++;
}

void UPickupSpawnerComponent::DebugSpawnerCount()
{
	UE_LOG(LogTemp, Log, TEXT("--- Debug Counters -----------------"))
	UE_LOG(LogTemp, Log, TEXT("Total Pickups: %d / %d"), TotalPickupCount, TotalPickupCap);
	UE_LOG(LogTemp, Log, TEXT("Normal Pickup: %d / %d"), NormalPickupCount, NormalPickupCap);
	UE_LOG(LogTemp, Log, TEXT("Throw Pickup: %d / %d"), ThrowPickupCount, ThrowPickupCap);
	UE_LOG(LogTemp, Log, TEXT("Take Pickup: %d / %d"), TakePickupCount, TakePickupCap);
}

AActor* UPickupSpawnerComponent::SpawnAndBindPickup(UClass* PickupClass)
{
	const bool bCanSpawn = PickupClass && PlayerCameraManager.IsValid();
	if (!bCanSpawn) return nullptr;
	
	// Prepare spawn
	// Compute spawn location and rotation
	const FVector CameraLocation = PlayerCameraManager->GetCameraLocation();
	const FVector CameraForward = PlayerCameraManager->GetActorForwardVector();
	FVector PickupLocation = CameraLocation + (CameraForward * SpawnDistance);
	FRotator PickupRotation = PlayerCameraManager->GetCameraRotation();
	
	// Set parameters
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	// Spawn pickup
	AActor* SpawnedActor = GetWorld()->SpawnActor(PickupClass, &PickupLocation, &PickupRotation, SpawnParams);
	if (!SpawnedActor) return nullptr;
	
	// Bind pickup
	UPickupComponent* PickupComponent = SpawnedActor->FindComponentByClass<UPickupComponent>();
	if (!PickupComponent) return nullptr;
	PickupComponent->PickupDestroy.AddUniqueDynamic(this, &UPickupSpawnerComponent::OnPickupDestroyed);
	
	return SpawnedActor;
}

void UPickupSpawnerComponent::CountAndBindPickups(UClass* PickupClass, unsigned int& PickupCount)
{
	TArray<AActor*> PickupActors;
	
	UGameplayStatics::GetAllActorsOfClass(this, PickupClass, PickupActors);
	for (AActor* Pickup : PickupActors)
	{
		// Get and Check Pickup comp
		UPickupComponent* PickupComp = Pickup->FindComponentByClass<UPickupComponent>();
		if (!PickupComp) continue; 
		
		// Count and bind
		PickupCount++;
		TotalPickupCount++;
		PickupComp->PickupDestroy.AddUniqueDynamic(this, &UPickupSpawnerComponent::OnPickupDestroyed);
	}
}

void UPickupSpawnerComponent::UnbindPickups(UClass* PickupClass)
{	
	TArray<AActor*> PickupActors;
	
	UGameplayStatics::GetAllActorsOfClass(this, PickupClass, PickupActors);
	for (AActor* Pickup : PickupActors)
	{
		// Get and Check Pickup comp
		UPickupComponent* PickupComp = Pickup->FindComponentByClass<UPickupComponent>();
		if (!PickupComp) continue;
		
		PickupComp->PickupDestroy.RemoveDynamic(this, &UPickupSpawnerComponent::OnPickupDestroyed);
	}
}

void UPickupSpawnerComponent::OnPickupDestroyed(UPickupComponent* PickupComponent)
{
	EPickupType PickupType = PickupComponent->GetPickupType();
	if (PickupType == EPickupType::Normal) NormalPickupCount--;
	else if (PickupType == EPickupType::DestroyAfterThrow) ThrowPickupCount--;
	else if (PickupType == EPickupType::DestroyAfterTake) TakePickupCount--;
	else return;
	
	TotalPickupCount--;
}
