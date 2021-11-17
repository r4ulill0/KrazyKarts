// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"
#include "Engine/World.h"

// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Mass == 0) return;
	FVector Force = MaxDrivingForce * Throttle * GetActorForwardVector();
	Force += GetAirResistance();
	Force += GetRollingResistance();
	FVector Acceleration = Force / Mass;

	Velocity = Velocity + Acceleration * DeltaTime;

	UpdateRotation(DeltaTime);
	UpdatePositionFromVelocity(DeltaTime);
}

void AGoKart::UpdateRotation(float DeltaTime)
{
	float CoveredDistancePerSecond = FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime;
	float RotationAngle = CoveredDistancePerSecond /MinSteeringRadius * SteeringThrow;
	// float RotationAngle = SteeringThrow * MinSteeringRadius * DeltaTime;
	FQuat RotationDelta(GetActorUpVector(), RotationAngle);
	AddActorWorldRotation(RotationDelta);
	Velocity = RotationDelta.RotateVector(Velocity);
}

void AGoKart::UpdatePositionFromVelocity(float DeltaTime)
{
	FVector NewPosition = 100 * Velocity * DeltaTime;
	FHitResult hitResult;
	AddActorWorldOffset(NewPosition, true, &hitResult);
	if (hitResult.IsValidBlockingHit()) Velocity = FVector::ZeroVector;
}
// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGoKart::Server_MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGoKart::Server_MoveRight);

}

void AGoKart::Server_MoveForward_Implementation(float Value)
{
	Throttle = Value;
}

bool AGoKart::Server_MoveForward_Validate(float Value)
{
	return FMath::Abs(Value)<=1;
}

void AGoKart::Server_MoveRight_Implementation(float Value)
{
	SteeringThrow = Value;
}

bool AGoKart::Server_MoveRight_Validate(float Value)
{
	return FMath::Abs(Value)<=1;
}

FVector AGoKart::GetAirResistance()
{
	return Velocity.GetSafeNormal() * Velocity.SizeSquared() * -1 * DragCoefficient;
}
FVector AGoKart::GetRollingResistance()
{
	GetWorld()->GetGravityZ();
	float AccelerationDueToGravity = - GetWorld()->GetGravityZ()/100;
	return Velocity.GetSafeNormal() * AccelerationDueToGravity * RollingResistanceCoefficient;
}