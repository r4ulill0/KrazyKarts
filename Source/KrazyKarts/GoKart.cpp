// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGoKart::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGoKart, ReplicatedLocation);
	DOREPLIFETIME(AGoKart, ReplicatedRotation);
}

// This was created because yolo, it could be used UEnum::GetValueAsString(GetLocalRole(), value); instead
FString GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "None";
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	case ROLE_Authority:
		return "Authority";
	default:
		return "None";
	}
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

	if (HasAuthority())
	{
		ReplicatedLocation = GetActorLocation();
		ReplicatedRotation = GetActorRotation();
	}
	else
	{
		SetActorLocation(ReplicatedLocation);
		SetActorRotation(ReplicatedRotation);
	}

	DrawDebugString(GetWorld(), FVector(0,0,100), GetEnumText(GetLocalRole()), this, FColor::White, DeltaTime);
}

void AGoKart::UpdateRotation(float DeltaTime)
{
	float CoveredDistancePerSecond = FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime;
	float RotationAngle = CoveredDistancePerSecond /MinSteeringRadius * SteeringThrow;
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
	PlayerInputComponent->BindAxis("MoveForward", this, &AGoKart::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGoKart::MoveRight);

}

void AGoKart::MoveForward(float Value)
{
	Throttle = Value;
	Server_MoveForward(Value);
}

void AGoKart::MoveRight(float Value)
{
	SteeringThrow = Value;
	Server_MoveRight(Value);
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