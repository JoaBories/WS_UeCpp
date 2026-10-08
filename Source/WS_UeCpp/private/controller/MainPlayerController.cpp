// Fill out your copyright notice in the Description page of Project Settings.
#include "controller/MainPlayerController.h"

#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputActionValue.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "controller/GravityGunController.h"
#include "controller/ScoreController.h"

#include "gameplay/MainCharacter.h"

void AMainPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	if (!Character.IsValid()) 
	{
		Character = Cast<AMainCharacter>(InPawn);
		
		// Get gravity gun controller
		GravityGunController = FindComponentByClass<UGravityGunController>();
		if (GravityGunController)
		{
			GravityGunController->SetupInputComponentGravityGun(InputComponent, Character.Get());
		}
		
		// Get score controller
		ScoreController = FindComponentByClass<UScoreController>();
		if (ScoreController)
		{
			ScoreController->SetupInputComponentScore(InputComponent, Character.Get());
		}
	}
}

void AMainPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Check pointers
	const bool bAllPointersChecked = InputMappingContext && InputActionMove && InputActionLook;
	if (bAllPointersChecked == false) 
	{
		UE_LOG(LogTemp, Error, TEXT("Missing Input Action or Mapping Context"));
		return;
	}

	// Get subsystem
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (!EnhancedInputSubsystem) return;

	// Clear and bind IMC
	EnhancedInputSubsystem->ClearAllMappings();
	EnhancedInputSubsystem->AddMappingContext(InputMappingContext, 0);

	// Get Enhanced input comp
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent) return;

	// Bind input action
	EnhancedInputComponent->BindAction(InputActionMove, ETriggerEvent::Triggered, this, &AMainPlayerController::Move);
	EnhancedInputComponent->BindAction(InputActionLook, ETriggerEvent::Triggered, this, &AMainPlayerController::Look);
	EnhancedInputComponent->BindAction(InputActionJump, ETriggerEvent::Triggered, this, &AMainPlayerController::Jump);
}

void AMainPlayerController::Move(const FInputActionValue& InputActionValue)
{
	// Check character
	if (!Character.IsValid()) return;

	const FVector2D MovementValue = InputActionValue.Get<FVector2D>();
	
	// Move Forward
	if (MovementValue.Y) 
	{
		Character->AddMovementInput(Character->GetActorForwardVector(), MovementValue.Y);
	}

	// Move Right
	if (MovementValue.X)
	{
		Character->AddMovementInput(Character->GetActorRightVector(), MovementValue.X);
	}
	
	//UE_LOG(LogTemp, Log, TEXT("Movement value : %s"), *MovementValue.ToString());
}

void AMainPlayerController::Look(const FInputActionValue& InputActionValue)
{
	// Check character
	if (!Character.IsValid()) return;

	// Get look value
	const FVector2D LookValue = InputActionValue.Get<FVector2D>();

	// Update Yaw
	if (LookValue.X)
	{
		Character->AddControllerYawInput(LookValue.X * MouseXSensisivity);
	}

	// Update Pitch
	if (LookValue.Y)
	{
		Character->AddControllerPitchInput((MouseYInvert ? -1.0f : 1.0f) * LookValue.Y * MouseYSensisivity);
	}
}

void AMainPlayerController::Jump() 
{
	// Check character
	if (!Character.IsValid()) return;

	Character->Jump();
}
