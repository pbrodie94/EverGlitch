// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Player/Grabber.h"
#include "PlayerBase.generated.h"

UCLASS()
class PROJECTMOUSTACHE_API APlayerBase : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = true))
	class USpringArmComponent* cameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = true))
	class UCameraComponent* followCamera;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<UMatineeCameraShake> cameraShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	float aimSensitivityX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	float aimSensitivityY;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float runSpeed;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	float jumpHeight;

	UPROPERTY(EditDefaultsOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	float airControl;

	UPROPERTY(EditDefaultsOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	float dashPower;

	UPROPERTY(EditDefaultsOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	float dashDelayInterval;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Stats, meta = (AllowPrivateAccess = true))
	float health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	float maxHealth;

	bool isMeleeAttacking;

	float timeLastDashed;

	void Dash();

	void DetectMeleeHits();

public:
	// Sets default values for this character's properties
	APlayerBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	//Used to keep track of actors hit during hit detection
	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> hitActors;

	UPROPERTY(BlueprintReadWrite)
	float percentageDamageChange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	float projectileDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	float meleeDamage;

	UPROPERTY(BlueprintReadWrite)
	bool isDead;

	//Handles movement
	void MoveForward(float value);
	void MoveRight(float value);

	UFUNCTION(BlueprintCallable)
	void BeginMeleeAttackDamage();

	UFUNCTION(BlueprintCallable)
	void EndMeleeAttackDamage();

	UFUNCTION(BlueprintImplementableEvent)
	void Die();

	UFUNCTION(BlueprintCallable)
	bool GetIsMeleeAttacking() { return isMeleeAttacking; }

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	float GetCurrentHealth() { return health; }

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() { return maxHealth; }
};
