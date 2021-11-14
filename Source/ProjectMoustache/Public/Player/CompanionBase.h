// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "PlayerObserver.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/Pawn.h"
#include "CompanionBase.generated.h"

UCLASS()
class PROJECTMOUSTACHE_API ACompanionBase : public APawn, public IPlayerObserver
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UFloatingPawnMovement* floatingPawnMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* meshComponent;
	
	// Reference to the player's interface
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TScriptInterface<IPlayerCharacter> playerInterface;

	// The speed the companion will float at while stationary
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	float baseMoveSpeed;

	// Max move speed
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	float maxSpeed;

	// Range at which max speed is used
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	float maxSpeedRange;

	// Tick delay to update offset
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Timers, meta = (AllowPrivateAccess = true))
	float updateOffsetInterval;

	// Rate of rotation interpolation
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	float baseRotationRate;
	
	// Rate of velocity interpolation
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	float velocityInterpolationRate;

	// Offset of the follow position
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	FVector followPositionOffset;

	// The distance from the wanted location at which the companion will begin to catch up
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	float followDistanceThreshold;
	
	// Used to randomize the offset with positive and negative values
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	FVector offsetAmount;

	// Base position companion is following
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FVector baseFollowPosition;
	
	// Position that the companion is following based on
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FVector wantedFollowLocation;

	// Holds onto the current offset from the base location
	FVector currentOffset;

	// Delay for checking change sides from left to right
	UPROPERTY(EditDefaultsOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	FVector2D checkChangeSidesIntervalRange;
	
	// Time check for switching from left to right side
	float timeNextCheckSwitchSides;
	
	// Time to update offset
	float timeNextCheckOffset;
	
	bool playerIsAiming;

	/**
	 * Updates the following offset to a random value
	 */
	void UpdateFollowPositionOffset();

	/**
	 * Updates the following velocity
	 */
	void UpdateFollowVelocity(float deltaTime);

	/**
	* Rotates the offset vector based on the player's rotation, so the offset is oriented to the player the same
	* in any direction
	*/
	FVector GetRotatedVector(const FVector inVector, const float inAngle);
	
public:
	// Sets default values for this pawn's properties
	ACompanionBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**
	* Notifies observers that the player has began aiming
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnPlayerBeginAiming();
	void OnPlayerBeginAiming_Implementation();

	/**
	* Notifies observers that the player is no longer aiming
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnPlayerEndAiming();
	void OnPlayerEndAiming_Implementation();	
};
