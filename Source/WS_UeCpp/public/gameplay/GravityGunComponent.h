// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GravityGunComponent.generated.h"

class UGravityGunDataAsset;
class UPickupComponent;
class UStaticMeshComponent;
class AMainCharacter;
class UCurveFloat;
class APlayerCameraManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickupTakenDelegate, AActor*, PickupActor);

UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WS_UECPP_API UGravityGunComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGravityGunComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	void OnUpdateMinReach();
	void OnUpdateMaxReach();
#endif
	
	virtual void BeginPlay() override;

public:	
	void OnTakeObject();
	
	void OnThrowObjectPressed();
	void OnThrowObjectReleased();
	
	void OnDestroyObject();
	
	void OnAdditionalMult(bool State);
	
	void OnUpdateReach(float Value);

	bool TryGrabPickup(AActor* Actor);
	
public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category= "Gravity Gun")
	FOnPickupTakenDelegate PickupTaken;
	
protected:
	TWeakObjectPtr<APlayerCameraManager> PlayerCameraManager = nullptr;
	TWeakObjectPtr<AMainCharacter> Character = nullptr;
	
#pragma region Collisions
protected:
	UPROPERTY(EditDefaultsOnly, Category="Gravity Gun")
	TEnumAsByte<ETraceTypeQuery> GravityGunTraceChannel;
	ECollisionChannel GravityGunCollisionChannel;
	
	UPROPERTY(EditDefaultsOnly, Category="Gravity Gun")
	float SphereTraceRadius = 10.0f;
	
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
	
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Throw", meta = (ClampMin = "0.0", ClampMax = "20.0"))
	float ThrowMaxHoldMult = 5.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Throw", meta = (ClampMin = "0.0", ClampMax = "30.0", Units = "Seconds"))
	float ThrowMaxHoldTime = 2.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Throw")
	TObjectPtr<UCurveFloat> ThrowHoldCurve = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Throw")
	TObjectPtr<UGravityGunDataAsset> GravityGunDataAsset = nullptr;
	
	float TimeThrowPressed = 0.0f;
	bool bThrowPressed = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Throw", meta = (ClampMin = "0.0", ClampMax = "20.0"))
	float ThrowAdditionalMult = 5.0f;
	
	bool bAdditionalMult = false;

public:
	UFUNCTION(BlueprintPure, Category= "Gravity Gun")
	float GetThrowMaxHoldTime() const;
	UFUNCTION(BlueprintPure, Category= "Gravity Gun")
	float GetThrowTime() const;

protected:
	void UpdatePickupLocation();
	void ReleasePickup(bool bThrow = false);
	
	bool ShootPickupSphereTrace(float Radius, float Length, FHitResult& Hit, bool bDrawDebugTrace = false);
	
	UFUNCTION()
	void OnPickupDestroyed(UPickupComponent* PickupComponent);
#pragma endregion
	
#pragma region Debug
protected :
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Debug")
	bool bDrawDebug = false;
	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun|Debug", meta = (ClampMin = "0.0", ClampMax = "10.0", Units = "Seconds", EditCondition = "bDrawDebug", EditConditionHides = "bDrawDebugLine"))
	float DrawDebugTime = 1.0f;
#pragma endregion
};
