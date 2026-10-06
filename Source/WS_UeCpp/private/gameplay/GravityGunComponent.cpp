// Fill out your copyright notice in the Description page of Project Settings.


#include "gameplay/GravityGunComponent.h"

UGravityGunComponent::UGravityGunComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGravityGunComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UGravityGunComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

