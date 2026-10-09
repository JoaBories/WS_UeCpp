// Fill out your copyright notice in the Description page of Project Settings.


#include "gameplay/PickupSpawnerComponent.h"

#include "Kismet/GameplayStatics.h"

#include "gameplay/MainCharacter.h"
#include "gameplay/PickupComponent.h"
#include "gameplay/GravityGunComponent.h"

UPickupSpawnerComponent::UPickupSpawnerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPickupSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Get references
	PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	Character = Cast<AMainCharacter>(GetOwner());
	GravityGunComponent = Character->FindComponentByClass<UGravityGunComponent>();
	
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
	
	ClearCooldownTimer();
	
	Super::EndPlay(EndPlayReason);
}

void UPickupSpawnerComponent::SpawnNormalPickup()
{
	// Check Class and Cooldown
	if (!NormalPickup || bIsOnCooldown) return;
	
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
	
	// Try grabbing spawned pickup
	if (GravityGunComponent.IsValid())
	{
		GravityGunComponent->TryGrabPickup(SpawnedActor);
	}
	
	LaunchCooldownTimer();
	
	// Count pickup
	NormalPickupCount++;
	TotalPickupCount++;
}

void UPickupSpawnerComponent::SpawnThrowPickup()
{
	// Check Class and Cooldown
	if (!ThrowPickup || bIsOnCooldown) return;
	
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
	
	// Try grabbing spawned pickup
	if (GravityGunComponent.IsValid())
	{
		GravityGunComponent->TryGrabPickup(SpawnedActor);
	}
	
	LaunchCooldownTimer();
	
	// Count pickup
	ThrowPickupCount++;
	TotalPickupCount++;
}

void UPickupSpawnerComponent::SpawnTakePickup()
{
	// Check Class and Cooldown
	if (!TakePickup || bIsOnCooldown) return;
	
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
	
	// Try grabbing spawned pickup
	if (GravityGunComponent.IsValid())
	{
		GravityGunComponent->TryGrabPickup(SpawnedActor);
	}
	
	LaunchCooldownTimer();
	
	// Count pickup
	TakePickupCount++;
	TotalPickupCount++;
}

void UPickupSpawnerComponent::DisplayPickupCounters()
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

	const FVector PickupLocation = CameraLocation + (CameraForward * SpawnDistance);
	const FRotator PickupRotation = PlayerCameraManager->GetCameraRotation();
	
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
	for (const AActor* Pickup : PickupActors)
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
	for (const AActor* Pickup : PickupActors)
	{
		// Get and Check Pickup comp
		UPickupComponent* PickupComp = Pickup->FindComponentByClass<UPickupComponent>();
		if (!PickupComp) continue;
		
		PickupComp->PickupDestroy.RemoveDynamic(this, &UPickupSpawnerComponent::OnPickupDestroyed);
	}
}

void UPickupSpawnerComponent::LaunchCooldownTimer()
{
	bIsOnCooldown = true;
	
	// Prepare timer
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(CooldownTimerHandle);
	TimerManager.SetTimer(CooldownTimerHandle, this, &UPickupSpawnerComponent::EndCooldown, SpawnCooldown, false);
}

void UPickupSpawnerComponent::ClearCooldownTimer()
{
	// Clear timer
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(CooldownTimerHandle);
}

void UPickupSpawnerComponent::EndCooldown()
{
	ClearCooldownTimer();
	bIsOnCooldown = false;
}

void UPickupSpawnerComponent::OnPickupDestroyed(UPickupComponent* PickupComponent)
{
	if (!PickupComponent) return;
	
	PickupComponent->PickupDestroy.RemoveDynamic(this, &UPickupSpawnerComponent::OnPickupDestroyed);
	
	const EPickupType PickupType = PickupComponent->GetPickupType();
	
	if (PickupType == EPickupType::Normal) NormalPickupCount--;
	else if (PickupType == EPickupType::DestroyAfterThrow) ThrowPickupCount--;
	else if (PickupType == EPickupType::DestroyAfterTake) TakePickupCount--;
	else return;
	
	TotalPickupCount--;
}
