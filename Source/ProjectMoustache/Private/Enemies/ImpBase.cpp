// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/ImpBase.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

AImpBase::AImpBase()
{
	PrimaryActorTick.bCanEverTick = true;

	
	bodyCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BodyCapsule"));
	bodyCollider->SetupAttachment(GetCapsuleComponent());

	GetMesh()->SetupAttachment(bodyCollider);

	runSpeed = 500;
	walkSpeed = 300;

	glitchSpeed = 2;
	glitchCooldown = 3;
	isGlitching = false;

	floatingOffsetAmount = FVector(20, 20, 20);
	updateOffsetInterval = 3.0f;
	floatingSpeed = 1;
}

void AImpBase::BeginPlay()
{
	Super::BeginPlay();

	if (glitchSpeed <= 0)
	{
		glitchSpeed = 2;
	}

	if (glitchCooldown < 0)
	{
		glitchCooldown = 3;
	}

	if (updateOffsetInterval <= 0)
	{
		updateOffsetInterval = 3.0f;
	}

	baseLocation = bodyCollider->GetRelativeLocation();

	floatOffset = FVector(FMath::RandRange(floatingOffsetAmount.X * -1, floatingOffsetAmount.X),
		FMath::RandRange(floatingOffsetAmount.Y * -1, floatingOffsetAmount.Y),
		FMath::RandRange(floatingOffsetAmount.Z * - 1, floatingOffsetAmount.Z));
	floatOffset += baseLocation;
	timeNextUpdateOffset = GetWorld()->GetTimeSeconds() + updateOffsetInterval;

	interpFrom = bodyCollider->GetRelativeLocation();
}

void AImpBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetIsDead())
	{
		return;
	}

	// Lock mesh look at target
	if (lockedOnTarget && target != nullptr)
	{
		FVector dir = target->GetActorLocation() - bodyCollider->GetComponentLocation();
		FRotator rotation = UKismetMathLibrary::MakeRotFromXZ(dir, FVector::UpVector);
		bodyCollider->SetWorldRotation(rotation);
	}

	if (isGlitching)
	{
		UpdateGlitch(DeltaSeconds);
		return;
	}
	
	FloatingMovement(DeltaSeconds);
}

void AImpBase::FloatingMovement(float deltaTime)
{
	const UWorld* world = GetWorld();	
	if (world->GetTimeSeconds() >= timeNextUpdateOffset)
	{
		floatOffset = FVector(FMath::RandRange(floatingOffsetAmount.X * -1, floatingOffsetAmount.X),
		FMath::RandRange(floatingOffsetAmount.Y * -1, floatingOffsetAmount.Y),
		FMath::RandRange(floatingOffsetAmount.Z * - 1, floatingOffsetAmount.Z));
		floatOffset += baseLocation;
		timeNextUpdateOffset = world->GetTimeSeconds() + updateOffsetInterval;

		interpFrom = bodyCollider->GetRelativeLocation();
	}

	float verticalPosition = floatingOffsetAmount.Z * FMath::Sin(verticalFloatCounter) + baseLocation.Z;
	verticalFloatCounter += deltaTime * floatingSpeed;
	floatOffset.Z = verticalPosition;
	/*const FVector smoothFloatLocation = FMath::VInterpTo(bodyCollider->GetRelativeLocation(),
	                                                     floatOffset, deltaTime, floatingSpeed);*/
	const FVector smoothFloatLocation = FMath::VInterpTo(interpFrom,
														floatOffset, deltaTime, floatingSpeed);
	interpFrom.Z = bodyCollider->GetRelativeLocation().Z;
	bodyCollider->SetRelativeLocation(smoothFloatLocation);
}

bool AImpBase::GlitchToLocation(FVector location)
{
	if (GetWorld()->GetTimeSeconds() < timeNextCanGlitch)
	{
		return false;
	}
	
	beforeGlitchLocation = GetActorLocation();
	glitchLocation = location;
	isGlitching = true;
	bodyCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	return true;
}

void AImpBase::UpdateGlitch(float deltaTime)
{
	const FVector updatedLocation = FMath::Lerp(beforeGlitchLocation, glitchLocation, glitchAlpha);
	SetActorLocation(updatedLocation);
	glitchAlpha += deltaTime * glitchSpeed;

	if (glitchAlpha >= 1)
	{
		glitchAlpha = 0;
		timeNextUpdateOffset = 0;
		isGlitching = false;
		bodyCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		timeNextCanGlitch = GetWorld()->GetTimeSeconds() + glitchCooldown;
	}
}

void AImpBase::LockOnTarget(AActor* lockTarget)
{
	target = lockTarget;
	bUseControllerRotationYaw = false;
	lockedOnTarget = true;
}

void AImpBase::UnlockTarget()
{
	target = nullptr;
	bUseControllerRotationYaw = true;
	lockedOnTarget = false;

	bodyCollider->SetRelativeRotation(FRotator(0, 0, 0));
}

void AImpBase::SetMoveSpeed(bool running)
{
	GetCharacterMovement()->MaxFlySpeed = running ? runSpeed : walkSpeed;
}
