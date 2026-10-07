// Fill out your copyright notice in the Description page of Project Settings.


#include "gameplay/GravityGunComponent.h"

#include "gameplay/MainCharacter.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"

#include "gameplay/PickupComponent.h"
#include "Components/StaticMeshComponent.h"

UGravityGunComponent::UGravityGunComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGravityGunComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	UpdatePickupLocation();
}

void UGravityGunComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Get references
	Character = Cast<AMainCharacter>(GetOwner());
	PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	
	// Convert trace query to collision channel
	GravityGunCollisionChannel = UEngineTypes::ConvertToCollisionChannel(GravityGunTraceChannel);
	
	if (GravityGunMinReach > GravityGunMaxReach) UE_LOG(LogTemp, Error, TEXT("Minimum reach is higher than Maximum reach"))
	GravityGunReach = FMath::Clamp(GravityGunReach, GravityGunMinReach, GravityGunMaxReach);
	
	if (PickupHoldMinDistance > PickupHoldMaxDistance) UE_LOG(LogTemp, Error, TEXT("Minimum hold distance is higher than Maximum hold distance"))
	PickupHoldDistance = FMath::Clamp(PickupHoldDistance, PickupHoldMinDistance, PickupHoldMaxDistance);
}

void UGravityGunComponent::OnTakeObjectInputPressed()
{
	const bool bAllPointersChecked = PlayerCameraManager.IsValid() && Character.IsValid();
	if (!bAllPointersChecked) return;
	
	if (CurrentPickup.IsValid())
	{
		ReleasePickup();
		return;
	}
	
	// Prepare Raycast
	const FVector RaycastStart = PlayerCameraManager->GetCameraLocation();
	const FVector RaycastEnd = RaycastStart + PlayerCameraManager->GetActorForwardVector() * GravityGunReach;
	FHitResult HitResult;
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character.Get());
	
	// Launch Raycast
#if !UE_BUILD_SHIPPING
	if (bDrawDebugLine)
	{
		DrawDebugLine(GetWorld(), RaycastStart, RaycastEnd, 
			FColor::Red, false, DrawDebugTime, 0, 1.0f);
	}
#endif
	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, 
		RaycastStart, RaycastEnd, GravityGunCollisionChannel, Params);
	
	if (!bHit)
	{
		UE_LOG(LogTemp, Log, TEXT("Didn't hit nothing"));
		return;
	}
	
	UE_LOG(LogTemp, Log, TEXT("We hit: %s"), 
		*UKismetSystemLibrary::GetDisplayName(HitResult.GetActor()));
	
	// Get pickup reference
	CurrentPickup = HitResult.GetActor();
	if (CurrentPickup.IsValid())
	{
		CurrentPickupComponent = CurrentPickup->FindComponentByClass<UPickupComponent>();
		CurrentPickupStaticMesh = CurrentPickup->FindComponentByClass<UStaticMeshComponent>();
		if (!CurrentPickupComponent.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("Pick UP is missing Pick Up Component"));
		}
	}
	
	PickupHoldDistance = FMath::Clamp(HitResult.Distance, PickupHoldMinDistance, PickupHoldMaxDistance);
	
	// Disable pickup physics
	CurrentPickupStaticMesh->SetSimulatePhysics(false);
	PreviousCollisionProfile = CurrentPickupStaticMesh->GetCollisionProfileName();
	CurrentPickupStaticMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
}

void UGravityGunComponent::OnThrowObjectInputPressed()
{
	if (CurrentPickup.IsValid())
	{
		ReleasePickup(true);
	}
}

void UGravityGunComponent::OnUpdateReach(const float Value)
{
	if (CurrentPickup.IsValid())
	{
		PickupHoldDistance += Value * PickupHoldChangerate;
		PickupHoldDistance = FMath::Clamp(PickupHoldDistance, PickupHoldMinDistance, PickupHoldMaxDistance);
		UE_LOG(LogTemp, Log, TEXT("Updated Hold Distance: %f cm"), PickupHoldDistance);
	}
	else
	{
		GravityGunReach += Value * GravityGunReachChangerate;
		GravityGunReach = FMath::Clamp(GravityGunReach, GravityGunMinReach, GravityGunMaxReach);
		UE_LOG(LogTemp, Log, TEXT("Updated Reach: %f cm"), GravityGunReach);
	}
}

void UGravityGunComponent::UpdatePickupLocation()
{
	if (!CurrentPickup.IsValid() || !PlayerCameraManager.IsValid()) return;
	
	// Compute and apply new transform 
	const FRotator CameraRotation = PlayerCameraManager->GetCameraRotation();
	const FVector CameraLocation = PlayerCameraManager->GetCameraLocation();
	const FVector CameraForward = PlayerCameraManager->GetActorForwardVector();
	
	FVector NewLocation = CameraLocation + (CameraForward * PickupHoldDistance);
	NewLocation.Z += PickupHeightOffset;
	CurrentPickup->SetActorLocationAndRotation(NewLocation, CameraRotation);
}

void UGravityGunComponent::ReleasePickup(bool bThrow)
{
	// Enable pickup physics
	CurrentPickupStaticMesh->SetCollisionProfileName(PreviousCollisionProfile);
	CurrentPickupStaticMesh->SetSimulatePhysics(true);
	
	// Throw pickup
	if (bThrow && PlayerCameraManager.IsValid())
	{
		const FVector Impulse = PlayerCameraManager->GetActorForwardVector() * PickupThrowForce;
		CurrentPickupStaticMesh->AddImpulse(Impulse);
		
		const FVector AngularImpulse = FVector(
			FMath::RandRange(-PickupAngularForce.X,PickupAngularForce.X),
			FMath::RandRange(-PickupAngularForce.Y,PickupAngularForce.Y),
			FMath::RandRange(-PickupAngularForce.Z,PickupAngularForce.Z));
		CurrentPickupStaticMesh->AddAngularImpulseInDegrees(AngularImpulse);
	}
	
	// Clear pointers
	CurrentPickupStaticMesh = nullptr;
	CurrentPickupComponent = nullptr;
	CurrentPickup = nullptr;
}
