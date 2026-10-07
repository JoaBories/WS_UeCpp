// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

class AMainCharacter;

/**
 * Main controller of the player
 */
UCLASS(Abstract)
class WS_UECPP_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void SetPawn(APawn* InPawn) override;

protected:
	virtual void SetupInputComponent() override;

protected:
	TWeakObjectPtr<AMainCharacter> Character = nullptr;
	
#pragma region Inputs
protected:
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput", meta = (ToolTip = "Main IMC of the controller"))
	TObjectPtr<UInputMappingContext> InputMappingContext = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	TObjectPtr<UInputAction> InputActionMove = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	TObjectPtr<UInputAction> InputActionLook = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	TObjectPtr<UInputAction> InputActionJump = nullptr;

protected:
	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);
	void Jump();
#pragma endregion

#pragma region MouseSensitivity
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Mouse", meta = (ClampMin = "0.0", ClampMax = "5.0"))
	float MouseXSensisivity = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Mouse", meta = (ClampMin = "0.0", ClampMax = "5.0"))
	float MouseYSensisivity = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Mouse")
	bool MouseYInvert = true;
#pragma endregion
};