// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GravityGunComponent.generated.h"

class UPickupComponent;
class UStaticMeshComponent;
class AMainCharacter;
class APlayerCameraManager;

UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WS_UECPP_API UGravityGunComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGravityGunComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

public:	
	void OnTakeObjectInputPressed();
	
	void OnThrowObjectInputPressed();
	void OnThrowObjectInputReleased();
	
	void OnAdditionalMultInput(bool State);
	
	void OnUpdateReach(float Value);
	
protected:
	TWeakObjectPtr<APlayerCameraManager> PlayerCameraManager = nullptr;
	TWeakObjectPtr<AMainCharacter> Character = nullptr;
	
#pragma region Collisions
protected:
	UPROPERTY(EditDefaultsOnly, Category="Gravity Gun")
	TEnumAsByte<ETraceTypeQuery> GravityGunTraceChannel;
	ECollisionChannel GravityGunCollisionChannel;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Reach", meta = (ClampMin = "0.0", ClampMax = "1000.0", Units = "Centimeters"))
	float GravityGunMinReach = 100.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Reach", meta = (ClampMin = "0.0", ClampMax = "1000.0", Units = "Centimeters"))
	float GravityGunMaxReach = 1000.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Reach", meta = (ClampMin = "0.0", ClampMax = "1000.0", Units = "Centimeters"))
	float GravityGunReach = 500.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Reach", meta = (ClampMin = "0.0", ClampMax = "100.0", Units = "CentimetersPerSecond"))
	float GravityGunReachChangerate = 10.0f;
#pragma endregion
	
#pragma region Pickups
protected:
	TWeakObjectPtr<AActor> CurrentPickup = nullptr;
	TWeakObjectPtr<UPickupComponent> CurrentPickupComponent = nullptr;
	TWeakObjectPtr<UStaticMeshComponent> CurrentPickupStaticMesh = nullptr;
	FName PreviousCollisionProfile = NAME_None;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Hold", meta = (ClampMin = "-100.0", ClampMax = "100.0", Units = "Centimeters"))
	float PickupHeightOffset = -30.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Hold", meta = (ClampMin = "0.0", ClampMax = "1000.0", Units = "Centimeters"))
	float PickupHoldMinDistance = 100.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Hold", meta = (ClampMin = "0.0", ClampMax = "1000.0", Units = "Centimeters"))
	float PickupHoldMaxDistance = 500.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Hold", meta = (ClampMin = "0.0", ClampMax = "1000.0", Units = "Centimeters"))
	float PickupHoldDistance = 100.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Hold", meta = (ClampMin = "0.0", ClampMax = "100.0", Units = "CentimetersPerSecond"))
	float PickupHoldChangerate = 10.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Throw", meta = (ClampMin = "0.0", ClampMax = "10000.0", Units = "CentimetersPerSecond"))
	float PickupThrowForce = 3000.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Throw", meta = (ClampMin = "0.0", ClampMax = "20.0"))
	float ThrowMaxHoldMult = 5.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Throw", meta = (ClampMin = "0.0", ClampMax = "30.0", Units = "Seconds"))
	float ThrowMaxHoldTime = 2.0f;
	
	float TimeThrowPressed = 0.0f;
	bool bThrowPressed = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Throw", meta = (ClampMin = "0.0", ClampMax = "20.0"))
	float ThrowAdditionalMult = 5.0f;
	
	bool bAdditionalMult = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Throw")
	FVector PickupAngularForce = FVector(2000.0f, 2000.0f, 2000.0f);
protected:
	void UpdatePickupLocation();
	void ReleasePickup(bool bThrow = false);
	
	UFUNCTION()
	void OnPickupDestroyed();
#pragma endregion
	
#pragma region Debug
protected :
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Debug")
	bool bDrawDebugLine = false;
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Debug", meta = (ClampMin = "0.0", ClampMax = "10.0", Units = "Seconds", EditCondition = "bDrawDebugLine", EditConditionHides = "bDrawDebugLine"))
	float DrawDebugTime = 1.0f;
#pragma endregion
};
