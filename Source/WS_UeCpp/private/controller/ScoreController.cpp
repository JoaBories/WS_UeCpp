// Fill out your copyright notice in the Description page of Project Settings.


#include "controller/ScoreController.h"

#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"

#include "gameplay/ScoreComponent.h"
#include "gameplay/MainCharacter.h"

UScoreController::UScoreController()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UScoreController::SetupInputComponentScore(TObjectPtr<UInputComponent> InputComponent,
	AMainCharacter* MainCharacter)
{
	const bool bAllPointersChecked = InputComponent && MainCharacter && InputActionShowScore;
	if (!bAllPointersChecked) return;
	
	// Get gravity gun comp
	ScoreComponent = MainCharacter->FindComponentByClass<UScoreComponent>();
	
	// Cast to Enhanced inputs
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent) return;
	
	// Bind input action
	EnhancedInputComponent->BindAction(InputActionShowScore, ETriggerEvent::Triggered, this, &UScoreController::OnShowScore);
	EnhancedInputComponent->BindAction(InputActionCountPickups, ETriggerEvent::Triggered, this, &UScoreController::OnCountPickups);
}

void UScoreController::OnShowScore(const FInputActionValue& InputActionValue)
{
	if (ScoreComponent.IsValid())
	{
		const float MovementValue = InputActionValue.Get<float>();
		
		if (MovementValue > 0) ScoreComponent->PrintGoalScore();
		else ScoreComponent->PrintTeamScore();
	}
}

void UScoreController::OnCountPickups()
{
	if (ScoreComponent.IsValid())
	{
		ScoreComponent->NumberOfPickupsInGoals();
	}
}
