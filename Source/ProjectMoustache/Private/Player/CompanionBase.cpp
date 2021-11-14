// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CompanionBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACompanionBase::ACompanionBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set default values
	baseMoveSpeed = 100;
	maxSpeed = 5000;
	maxSpeedRange = 5000;
	baseRotationRate = 3;
	velocityInterpolationRate = 3;
	updateOffsetInterval = 5;

	checkChangeSidesIntervalRange = FVector2D(10, 20);
	followPositionOffset = FVector(-100, 100, 100);
	followDistanceThreshold = 100;
	offsetAmount = FVector(20, 20, 50);

	// Create mesh and attach it to the root
	meshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	meshComponent->SetupAttachment(RootComponent);
	
	// Create the floating pawn movement component
	floatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));

}

// Called when the game starts or when spawned
void ACompanionBase::BeginPlay()
{
	Super::BeginPlay();

	// Get reference to player's interface
	AActor* player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (player->GetClass()->ImplementsInterface(UPlayerCharacter::StaticClass()))
	{
		playerInterface.SetInterface(Cast<IPlayerCharacter>(player));
		playerInterface.SetObject(player);
	}

	// If no player interface found, destroy
	if (playerInterface == nullptr)
	{
		Destroy();
	}

	// Subscribe as an observer of the player
	playerInterface->Execute_SubscribeAsObserver(playerInterface.GetObject(), this);

	// Set default follow positions
	float playerRotation = playerInterface->Execute_GetPlayerRotation(playerInterface.GetObject()).Yaw;
	baseFollowPosition = playerInterface->Execute_GetPlayerLocation(playerInterface.GetObject()) + GetRotatedVector(followPositionOffset, playerRotation);
	wantedFollowLocation = baseFollowPosition;
	
	// Set update tick timers
	UWorld* world = GetWorld();

	// Set change sides interval
	float randInterval = FMath::RandRange(checkChangeSidesIntervalRange.X, checkChangeSidesIntervalRange.Y);
	timeNextCheckSwitchSides = world->GetTimeSeconds() + randInterval;
}

// Called every frame
void ACompanionBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateFollowPositionOffset();
	UpdateFollowVelocity(DeltaTime);
}

/**
* Updates the following offset to a random value, and randomly changes the side to follow on
*/
void ACompanionBase::UpdateFollowPositionOffset()
{
	// Switch the side the companion follows on on an interval
	// Skip if the player is aiming
	if (!playerIsAiming && GetWorld()->GetTimeSeconds() > timeNextCheckSwitchSides)
	{
		followPositionOffset.Y *= -1;
		float randInterval = FMath::RandRange(checkChangeSidesIntervalRange.X, checkChangeSidesIntervalRange.Y);
		timeNextCheckSwitchSides = GetWorld()->GetTimeSeconds() + randInterval;
	}

	// Sets a random offset to give the floating effect
	float playerVelocity = playerInterface->Execute_GetCurrentPlayerVelocity(playerInterface.GetObject());
	float playerRotation = playerInterface->Execute_GetPlayerRotation(playerInterface.GetObject()).Yaw;
	baseFollowPosition = playerInterface->Execute_GetPlayerLocation(playerInterface.GetObject()) + GetRotatedVector(followPositionOffset, playerRotation);

	if (GetWorld()->GetTimeSeconds() > timeNextCheckOffset)
	{
		FVector offset = GetRotatedVector(offsetAmount, GetActorRotation().Yaw);
		if (playerVelocity > baseMoveSpeed)
		{
			offset = GetRotatedVector(FVector(0, offsetAmount.Y, offsetAmount.Z), playerRotation);
		}
		currentOffset = FVector(FMath::RandRange(offset.X * -1, offset.X),
			FMath::RandRange(offset.Y * -1, offset.Y),
			FMath::RandRange(offset.Z * -1, offset.Z));

		timeNextCheckOffset = GetWorld()->GetTimeSeconds() + updateOffsetInterval;
	}

	wantedFollowLocation = baseFollowPosition + currentOffset;
}

/**
* Updates the following velocity
*/
void ACompanionBase::UpdateFollowVelocity(float deltaTime)
{
	// Set direction and speed to follow based on player movement and distance from follow point
	FVector movementDirection = wantedFollowLocation - GetActorLocation();
	movementDirection.Normalize();
	float distToFollowPosition = FVector::Dist(GetActorLocation(), baseFollowPosition);
	float playerVelocity = playerInterface->Execute_GetCurrentPlayerVelocity(playerInterface.GetObject());
	float speed = playerVelocity > baseMoveSpeed ? playerVelocity + 150 : baseMoveSpeed;

	// Follows faster if outside of the follow threshold
	if (distToFollowPosition > followDistanceThreshold)
	{
		float distRange = distToFollowPosition - followDistanceThreshold;
		float multiplier = distRange / maxSpeedRange;
		
		speed = FMath::Clamp((maxSpeed * multiplier) + playerVelocity + 150, baseMoveSpeed + playerVelocity, maxSpeed);
	}

	// Set speed higher if aiming, so companion gets out of the way faster
	if (playerIsAiming)
	{
		speed = 1500;
	}

	// Apply speed and interpolate the velocity smoothly
	movementDirection *= speed;
	floatingPawnMovement->Velocity = FMath::VInterpTo(floatingPawnMovement->Velocity, movementDirection,
		deltaTime, velocityInterpolationRate);

	// Update rotation
	FRotator movementRotation = UKismetMathLibrary::FindLookAtRotation(GetActorForwardVector(), movementDirection);
	FRotator wantedRotation = movementRotation;
	float rotationSpeed = 3;
	float rotationOffset = 15;
	
	// If within the threshold, maintain player facing direction, and limit movement direction
	if (distToFollowPosition < followDistanceThreshold)
	{
		FRotator playerRotation = playerInterface->Execute_GetPlayerRotation(playerInterface.GetObject());
		rotationSpeed = baseRotationRate;
		wantedRotation = FRotator(FMath::Clamp(movementRotation.Pitch, playerRotation.Pitch - rotationOffset, playerRotation.Pitch + rotationOffset),
			FMath::Clamp(movementRotation.Yaw, playerRotation.Yaw - rotationOffset, playerRotation.Yaw + rotationOffset),
			FMath::Clamp(movementRotation.Roll, playerRotation.Roll - rotationOffset, playerRotation.Roll + rotationOffset));
	}
	// Otherwise clamp the movement rotation to the base direction
	else
	{
		rotationOffset = 5;
		FVector baseMovementDirection = baseFollowPosition - GetActorLocation();
		FRotator baseMovementRotation = UKismetMathLibrary::FindLookAtRotation(GetActorForwardVector(), baseMovementDirection);
		wantedRotation = FRotator(FMath::Clamp(movementRotation.Pitch, baseMovementRotation.Pitch - rotationOffset, baseMovementRotation.Pitch + rotationOffset),
			FMath::Clamp(movementRotation.Yaw, baseMovementRotation.Yaw - rotationOffset, baseMovementRotation.Yaw + rotationOffset),
			FMath::Clamp(movementRotation.Roll, baseMovementRotation.Roll - rotationOffset, baseMovementRotation.Roll + rotationOffset));
	}

	// Smoothly interpolate rotation, and apply it to rotation
	FRotator smoothRotation = FMath::RInterpTo(GetActorRotation(), wantedRotation,
		deltaTime, rotationSpeed);
	SetActorRotation(smoothRotation);
}

/**
 * Rotates the offset vector based on the player's rotation, so the offset is oriented to the player the same
 * in any direction
 */
FVector ACompanionBase::GetRotatedVector(const FVector inVector, const float inAngle)
{
	float angle = FMath::DegreesToRadians(inAngle);
	float x = (FMath::Cos(angle) * inVector.X) - (FMath::Sin(angle) * inVector.Y);
	float y = (FMath::Sin(angle) * inVector.X) + (FMath::Cos(angle) * inVector.Y);

	return FVector(x, y, inVector.Z);
}


/**
* Notifies observers that the player has began aiming
*/
void ACompanionBase::OnPlayerBeginAiming_Implementation()
{
	playerIsAiming = true;
	
	// Set companion's wanted position to the player's left side so they are out of view
	if (followPositionOffset.Y > 0)
	{
		followPositionOffset.Y *= -1;
	}
}

/**
* Notifies observers that the player is no longer aiming
*/
void ACompanionBase::OnPlayerEndAiming_Implementation()
{
	float randInterval = FMath::RandRange(checkChangeSidesIntervalRange.X, checkChangeSidesIntervalRange.Y);
	timeNextCheckSwitchSides = GetWorld()->GetTimeSeconds() + randInterval;
	playerIsAiming = false;
}