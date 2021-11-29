// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"


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
USTRUCT(BlueprintType)
struct FGoKartState
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FTransform Transform;
	UPROPERTY()
	FGoKartMove LastMove;
	UPROPERTY()
	FVector Velocity;
};
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
	void SimulateMove(FGoKartMove Move);
	FVector GetAirResistance();
	FVector GetRollingResistance();
	void MoveForward(float Value);
	void MoveRight(float Value);
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FGoKartMove Move);
	void UpdatePositionFromVelocity(float DeltaTime);
	void UpdateRotation(float DeltaTime, float SteeringThrow);

	UPROPERTY(ReplicatedUsing=OnRep_ServerState)
	FGoKartState ServerState;

	FVector Velocity;
	UFUNCTION()
	void OnRep_ServerState();

	float Throttle;
	float SteeringThrow; 
};
