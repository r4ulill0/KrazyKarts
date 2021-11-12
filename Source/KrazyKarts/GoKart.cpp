// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"

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
	FVector Acceleration = Force / Mass;
	Velocity = Velocity + Acceleration * DeltaTime;
	updatePositionFromVelocity();
}

void AGoKart::updatePositionFromVelocity()
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

	PlayerInputComponent->BindAxis("MoveForward", this, &AGoKart::MoveForward);
}

void AGoKart::MoveForward(float Value)
{
	Throttle = Value;
}