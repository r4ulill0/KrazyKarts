// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoKartMovementComponent.h"
#include "GoKartMovementReplicator.generated.h"


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

struct FHermiteCubicSpline
{

	FVector TargetLocation, StartLocation, StartDerivative, TargetDerivative;

	FVector InterpolateSpline(float LerpRatio) const { return FMath::CubicInterp(StartLocation, StartDerivative, TargetLocation, TargetDerivative, LerpRatio);}
	FVector InterpolateDerivative(float LerpRatio) const { return FMath::CubicInterpDerivative(StartLocation, StartDerivative, TargetLocation, TargetDerivative, LerpRatio);};

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRAZYKARTS_API UGoKartMovementReplicator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGoKartMovementReplicator();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FGoKartState GetServerState() {return ServerState;};
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FGoKartMove Move);

private:
	void ClearAcknowledgedMoves(FGoKartMove LastMove);
	void UpdateServerState(const FGoKartMove& Move);
	void ClientTick(float DeltaTime);
	FHermiteCubicSpline CreateSpline();
	void InterpolateLocation(FHermiteCubicSpline& Spline, float LerpRatio);
	void InterpolateVelocity(FHermiteCubicSpline& Spline, float LerpRatio);
	void InterpolateRotation(float LerpRatio);
	float VelocityToDerivative();

	UPROPERTY(ReplicatedUsing=OnRep_ServerState)
	FGoKartState ServerState;

	UFUNCTION()
	void OnRep_ServerState();
	void AutonomousProxy_ServerState();
	void SimulatedProxy_OnRep_ServerState();

	TArray<FGoKartMove> UnacknowledgedMoves;

	float ClientTimeSinceUpdate;
	float ClientTimeBetweenLastUpdates;
	FTransform ClientStartTransform;
	FVector ClientStartVelocity;


	UPROPERTY()
	UGoKartMovementComponent* MovementComponent;
};
