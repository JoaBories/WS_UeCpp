// Fill out your copyright notice in the Description page of Project Settings.


#include "gameplay/Goal.h"

#include "Components/BoxComponent.h"
#include "gameplay/PickupComponent.h"
#include "Kismet/KismetSystemLibrary.h"

AGoal::AGoal(const FObjectInitializer& ObjectInitializer) : 
	Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	
	// Create Box Component
	BoxComponent = ObjectInitializer.CreateOptionalDefaultSubobject<UBoxComponent>(this, TEXT("BoxComponent"));
	if (BoxComponent) SetRootComponent(BoxComponent);
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
	FString GoalName = UKismetSystemLibrary::GetDisplayName(this);
	UE_LOG(LogTemp, Log, TEXT("%d Pickup entered the %s"), Score, *GoalName);
}

