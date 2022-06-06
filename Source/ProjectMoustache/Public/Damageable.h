// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Damageable.generated.h"

UENUM(BlueprintType, Blueprintable)
enum EStatusEffectType
{
	// No status effect
	None UMETA(DisplayName = "None"),
	// Damage over time
	Burn UMETA(DisplayName = "Burn"),
	// Slows movement speed
	Chilled UMETA(DisplayName = "Chilled"),
	// Freezes movement
	Frozen UMETA(DisplayName = "Frozen"),
	// Freezes from ice, increased damage from lightning, fire removes wet
	Wet UMETA(DisplayName = "Wet"),
	// Random stun
	Static UMETA(DisplayName = "Static"),
};

USTRUCT(BlueprintType)
struct FStatusEffect
{
	GENERATED_BODY()

	// Enum for status effect
	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<EStatusEffectType> statusEffectType;

	// Duration the status lasts
	UPROPERTY(BlueprintReadWrite)
	float duration;

	/**
	 * The chance the status will be inflicted on the character.
	 * Value is clamped between 0 - 100
	 * 100 % always inflicts unless the character has immunity
	 * 0% never inflicts, but will deal bonus damage if character has weakness
	 */
	UPROPERTY(BlueprintReadWrite)
	float statusInflictChance;

	/**
	 * Amount of effect the status has
	 * Amount of damage DOT causes
	 * Percentage of damage increase for damage multipliers (Static + Wet Status)
	 */
	UPROPERTY(BlueprintReadWrite)
	float effectAmount;

	/**
	 * Interval of Damage Over Time
	 */
	UPROPERTY(BlueprintReadWrite)
	float dotInterval;

	FStatusEffect()
	{
		duration = 0;
		statusInflictChance = 0;
		effectAmount = 0;
		dotInterval = 0;
		statusEffectType = None;
	}

	FStatusEffect(EStatusEffectType effectType, float effectDuration = 0, float inflictChance = 0)
	{
		statusEffectType = effectType;
		duration = effectDuration;
		statusInflictChance = inflictChance;
		effectAmount = 0;
		dotInterval = 0;
	}

	FStatusEffect(EStatusEffectType effectType, float amount, float interval, float effectDuration = 0, float inflictChance = 0)
	{
		statusEffectType = effectType;
		duration = effectDuration;
		statusInflictChance = inflictChance;
		dotInterval = interval;
		effectAmount = amount;
	}
};

USTRUCT(BlueprintType)
struct FDamageData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FStatusEffect statusEffect;

	FDamageData()
	{
		statusEffect = FStatusEffect();
	}

	FDamageData(FStatusEffect elementalEffect)
	{
		statusEffect = elementalEffect;
	}
};

// This class does not need to be modified.
UINTERFACE()
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class PROJECTMOUSTACHE_API IDamageable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float TakeIncomingDamage(float damageAmount, AActor* damageCauser, AController* eventInstigator, FDamageData damageData);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Die();
	
	UFUNCTION()
	virtual void SetMoveSpeed(float speed) = 0; 
	
	/**
	* Takes in a status effect struct and adds it to the character
	* If the resistance to the element is <= 0, the effect will not be added
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void AddStatusEffect(FStatusEffect statusEffect);

	/**
	* Removes all status effects of a type
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RemoveStatus(EStatusEffectType statusEffect);

	/**
	* Called by a status effect to remove themselves
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RemoveStatusEffect(class UStatusEffectBase* statusEffect);

	/**
	 * Returns a list of status effects active on the character
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<UStatusEffectBase*> GetAllStatusEffects() const;

	/**
	 * Returns character's resistance to fire
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetFireResistance() const;

	/**
	 * Returns character's resistance to ice
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetIceResistance() const;

	/**
	* Returns character's resistance to water
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetWaterResistance() const;

	/**
	* Returns character's resistance to lightning
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetLightningResistance() const;

	/**
	 * Returns true if the character has the status effect
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetHasStatusEffect(EStatusEffectType statusEffect);

	/**
	* Returns the first found non-expired object for the inputted status effect type
	* If the status effect does not exist, it will return nullptr
	*/
	class UStatusEffectBase* GetStatusEffect(EStatusEffectType effectType);

	/**
	 * Returns the current amount of health
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetCurrentHealth() const;

	/**
	 * Returns the max health the character can have
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetMaxHealth() const;

	UFUNCTION()
	virtual float GetMoveSpeed() const = 0;

	/**
	 * Returns true if the character is dead
	 * i.e. the characters health is <= 0
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetIsDead() const;
};
