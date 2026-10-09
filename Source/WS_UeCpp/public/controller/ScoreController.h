// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ScoreController.generated.h"

struct FInputActionValue;
class AMainCharacter;
class UScoreComponent;
class UInputAction;

UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WS_UECPP_API UScoreController : public UActorComponent
{
	GENERATED_BODY()

public:	
	UScoreController();
	
protected:
	TWeakObjectPtr<UScoreComponent> ScoreComponent;
	
#pragma region Input
protected:
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	TObjectPtr<UInputAction> InputActionShowScore = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	TObjectPtr<UInputAction> InputActionCountPickups = nullptr;
	
public:
	void SetupInputComponentScore(TObjectPtr<UInputComponent> InputComponent, AMainCharacter* MainCharacter);
	
protected:
	void OnShowScore(const FInputActionValue& InputActionValue);
	void OnCountPickups();
#pragma endregion
};
