// Fill out your copyright notice in the Description page of Project Settings.


#include "gameplay/Goal.h"

#include "Components/BoxComponent.h"
#include "gameplay/PickupComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UObject/ObjectSaveContext.h"

AGoal::AGoal(const FObjectInitializer& ObjectInitializer) : 
	Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	
	// Create Box Component
	BoxComponent = ObjectInitializer.CreateOptionalDefaultSubobject<UBoxComponent>(this, TEXT("BoxComponent"));
	if (BoxComponent) SetRootComponent(BoxComponent);
}

#if !UE_BUILD_SHIPPING
void AGoal::PreSave(const FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);
	
	//Make sure we're on the world and make sure we're dealing with an Instance
	const bool bIsInstanceInLoadedWorld = GetWorld() && !IsTemplate();
	if (bIsInstanceInLoadedWorld)
	{
		if (Team == ETeam::None)
		{
			FString GoalName = UKismetSystemLibrary::GetDisplayName(this);
			UE_LOG(LogTemp, Warning, TEXT("The team need to be setup for %s"), *GoalName);
		}
	}
}
#endif

ETeam AGoal::GetTeam() const
{
	return Team;
}

void AGoal::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind on Overlap event from Box comp
	if (BoxComponent)
	{
		BoxComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AGoal::OnBoxComponentOverlap);
	}
}

void AGoal::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unbind from Overlap event from Box comp
	if (BoxComponent)
	{
		BoxComponent->OnComponentBeginOverlap.RemoveDynamic(this, &AGoal::OnBoxComponentOverlap);
	}
	
	Super::EndPlay(EndPlayReason);
}

unsigned int AGoal::CountPickupInGoal()
{
	if (!BoxComponent) return 0;
	
	// Prepare Box Cast
	const FVector GoalLocation = GetActorLocation();
	const FRotator GoalRotation = GetActorRotation();
	const FVector ScaleBoxExtent = BoxComponent->GetScaledBoxExtent();
	TArray<FHitResult> HitResults;
	TArray<AActor*> ActorsToIgnore;
	
	UKismetSystemLibrary::BoxTraceMulti(this, 
		GoalLocation, GoalLocation, ScaleBoxExtent, GoalRotation, 
		GoalTraceChannel, false, ActorsToIgnore, EDrawDebugTrace::None, 
		HitResults, true);

	const unsigned int Count = HitResults.Num();
	return Count;
	
	// // Method with overlapping actors
	// // Get actors
	// TArray<AActor*> OverlappingActors;
	// BoxComponent->GetOverlappingActors(OverlappingActors);
	// // Count Pickups among OverlappingActors
	// unsigned int NumberOfPickup = 0;
	// for (AActor* Actor : OverlappingActors)
	// {
	// 	if (Actor->FindComponentByClass<UPickupComponent>())
	// 	{
	// 		NumberOfPickup++;
	// 	}
	// }
	// return NumberOfPickup;
}

void AGoal::OnBoxComponentOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;
	
	// Check if we're dealing with a pickup
	UPickupComponent* PickupComponent =	OtherActor->FindComponentByClass<UPickupComponent>();
	if (!PickupComponent) return;
	
	// Update score
	Score++;
	
	GoalScored.Broadcast(this, Score);
	
#if !UE_BUILD_SHIPPING
	FString GoalName = UKismetSystemLibrary::GetDisplayName(this);
	UE_LOG(LogTemp, Log, TEXT("%d Pickup entered the %s"), Score, *GoalName);
#endif
}

