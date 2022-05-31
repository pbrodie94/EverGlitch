// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "PlayerObserver.h"
#include "EntityBase.h"
#include "Weapons/WeaponBase.h"
#include "PlayerBase.generated.h"

UCLASS()
class PROJECTMOUSTACHE_API APlayerBase : public AEntityBase, public IPlayerCharacter
{
	GENERATED_BODY()

	// COMPONENTS ***********************************************************************
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
	class UMagicComponent* magicComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UCombatManagerComponent* combatManager;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UInventoryComponentBase* inventoryComponent;

	//************************************************************************************

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	float aimSensitivityX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	float aimSensitivityY;

	// ********************************************************************************
	// MOVEMENT ***********************************************************************
	
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
	UPROPERTY(EditDefaultsOnly, Category = Abilities, meta = (AllowPrivateAccess = true))
	float airControl;

	// Controls when player has control of the character
	bool hasControl;

	// *********************************************************************************
	// ENERGY VARIABLES ****************************************************************
	
	// Amount of energy player has to perform abilities like dash
	float abilityEnergy;

	// The max amount of ability energy
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Abilities, meta = (AllowPrivateAccess = true))
	float maxAbilityEnergy;

	// Delay while not in use before energy begins recharging
	UPROPERTY(EditDefaultsOnly, Category = Abilities, meta = (AllowPrivateAccess = true))
	float energyRechargeDelay;

	// The time in world time ability energy will begin to recharge
	float timeBeginRecharge;

	// The rate at which energy is recharged
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Abilities, meta = (AllowPrivateAccess = true))
	float energyRechargeRate;

	//*********************************************************************************
	// DASH VARIABLES *****************************************************************
	
	//Amount of force/distance in a dash
	UPROPERTY(EditDefaultsOnly, Category = Abilities, meta = (AllowPrivateAccess = true))
	float dashPower;

	// Energy cost to dash
	UPROPERTY(EditDefaultsOnly, Category = Abilities, meta = (AllowPrivateAccess = true))
	float dashEnergyCost;

	//Time in between dashes
	UPROPERTY(EditDefaultsOnly, Category = Abilities, meta = (AllowPrivateAccess = true))
	float dashDelayInterval;

	// Number of times player can dash in the air. Values less than 0 allow infinite air dashes
	UPROPERTY(EditDefaultsOnly, Category = Abilities, meta = (AllowPrivateAccess = true))
	int numAirDashes;

	// Counter for times dashed in the air. Resets when player is on the ground
	int timesDashedInAir;

	//**********************************************************************************
	// COMBAT VARIABLES ****************************************************************
	
	//Duration player is facing direction of camera after performing ranged attack
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	float combatStanceTime;

	// Enabled as player is performing a melee attack
	bool isMeleeAttacking;

	//Time able to fire again, time plus delay set every shot
	float timeNextShot;

	//*********************************************************************************
	// ENERGY ABILITY VARIABLES *******************************************************
	
	//Next time able to dash
	float timeNextDash;

	// TELEKINESIS VARIABLES *******************************************************

	bool isUsingTK;
	
	// The object currently being manipulated by TK
	UPROPERTY()
	AActor* tkObject; 
	
	// The target point in space the object will be brought to
	FVector targetPoint;

	float targetDistance;

	bool isTKPushing;
	bool isTKPulling;
	
	float timeNextDetectTK;

	// The speed the player can push or pull a TK object in the air
	UPROPERTY(EditDefaultsOnly, Category = Abilities, meta = (AllowPrivateAccess = true))
	float pushPullSpeed;

	// The ability energy cost to use telekinesis
	UPROPERTY(EditDefaultsOnly, Category = Abilities, meta = (AllowPrivateAccess = true))
	float tkEnergyCost;
	
	// The accuracy of how close the object gets to the target point
	UPROPERTY(EditDefaultsOnly, Category = Abilities, meta = (AllowPrivateAccess = true))
	float tkAccuracy;

	// The detection angle in degrees for highlighting objects to interact with TK
	float tkViewAngle;

	// The minimum distance the object can be brought in
	float minTKRange;
	
	// Max range of the telekinesis ability
	UPROPERTY(EditDefaultsOnly, Category = Abilities, meta = (AllowPrivateAccess = true))
	float maxTKRange;

	// The amount of force the TK push has
	UPROPERTY(EditDefaultsOnly, Category = Abilities, meta = (AllowPrivateAccess = true))
	float tkPushForce;

	// List of objects that are not tk objects
	UPROPERTY()
	TArray<AActor*> nonTKObjects;

	//****************************************************************************
	// Timer handles *************************************************************
	
	FTimerHandle rangedCombatTimerHandle;

	FTimerHandle damageTimerHandle;
	FTimerHandle speedTimerHandle;
	FTimerHandle jumpTimerHandle;

	// *****************************************************************************
	// Handles movement ************************************************************
	void MoveForward(float value);
	void MoveRight(float value);

	// Handles looking *************************************************************
	void LookX(float value);
	void LookY(float value);

	// Handles jumping *************************************************************
	void BeginJump();
	void EndJump();

	// *****************************************************************************
	// WEAPON FUNCTIONS ************************************************************
	
	// Temporarily Updating position
	void UpdateWeaponPosition() const;
	
	/**
	 * Fire projectiles on main fire button.
	 * Will be changed to call a fire function on a weapon when created
	 */
	void Fire();

	/**
	 * Called when the fire button is released
	 */
	void FireUp();

	// ****************************************************************************

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

	/**
	 * Used to cast magic support spell
	 */
	void UseAbility1();

	/**
	* Used to cast magic destruction spell
	*/
	void UseAbility2();

	/**
	 * If player has a current interactable object reference, interact with it
	 */
	void InteractWithObject();

	void BeginCombatStance();
	void BeginEndCombatStanceTimer();

	/**
	 * Expiry function for the combat stance timer
	 * Resets player to turn in direction of movement a set time after doing ranged attacks
	 */
	void EndCombatStance() const;

	/**
	 * Expiry functions for stat change timers
	 * Resets the values to default once expired
	 */
	void OnDamageChangeExpired();
	void OnSpeedChangeExpired();
	void OnJumpChangeExpired();

	// Telekinesis functions ***********************************************************

	/**
	 * Used to detect nearby objects that can be picked up with the TK ability
	 * Gets a list of objects then highlights one that is within the tkViewAngle.
	 * If more than one object is within the tkViewAngle the one that is closest
	 * to the center will be chosen.
	 */
	void DetectTKObjects();

	void RemoveTKObject();

	void PickupTKObject();

	void PushTKObject();

	void DropTKObject();
	
	void ManipulateTKObject(float deltaTime);

	void StopPushingTKObject();

	void StopPullingTKObject();
	
	//**********************************************************************************

	void ScreenDebugMessage(FString message, FColor displayColor, float displayTime);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AWeaponBase> startingWeapon;

	UPROPERTY(BlueprintReadWrite)
	AWeaponBase* currentWeapon;

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
	bool godMode;

	//Interactable reference container, gets set and allows player to call the interact function
	UPROPERTY(BlueprintReadWrite)
	TScriptInterface<IInteractable> currentInteractableObject;

	//List of objects observing the player
	UPROPERTY(BlueprintReadOnly)
	TArray<TScriptInterface<IPlayerObserver>> observers;

	//Begins melee hit detection
	UFUNCTION(BlueprintCallable)
	void BeginMeleeAttackDamage();

	//Ends melee hit detection
	UFUNCTION(BlueprintCallable)
	void EndMeleeAttackDamage();

	UFUNCTION(BlueprintNativeEvent) // Expects that the function is defined in Blueprint
	void HandleDashEffects(); // Put whatever parameters you need
	void HandleDashEffects_Implentaion() { }

	//Function for spawning projectiles in blueprint
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnProjectile(FTransform spawnTransform);
	
	virtual void Die_Implementation() override;

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
	void ToggleInventory_Implementation() {}

public:

	// Sets default values for this character's properties
	APlayerBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Takes in damage, and returns the actual damage
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual float TakeIncomingDamage_Implementation(float damageAmount, AActor* damageCauser, AController* eventInstigator, FDamageData damageData) override;

	/**
	 * Returns pointer reference to combat manager
	 */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UCombatManagerComponent* GetCombatManager() { return combatManager; }

	/**
	 * Returns pointer to inventory component
	 */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UInventoryComponentBase* GetInventoryComponent() { return inventoryComponent; }

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
	* Returns player's ability energy level
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetAbilityEnergyLevel() const;
	float GetAbilityEnergyLevel_Implementation() const { return abilityEnergy; }

	/**
	* Returns player's max ability energy level
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetMaxAbilityEnergyLevel() const;
	float GetMaxAbilityEnergyLevel_Implementation() const { return maxAbilityEnergy; }

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
	bool GetHasInteractable() const;
	bool GetHasInteractable_Implementation() const;

	/**
	* Returns the unique interactable message for each interactable
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FText GetInteractableMessage();
	FText GetInteractableMessage_Implementation();

	/**
	* Returns the camera's current location
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FVector GetCameraLocation() const;
	FVector GetCameraLocation_Implementation() const;

	/**
	 * Returns the forward vector of the player's camera
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FVector GetCameraForwardVector() const;
	FVector GetCameraForwardVector_Implementation() const;

	/**
	* Returns player's current location
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FVector GetPlayerLocation() const;
	FVector GetPlayerLocation_Implementation() const;

	/**
	* Returns player's current forward direction
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FVector GetPlayerForwardDirection() const;
	FVector GetPlayerForwardDirection_Implementation() const;

	/**
	* Returns player's current rotation
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FRotator GetPlayerRotation() const;
	FRotator GetPlayerRotation_Implementation() const;

	/**
	* Returns player's current velocity
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetCurrentPlayerVelocity() const;
	float GetCurrentPlayerVelocity_Implementation() const;

	/**
	* Returns the currently equipped weapon
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	AWeaponBase* GetWeapon() const;
	FORCEINLINE AWeaponBase* GetWeapon_Implementation() const { return currentWeapon; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetHasWeapon() const;
	FORCEINLINE bool GetHasWeapon_Implementation() const { return currentWeapon != nullptr; }
	
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

	/**
	* Sets whether or not the player character can be controlled
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetHasControl(bool control);
	FORCEINLINE void SetHasControl_Implementation(bool control) { hasControl = control; }

	/**
	* Returns whether or not the player character is accepting player input
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetHasControl() const;
	FORCEINLINE bool GetHasControl_Implementation() const { return hasControl; }
};
