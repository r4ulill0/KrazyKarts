// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKartMovementComponent.h"
#include "GoKart.h"

// Sets default values for this component's properties
UGoKartMovementComponent::UGoKartMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGoKartMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UGoKartMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy || GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy)
	{
		LastMove = CreateMove(DeltaTime);
		SimulateMove(LastMove);
	}
}


void UGoKartMovementComponent::SimulateMove(const FGoKartMove& Move)
{
	if (Mass == 0) return;
	FVector Force = MaxDrivingForce * Move.Throttle * GetOwner()->GetActorForwardVector();
	Force += GetAirResistance();
	Force += GetRollingResistance();
	FVector Acceleration = Force / Mass;

	Velocity = Velocity + Acceleration * Move.DeltaTime;

	UpdateRotation(Move.DeltaTime, Move.SteeringThrow);
	UpdatePositionFromVelocity(Move.DeltaTime);
}

FGoKartMove UGoKartMovementComponent::CreateMove(float DeltaTime)
{
	FGoKartMove Move;
	Move.DeltaTime = DeltaTime;
	Move.SteeringThrow = SteeringThrow;
	Move.Throttle = Throttle;
	Move.Time = GetWorld()->TimeSeconds;

	return Move;
}

FVector UGoKartMovementComponent::GetAirResistance()
{
	return Velocity.GetSafeNormal() * Velocity.SizeSquared() * -1 * DragCoefficient;
}

FVector UGoKartMovementComponent::GetRollingResistance()
{
	GetWorld()->GetGravityZ();
	float AccelerationDueToGravity = - GetWorld()->GetGravityZ()/100;
	return Velocity.GetSafeNormal() * AccelerationDueToGravity * RollingResistanceCoefficient;
}

void UGoKartMovementComponent::UpdateRotation(float DeltaTime, float Steering)
{
	float CoveredDistancePerSecond = FVector::DotProduct(GetOwner()->GetActorForwardVector(), Velocity) * DeltaTime;
	float RotationAngle = CoveredDistancePerSecond /MinSteeringRadius * Steering;
	FQuat RotationDelta(GetOwner()->GetActorUpVector(), RotationAngle);
	GetOwner()->AddActorWorldRotation(RotationDelta);
	Velocity = RotationDelta.RotateVector(Velocity);
}

void UGoKartMovementComponent::UpdatePositionFromVelocity(float DeltaTime)
{
	FVector NewPosition = 100 * Velocity * DeltaTime;
	FHitResult hitResult;
	GetOwner()->AddActorWorldOffset(NewPosition, true, &hitResult);
	if (hitResult.IsValidBlockingHit()) Velocity = FVector::ZeroVector;
}