// Fill out your copyright notice in the Description page of Project Settings.


#include "gameplay/ScoreComponent.h"

#include "gameplay/Goal.h"
#include "Kismet/GameplayStatics.h"

UScoreComponent::UScoreComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UScoreComponent::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(this, AGoal::StaticClass(), actors);
	
	for (AActor* actor : actors)
	{
		AGoal* goal = Cast<AGoal>(actor);
		if (goal)
		{
			goal->GoalScored.AddUniqueDynamic(this, &UScoreComponent::OnGoalScored);
			GoalMap.Add(goal, 0);
		}
	}
}

void UScoreComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	for (const auto& Goal : GoalMap)
	{
		if (Goal.Key)
		{
			Goal.Key->GoalScored.RemoveDynamic(this, &UScoreComponent::OnGoalScored);
		}
	}
	
	GoalMap.Empty();
	
	Super::EndPlay(EndPlayReason);
}

void UScoreComponent::PrintScore()
{
	UE_LOG(LogTemp, Log, TEXT("--- Scores ----------------------------------"));
	for (const auto& GoalEntry : GoalMap)
	{
		AGoal* Goal = GoalEntry.Key;
		
		if (Goal)
		{
#if !UE_BUILD_SHIPPING
			unsigned int Score = GoalEntry.Value;
			FString GoalName = UKismetSystemLibrary::GetDisplayName(Goal);
			UE_LOG(LogTemp, Log, TEXT("Goal %s , Score: %i"), *GoalName, Score);
#endif
		}
		else
		{
			GoalMap.Remove(Goal);
		}
	}
}

void UScoreComponent::NumberOfPickupsInGoals()
{
	UE_LOG(LogTemp, Log, TEXT("--- Number Of Pickups In Goals --------------"));
	for (const auto& GoalEntry : GoalMap)
	{
		AGoal* Goal = GoalEntry.Key;
		
		if (Goal)
		{
#if !UE_BUILD_SHIPPING
			unsigned int Score = Goal->CountPickupInGoal();
			FString GoalName = UKismetSystemLibrary::GetDisplayName(Goal);
			UE_LOG(LogTemp, Log, TEXT("%i Pickups in %s"), Score, *GoalName);
#endif
		}
		else
		{
			GoalMap.Remove(Goal);
		}
	}
}

void UScoreComponent::OnGoalScored(AGoal* Goal, const unsigned int NewScore)
{
	if (!Goal) return;
	
	if (GoalMap.Contains(Goal))
	{
		GoalMap[Goal] = NewScore;
	}
	else
	{
		GoalMap.Add(Goal, NewScore);
	}
}

