// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "ImpBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTMOUSTACHE_API AImpBase : public AEnemyBase
{
	GENERATED_BODY()

	/**
	 * Using default collider to follow nav mesh
	 * Add additional collider for collision on the character mesh
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UCapsuleComponent* bodyCollider;

	bool lockedOnTarget;

	// Floating movement
	FVector baseLocation;
	FVector interpFrom;
	FVector floatOffset;
	float verticalFloatCounter;
	float timeNextUpdateOffset;

	FVector glitchLocation;
	FVector beforeGlitchLocation;
	float glitchAlpha;
	float timeNextCanGlitch;
	bool isGlitching;	

	void UpdateGlitch(float deltaTime);
	
	void FloatingMovement(float deltaTime);

protected:

	UPROPERTY(BlueprintReadWrite)
	AActor* target;

	UPROPERTY(EditDefaultsOnly)
	FVector floatingOffsetAmount;
	
	UPROPERTY(EditDefaultsOnly)
	float updateOffsetInterval;

	UPROPERTY(EditDefaultsOnly)
	float floatingSpeed;

	UPROPERTY(EditDefaultsOnly)
	float glitchSpeed;
	
	UPROPERTY(EditDefaultsOnly)
	float glitchCooldown;
	
	virtual void BeginPlay() override;

public:

	AImpBase();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	bool GlitchToLocation(FVector location);

	UFUNCTION(BlueprintCallable)
	void LockOnTarget(AActor* lockTarget);

	UFUNCTION(BlueprintCallable)
	void UnlockTarget();
	
	/*virtual void SetMoveSpeed(bool running) override;*/
};
