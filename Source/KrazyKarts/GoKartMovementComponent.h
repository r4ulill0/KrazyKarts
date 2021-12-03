// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoKartMovementComponent.generated.h"

USTRUCT()
struct FGoKartMove
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY()
	float Throttle;
	UPROPERTY()
	float SteeringThrow; 

	UPROPERTY()
	float DeltaTime;
	UPROPERTY()
	float Time;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAZYKARTS_API UGoKartMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGoKartMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Mass of the car
	UPROPERTY(EditAnywhere)
	float Mass = 1000;

	/** Force applied by the car engine */
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000;

	/** Radius of the minimum circle the car can rotate around with maximum steering */
	UPROPERTY(EditAnywhere)
	float MinSteeringRadius = 10;

	// Air drag resistance
	UPROPERTY(EditAnywhere)
	float DragCoefficient = 16;

	// Ground rolling drag resistance
	UPROPERTY(EditAnywhere)
	float RollingResistanceCoefficient = 0.015;

	void SimulateMove(const FGoKartMove& Move);
	FGoKartMove CreateMove(float DeltaTime);

	void SetVelocity(FVector Value) {Velocity = Value;};
	void SetThrottle(float Value) {Throttle = Value;};
	void SetSteeringThrow(float Value) {SteeringThrow = Value;};
	FVector GetVelocity() {return Velocity;};

private:
	FVector GetAirResistance();
	FVector GetRollingResistance();
	void UpdatePositionFromVelocity(float DeltaTime);
	void UpdateRotation(float DeltaTime, float SteeringThrow);
	FVector Velocity;
	float Throttle;
	float SteeringThrow; 
};
