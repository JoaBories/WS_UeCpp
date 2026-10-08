// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Goal.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGoalScored, AGoal*, Goal, unsigned int, NewScore);

class UBoxComponent;

UCLASS(Abstract)
class WS_UECPP_API AGoal : public AActor
{
	GENERATED_BODY()
	
public:	
	AGoal(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:
	FOnGoalScored GoalScored;
	
#pragma region CollisionBox
public:
	unsigned int CountPickupInGoal();
	
protected:
	UFUNCTION()
	void OnBoxComponentOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> BoxComponent = nullptr;
	unsigned int Score = 0;
	
	UPROPERTY(EditDefaultsOnly, Category="Goal")
	TEnumAsByte<ETraceTypeQuery> GoalTraceChannel;
#pragma endregion
};
