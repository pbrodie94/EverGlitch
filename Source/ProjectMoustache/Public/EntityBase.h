// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Damageable.h"
#include "GameFramework/Character.h"
#include "EntityBase.generated.h"

UCLASS()
class PROJECTMOUSTACHE_API AEntityBase : public ACharacter, public IDamageable
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<class UStatusEffectBase*> newStatusEffects;
	UPROPERTY()
	TArray<class UStatusEffectBase*> statusEffects;
	UPROPERTY()
	TArray<class UStatusEffectBase*> removedStatusEffects;

public:
	// Sets default values for this character's properties
	AEntityBase();

protected:

	// Health the enemy starts with, defaults to max health
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float startingHealth;

	// Maximum possible health, if less than 1: defaults to 100
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float maxHealth;

	// Current health
	UPROPERTY(BlueprintReadWrite)
	float currentHealth;

	// Element effectiveness
	/**
	* Effectiveness of fire damage on the Entity
	* Effectiveness is clamped between -100% - 200%
	* 0% negates all damage
	* 200% deals double damage
	* -100% adds all damage back to health
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat)
	float fireEffectiveness;

	/**
	* Effectiveness of ice damage on the Entity
	* Effectiveness is clamped between -100% - 200%
	* 0% negates all damage
	* 200% deals double damage
	* -100% adds all damage back to health
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat)
	float iceEffectiveness;

	/**
	* Effectiveness of lightning damage on the Entity
	* Effectiveness is clamped between -100% - 200%
	* 0% negates all damage
	* 200% deals double damage
	* -100% adds all damage back to health
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat)
	float lightningEffectiveness;

	/**
	* Effectiveness of water damage on the Entity
	* Effectiveness is clamped between -100% - 200%
	* 0% negates all damage
	* 200% deals double damage
	* -100% adds all damage back to health
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat)
	float waterEffectiveness;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Die();
	virtual void Die_Implementation() { }

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*********************************************************************
	 * IDamageable
	 ********************************************************************/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float TakeIncomingDamage(float damageAmount, AActor* damageCauser, AController* eventInstigator, FDamageData damageData);
	virtual float TakeIncomingDamage_Implementation(float damageAmount, AActor* damageCauser, AController* eventInstigator, FDamageData damageData) override;

	/**
	* Takes in a status effect struct and adds it to the character
	* If the resistance to the element is <= 0, the effect will not be added
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void AddStatusEffect(FStatusEffect statusEffect);
	virtual void AddStatusEffect_Implementation(FStatusEffect) override;

	/**
	* Removes all status effects of a type
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RemoveStatus(EStatusEffectType statusEffect);
	virtual void RemoveStatus_Implementation(EStatusEffectType StatusEffect) override;

	/**
	 * Called by a status effect to remove themselves
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RemoveStatusEffect(class UStatusEffectBase* statusEffect);
	virtual void RemoveStatusEffect_Implementation(class UStatusEffectBase* statusEffect) override;

	/**
	* Returns a list of status effects active on the character
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<UStatusEffectBase*> GetAllStatusEffects();
	TArray<UStatusEffectBase*> GetAllStatusEffects_Implementation();
	
	/**
	 * Returns the first found non-expired object for the inputted status effect type
	 * If the status effect does not exist, it will return nullptr
	 */
	class UStatusEffectBase* GetStatusEffect(EStatusEffectType effectType);
	
	/**
	* Returns character's resistance to fire
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetFireResistance() const;
	virtual FORCEINLINE float GetFireResistance_Implementation() const override { return fireEffectiveness; }

	/**
	* Returns character's resistance to ice
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetIceResistance() const;
	virtual FORCEINLINE float GetIceResistance_Implementation() const override { return iceEffectiveness; }	

	/**
	* Returns character's resistance to water
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetWaterResistance() const;
	virtual FORCEINLINE float GetWaterResistance_Implementation() const override { return waterEffectiveness; }

	/**
	* Returns character's resistance to lightning
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetLightningResistance() const;
	virtual FORCEINLINE float GetLightningResistance_Implementation() const override { return 0; }
	
	/**
	* Returns true if the character has the status effect
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetHasStatusEffect(EStatusEffectType statusEffect);
	virtual bool GetHasStatusEffect_Implementation(EStatusEffectType statusEffect) override;

	/**
	* Returns the current amount of health
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetCurrentHealth() const;
	float FORCEINLINE GetCurrentHealth_Implementation() const override { return currentHealth; }

	/**
	* Returns the max health the character can have
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	float GetMaxHealth() const;
	float FORCEINLINE GetMaxHealth_Implementation() const override { return maxHealth; }

	/**
	* Returns true if the character is dead
	* i.e. the characters health is <= 0
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetIsDead() const;
	virtual FORCEINLINE bool GetIsDead_Implementation() const override { return currentHealth <= 0; }
};
