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
	
	if (bThrowPressed) TimeThrowPressed += DeltaTime;
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

void UGravityGunComponent::OnTakeObject()
{
	const bool bAllPointersChecked = PlayerCameraManager.IsValid() && Character.IsValid();
	if (!bAllPointersChecked) return;
	
	if (CurrentPickup.IsValid())
	{
		ReleasePickup();
		return;
	}
	
	// Prepare Raycast
	FHitResult HitResult;
	
	const bool bHit = ShootPickupSphereTrace(SphereTraceRadius, GravityGunReach, HitResult, bDrawDebug);
	if (!bHit)
	{
		//UE_LOG(LogTemp, Log, TEXT("Didn't hit anything"));
		return;
	}
	
	//UE_LOG(LogTemp, Log, TEXT("We hit: %s"), *UKismetSystemLibrary::GetDisplayName(HitResult.GetActor()));
	
	// Try grab pickup
	TryGrabPickup(HitResult.GetActor());
}

void UGravityGunComponent::OnThrowObjectPressed()
{
	if (CurrentPickup.IsValid())
	{
		TimeThrowPressed = 0.0f;
		bThrowPressed = true;
	}
}

void UGravityGunComponent::OnThrowObjectReleased()
{
	if (CurrentPickup.IsValid())
	{
		ReleasePickup(true);
	}

	TimeThrowPressed = 0.0f;
	bThrowPressed = false;
}

void UGravityGunComponent::OnDestroyObject()
{
	const bool bIsPickupValid = CurrentPickup.IsValid() && CurrentPickupComponent.IsValid();
	if (bIsPickupValid)
	{
		CurrentPickupComponent->DestroyPickup();
	}
}

void UGravityGunComponent::OnAdditionalMult(bool State)
{
	bAdditionalMult = State;
}

void UGravityGunComponent::OnUpdateReach(const float Value)
{
	if (CurrentPickup.IsValid())
	{
		PickupHoldDistance += Value * PickupHoldChangerate;
		PickupHoldDistance = FMath::Clamp(PickupHoldDistance, PickupHoldMinDistance, PickupHoldMaxDistance);
		//UE_LOG(LogTemp, Log, TEXT("Updated Hold Distance: %f cm"), PickupHoldDistance);
	}
	else
	{
		GravityGunReach += Value * GravityGunReachChangerate;
		GravityGunReach = FMath::Clamp(GravityGunReach, GravityGunMinReach, GravityGunMaxReach);
		//UE_LOG(LogTemp, Log, TEXT("Updated Reach: %f cm"), GravityGunReach);
	}
}

bool UGravityGunComponent::TryGrabPickup(AActor* Actor)
{
	// Don't grab if there is already a pickup
	if (CurrentPickup.IsValid()) return false;
	
	// Check pointer
	if (!Actor) return false;
	CurrentPickup = Actor;
	
	// Check actor
	if (!CurrentPickup.IsValid()) return false;
		
	// Get and check Pickup comp
	CurrentPickupComponent = CurrentPickup->FindComponentByClass<UPickupComponent>();
	if (!CurrentPickupComponent.IsValid())
	{
		UE_LOG(LogTemp, Log, TEXT("Pick UP is missing Pick Up Component"));
		return false;
	}
	
	// Get Static mesh
	CurrentPickupStaticMesh = CurrentPickup->FindComponentByClass<UStaticMeshComponent>();

	// Set hold distance depending on pickup distance
	const FVector CameraPosition = PlayerCameraManager->GetCameraLocation();
	const float Distance = (CurrentPickup->GetActorLocation() - CameraPosition).Length();
	PickupHoldDistance = FMath::Clamp(Distance, PickupHoldMinDistance, PickupHoldMaxDistance);
	
	// Disable pickup physics
	CurrentPickupStaticMesh->SetSimulatePhysics(false);
	PreviousCollisionProfile = CurrentPickupStaticMesh->GetCollisionProfileName();
	CurrentPickupStaticMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	
	// Check pickup type
	const EPickupType PickupType = CurrentPickupComponent.IsValid() ? CurrentPickupComponent->GetPickupType() : EPickupType::None;
	switch (PickupType)
	{
		case EPickupType::DestroyAfterTake:
			// Launch Timer
			CurrentPickupComponent->StartPickupDestructionTimer();
				
			// Bind on event
			CurrentPickupComponent->PickupDestroy.AddUniqueDynamic(this, &UGravityGunComponent::OnPickupDestroyed);
			break;
			
		case EPickupType::DestroyAfterThrow:
			CurrentPickupComponent->ClearDestructionTimer();
			break;
			
		default:
			break;
	}
	
	// Broadcast pickup event
	PickupTaken.Broadcast(CurrentPickup.Get());
	
	return true;
}

float UGravityGunComponent::GetThrowMaxHoldTime() const
{
	return ThrowMaxHoldTime;
}

float UGravityGunComponent::GetThrowTime() const
{
	return TimeThrowPressed;
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

void UGravityGunComponent::ReleasePickup(const bool bThrow)
{
	// Enable pickup physics
	CurrentPickupStaticMesh->SetCollisionProfileName(PreviousCollisionProfile);
	CurrentPickupStaticMesh->SetSimulatePhysics(true);
	
	// Throw pickup
	const bool bCanThrowPickup = bThrow && PlayerCameraManager.IsValid();
	if (bCanThrowPickup)
	{
		const float ThrowMult = ThrowMaxHoldMult * FMath::Clamp(TimeThrowPressed / ThrowMaxHoldTime, 0.0f, 1.0f) * (bAdditionalMult ? ThrowAdditionalMult : 1.0f);
		const FVector Impulse = PlayerCameraManager->GetActorForwardVector() * PickupThrowForce * ThrowMult;
		CurrentPickupStaticMesh->AddImpulse(Impulse);
		
		const FVector AngularImpulse = FVector(
			FMath::RandRange(-PickupAngularForce.X,PickupAngularForce.X),
			FMath::RandRange(-PickupAngularForce.Y,PickupAngularForce.Y),
			FMath::RandRange(-PickupAngularForce.Z,PickupAngularForce.Z));
		CurrentPickupStaticMesh->AddAngularImpulseInDegrees(AngularImpulse);
		
		//UE_LOG(LogTemp, Log, TEXT("Mult applied: %f"), ThrowMult);
	
		// Check if destruction required
		const bool bCanDestroyPickup = CurrentPickupComponent.IsValid() && CurrentPickupComponent->GetPickupType() == EPickupType::DestroyAfterThrow;
		if (bCanDestroyPickup)
		{
			CurrentPickupComponent->StartPickupDestructionTimer();
		}
	}
	
	// Unbind on destroy event
	const bool bCanUnbindFromDestructionEvent = 
		CurrentPickupComponent.IsValid() && CurrentPickupComponent->GetPickupType() == EPickupType::DestroyAfterTake;
	if (bCanUnbindFromDestructionEvent)
	{
		CurrentPickupComponent->PickupDestroy.RemoveDynamic(this, &UGravityGunComponent::OnPickupDestroyed);
	}
	
	// Clear pointers
	CurrentPickupStaticMesh = nullptr;
	CurrentPickupComponent = nullptr;
	CurrentPickup = nullptr;
}

bool UGravityGunComponent::ShootPickupSphereTrace(const float Radius, const float Length, FHitResult& Hit, const bool bDrawDebugTrace)
{
	// Prepare Raycast
	const FVector RaycastStart = PlayerCameraManager->GetCameraLocation();
	const FVector RaycastEnd = RaycastStart + PlayerCameraManager->GetActorForwardVector() * Length;

	const TArray<AActor*> ActorsToIgnore;
	
	// Launch Sphere trace
	const bool bHit = UKismetSystemLibrary::SphereTraceSingle(this, 
		RaycastStart, RaycastEnd, Radius, 
		GravityGunTraceChannel, false, ActorsToIgnore, 
		bDrawDebugTrace ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, Hit, true, 
		FLinearColor::Red, FLinearColor::Green, DrawDebugTime);
	
	return bHit;
}

void UGravityGunComponent::OnPickupDestroyed(UPickupComponent* PickupComponent)
{
	ReleasePickup();
}
