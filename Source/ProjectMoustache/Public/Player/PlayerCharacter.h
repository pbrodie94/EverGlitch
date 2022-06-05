// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Interactable.h"
#include "PlayerObserver.h"
#include "UObject/Interface.h"
#include "PlayerCharacter.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UPlayerCharacter : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTMOUSTACHE_API IPlayerCharacter
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	/**
	 * Used to add health to player
	 * The absolute value will be used, so any negative values will become positive
	 * Use Apply Damage for health removal
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool ApplyHealth(float amount);

	/**
	 * Applies a damage change to all of player's attacks
	 * Takes in the percentage of change from -99% to 200%
	 * Duration is the time in seconds the effect will last for, durations of 0 or less will be indefinite and must be
	 * undone by calling function again taking in 0, 0.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ApplyDamageChange(float percentage, float duration);

	/**
	* Applies a speed change to player's movement speed
	* Takes in the percentage of change from -99% to 100%
	* Duration is the time in seconds the effect will last for, durations of 0 or less will be indefinite and must be
	* undone by calling function again taking in 0, 0.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ApplySpeedChange(float percentage, float duration);

	/**
	* Applies a change to the player's jump height
	* Takes in the percentage of change from -100% to 100%
	* Duration is the time in seconds the effect will last for, durations of 0 or less will be indefinite and must be
	* undone by calling function again taking in 0, 0.
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ApplyJumpChange(float percentage, float duration);
	
	virtual bool PlayAnim(class UAnimMontage* montage, FName section) = 0;

	/**
	 * Returns player's ability energy level
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetAbilityEnergyLevel() const;

	/**
	 * Returns player's max ability energy level
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetMaxAbilityEnergyLevel() const;

	/**
	 * Takes in an interactable object, and sets it as the current interactable object
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetInteractable(const TScriptInterface<IInteractable>& interactableObject);

	/**
	 * Takes in an interactable reference, and removes it from the player's interactable pointer
	 * if the player currently has a pointer to the calling interactable object
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RemoveSelfAsInteractable(const TScriptInterface<IInteractable>& interactableReference);

	/**
	 * Returns whether or not the player currently has a reference to an interactable object
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetHasInteractable() const;
	
	/**
	 * Returns the unique interactable message for each interactable
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FText GetInteractableMessage();

	/**
	 * Returns the camera's current location
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FVector GetCameraLocation() const;

	/**
	* Returns the forward vector of the player's camera
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FVector GetCameraForwardVector() const;

	/**
	 * Returns player's current location
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FVector GetPlayerLocation() const;

	/**
	 * Returns player's current forward direction
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FVector GetPlayerForwardDirection() const;

	/**
	* Returns player's current forward direction
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FRotator GetPlayerRotation() const;

	/**
	 * Returns player's current velocity
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetCurrentPlayerVelocity() const;

	/**
	 * Returns the currently equipped weapon
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	class AWeaponBase* GetWeapon() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetHasWeapon() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetIsCombatStance() const;
	
	/**
	 * Subscribes actors as a new player observer
	 * Must implement the Player Observer Interface
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SubscribeAsObserver(const TScriptInterface<IPlayerObserver>& newObserver);

	/**
	 * Unsubscribes an actor as a player observer
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UnSubscribePlayerObserver(const TScriptInterface<IPlayerObserver>& oldObserver);

	/**
	 * Sets whether or not the player character can be controlled
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetHasControl(bool control);

	/**
	 * Returns whether or not the player character is accepting player input
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetHasControl() const;
};
