// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Animation/AnimMontage.h"
#include "MagicSpellBase.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PROJECTMOUSTACHE_API UMagicSpellBase : public UObject
{
	GENERATED_BODY()

	// The next time the spell can be fired
	UPROPERTY()
	float timeNextFire;

	// Used to get cooldown percentage
	UPROPERTY()
	float timeFired;

protected:

	// Name of the spell
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MagicSpell)
	FText spellName;

	// Description of the spell
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MagicSpell)
	FText spellDescription;

	// Spell icon
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MagicSpell)
	UTexture2D* spellIcon;

	// Spell projectile
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = MagicSpell)
	TSubclassOf<AActor> spellProjectile;

	// Animation
	UPROPERTY(EditAnywhere)
	UAnimMontage* castAnimation;

	// Amount of damage done
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MagicSpell)
	float damage;

	// Mana cost of the spell
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MagicSpell)
	float manaCost;

	// Time it takes between casting
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MagicSpell)
	float coolDownTime;

	UFUNCTION(BlueprintCallable)
	virtual UWorld* GetWorld() const override;

public:

	UMagicSpellBase();

	/**
	 * Universal call function to cast the spell
	 * Checks if the spell can be casted before casting
	 */
	UFUNCTION(BlueprintCallable)
	bool Execute(APawn* userActor);

	void CastSpell(APawn* userActor);

	UFUNCTION(BlueprintCallable)
	FText GetSpellName() { return spellName; }
	
	UFUNCTION(BlueprintCallable)
	FText GetSpellDescription() { return spellDescription; }

	UFUNCTION(BlueprintCallable)
	UTexture2D* GetSpellIcon() { return spellIcon; }

	UFUNCTION(BlueprintCallable)
	float GetCooldownTime() { return coolDownTime; }

	/**
	 * Returns the percentage of time left before can fire again
	 */
	UFUNCTION(BlueprintCallable)
	float GetCooldownProgress();
};
