// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MagicComponent.h"
#include "MagicSpellBase.h"
#include "PlayerCharacter.h"
#include "PlayerObserver.h"
#include "GameFramework/Character.h"
#include "Interactables/InventoryComponentBase.h"
#include "PlayerBase.generated.h"

UCLASS()
class PROJECTMOUSTACHE_API APlayerBase : public ACharacter, public IPlayerCharacter
{
	GENERATED_BODY()

	//Camera boom arm
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = true))
	class USpringArmComponent* cameraBoom;

	//Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = true))
	class UCameraComponent* followCamera;

	//Reference to the camera shake object
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<UMatineeCameraShake> cameraShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UMagicComponent* magicComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	float aimSensitivityX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	float aimSensitivityY;

	//Default move speed
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float runSpeed;

	// Speed while aiming
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float aimSpeed;

	//Default jump height
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	float jumpHeight;

	//Default air control
	UPROPERTY(EditDefaultsOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	float airControl;

	//Amount of force/distance in a dash
	UPROPERTY(EditDefaultsOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	float dashPower;

	//Time in between dashes
	UPROPERTY(EditDefaultsOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	float dashDelayInterval;

	//Current health
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Stats, meta = (AllowPrivateAccess = true))
	float health;

	//Max health
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	float maxHealth;

	//Duration player is facing direction of camera after performing ranged attack
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	float combatStanceTime;

	bool isMeleeAttacking;

	//Time able to fire again, time plus delay set every shot
	float timeNextShot;

	//Next time able to dash
	float timeNextDash;

	//Timer handles
	FTimerHandle rangedCombatTimerHandle;

	FTimerHandle damageTimerHandle;
	FTimerHandle speedTimerHandle;
	FTimerHandle jumpTimerHandle;

	/**
	 * Fire projectiles on main fire button.
	 * Will be changed to call a fire function on a weapon when created
	 */
	void Fire();

	/**
	 * Takes the movement direction of the player, excludes the vertical direction
	 * then applies a dash force, as well as a slight upwards force to keep from getting stuck on floor
	 */
	void Dash();

	/**
	 * Hit detection for melee hits
	 * Will be moved to weapon class when created
	 */
	void DetectMeleeHits();

	void CastMagicSpell();

	void CastSupportSpell();

	/**
	 * If player has a current interactable object reference, interact with it
	 */
	void InteractWithObject();

	/**
	 * Expiry function for the combat stance timer
	 * Resets player to turn in direction of movement a set time after doing ranged attacks
	 */
	void OnCombatStanceEnd();

	/**
	 * Expiry functions for stat change timers
	 * Resets the values to default once expired
	 */
	void OnDamageChangeExpired();
	void OnSpeedChangeExpired();
	void OnJumChangeExpired();

public:
	// Sets default values for this character's properties
	APlayerBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInventoryComponentBase* inventoryComponent;*/

	//Used to keep track of actors hit during hit detection
	UPROPERTY(BlueprintReadWrite)
	TArray<AActor*> hitActors;

	//Multiplier for damage values used for applying damage changes
	UPROPERTY(BlueprintReadWrite)
	float percentageDamageChange;

	UPROPERTY(BlueprintReadWrite)
	bool isAiming;

	//Time between shots, will be moved to weapon class when created
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	float fireDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	float projectileDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
	float meleeDamage;

	UPROPERTY(BlueprintReadWrite)
	bool isDead;

	UPROPERTY(BlueprintReadWrite)
	bool godMode;

	//Interactable reference container, gets set and allows player to call the interact function
	UPROPERTY(BlueprintReadWrite)
	TScriptInterface<IInteractable> currentInteractableObject;

	//List of objects observing the player
	UPROPERTY(BlueprintReadOnly)
	TArray<TScriptInterface<IPlayerObserver>> observers;

	//Handles movement
	void MoveForward(float value);
	void MoveRight(float value);

	//Begins melee hit detection
	UFUNCTION(BlueprintCallable)
	void BeginMeleeAttackDamage();

	//Ends melee hit detection
	UFUNCTION(BlueprintCallable)
	void EndMeleeAttackDamage();

	//Function for spawning projectiles in blueprint
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnProjectile(FTransform spawnTransform);

	//Partially implemented in cpp, remainder in blueprint
	UFUNCTION(BlueprintNativeEvent)
	void Die();
	void Die_Implementation();

	UFUNCTION(BlueprintCallable)
	bool GetIsMeleeAttacking() { return isMeleeAttacking; }

	UFUNCTION(BlueprintNativeEvent)
	void BeginAiming();
	void BeginAiming_Implementation();

	UFUNCTION(BlueprintNativeEvent)
	void EndAiming();
	void EndAiming_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ToggleInventory();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Takes in damage, and returns the actual damage
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float GetCurrentHealth();
	float GetCurrentHealth_Implementation() { return health; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float GetMaxHealth();
	float GetMaxHealth_Implementation() { return maxHealth; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool GetIsPlayerDead();
	bool GetIsPlayerDead_Implementation() { return isDead; }

	//Player Character interface functions
	/**
	* Used to add health to player, and returns false if health is full
	* The absolute value will be used, so any negative values will become positive
	* Use Apply Damage for health removal
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool ApplyHealth(float amount);
	bool ApplyHealth_Implementation(float amount);

	/**
	* Applies a damage change to all of player's attacks
	* Takes in the percentage of change from -99% to 200%
	* Duration is the time in seconds the effect will last for, durations of 0 or less will be indefinite and must be
	* undone by calling function again taking in (0, 0) as parameters.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ApplyDamageChange(float percentage, float duration);
	void ApplyDamageChange_Implementation(float percentage, float duration);

	/**
	* Applies a speed change to player's movement speed
	* Takes in the percentage of change from -99% to 100%
	* Duration is the time in seconds the effect will last for, durations of 0 or less will be indefinite and must be
	* undone by calling function again taking in (0, 0) as parameters.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ApplySpeedChange(float percentage, float duration);
	void ApplySpeedChange_Implementation(float percentage, float duration);

	/**
	* Applies a change to the player's jump height
	* Takes in the percentage of change from -100% to 100%
	* Duration is the time in seconds the effect will last for, durations of 0 or less will be indefinite and must be
	* undone by calling function again taking in (0, 0) as parameters.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ApplyJumpChange(float percentage, float duration);
	void ApplyJumpChange_Implementation(float percentage, float duration);

	/**
	* Takes in an interactable object, and sets it as the current interactable object
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetInteractable(const TScriptInterface<IInteractable>& interactableObject);
	virtual void SetInteractable_Implementation(const TScriptInterface<IInteractable>& interactableObject);

	/**
	* Takes in an interactable reference, and removes it from the player's interactable pointer
	* if the player currently has a pointer to the calling interactable object it will not be removed
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RemoveSelfAsInteractable(const TScriptInterface<IInteractable>& interactableReference);
	void RemoveSelfAsInteractable_Implementation(const TScriptInterface<IInteractable>& interactableReference);

	/**
	* Returns whether or not the player currently has a reference to an interactable object
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetHasInteractable();
	bool GetHasInteractable_Implementation();

	/**
	* Returns the camera's current location
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FVector GetCameraLocation();
	FVector GetCameraLocation_Implementation();

	/**
	* Returns player's current location
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FVector GetPlayerLocation();
	FVector GetPlayerLocation_Implementation();

	/**
	* Returns player's current forward direction
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FVector GetPlayerForwardDirection();
	FVector GetPlayerForwardDirection_Implementation();

	/**
	* Returns player's current rotation
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FRotator GetPlayerRotation();
	FRotator GetPlayerRotation_Implementation();

	/**
	* Returns player's current velocity
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetCurrentPlayerVelocity();
	float GetCurrentPlayerVelocity_Implementation();

	/**
	* Subscribes actors as a new player observer
	* Must implement the Player Observer Interface
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SubscribeAsObserver(const TScriptInterface<IPlayerObserver>& newObserver);
	void SubscribeAsObserver_Implementation(const TScriptInterface<IPlayerObserver>& newObserver);

	/**
	* Unsubscribes an actor as a player observer
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UnSubscribePlayerObserver(const TScriptInterface<IPlayerObserver>& oldObserver);
	void UnSubscribePlayerObserver_Implementation(const TScriptInterface<IPlayerObserver>& oldObserver);
};
