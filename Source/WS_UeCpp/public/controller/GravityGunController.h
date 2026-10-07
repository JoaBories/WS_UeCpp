// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GravityGunController.generated.h"

class UGravityGunComponent;
class AMainCharacter;
class UInputAction;
class UInputComponent;
struct FInputActionValue;

UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WS_UECPP_API UGravityGunController : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGravityGunController();
	
protected:
	TWeakObjectPtr<UGravityGunComponent> GravityGunComponent;
	
#pragma region Input
protected:
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	TObjectPtr<UInputAction> InputActionTake = nullptr;	
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	TObjectPtr<UInputAction> InputActionThrow = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "EnhancedInput")
	TObjectPtr<UInputAction> InputActionUpdateReach = nullptr;
	
public:
	void SetupInputComponentGravityGun(TObjectPtr<UInputComponent> InputComponent, AMainCharacter* MainCharacter);
	
protected:
	void OnTakeObject(const FInputActionValue& Value);
	void OnThrowObject(const FInputActionValue& Value);
	void OnUpdateReach(const FInputActionValue& Value);
#pragma endregion
};
