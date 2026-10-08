// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TeamEnum.h"
#include "Components/ActorComponent.h"

#include "ScoreComponent.generated.h"

class AGoal;

UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WS_UECPP_API UScoreComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UScoreComponent();
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
protected:
	TMap<AGoal*, unsigned int> GoalMap;
	TMap<ETeam, unsigned int> TeamMap;
	
public:
	void PrintGoalScore();
	void PrintTeamScore();
	void NumberOfPickupsInGoals();
	
protected:
	UFUNCTION()
	void OnGoalScored(AGoal* Goal, unsigned int NewScore);
};
