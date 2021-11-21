// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"

UCLASS()
class KRAZYKARTS_API AGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoKart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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
private:
	FVector GetAirResistance();
	FVector GetRollingResistance();
	void MoveForward(float Value);
	void MoveRight(float Value);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveForward(float Value);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveRight(float Value);
	void UpdatePositionFromVelocity(float DeltaTime);
	void UpdateRotation(float DeltaTime);

	UPROPERTY(Replicated)
	FVector ReplicatedLocation;
	UPROPERTY(Replicated)
	FRotator ReplicatedRotation;
	FVector Velocity;
	float Throttle;
	float SteeringThrow; 
};
