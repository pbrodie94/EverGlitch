// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MagicSpellBase.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTMOUSTACHE_API AMagicSpellBase : public AActor
{
	GENERATED_BODY()

	// The next time the spell can be fired
	UPROPERTY()
	float timeNextFire;

protected:

	virtual void BeginPlay() override;

	// Name of the spell
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MagicSpell)
	FText spellName;

	// Description of the spell
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MagicSpell)
	FText spellDescription;

	// UI Icon for the spell
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MagicSpell)
	UTexture2D* spellIcon;

	// Amount of damage done
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MagicSpell)
	float damage;

	// Mana cost of the spell
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MagicSpell)
	float manaCost;

	// Time it takes between casting
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = MagicSpell)
	float coolDownTime;

	UPROPERTY()
	float timeCasted;

	// Implementation of the casting of the spell
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CastSpell(APawn* userActor);
	void CastSpell_Implementation(APawn* userActor) { }

public:

	AMagicSpellBase();

	/**
	 * Universal call function to cast the spell
	 * Checks if the spell can be casted before casting
	 */
	UFUNCTION(BlueprintCallable)
	void Execute(APawn* userActor);

	UFUNCTION(BlueprintCallable)
	FText GetSpellName() { return spellName; }
	
	UFUNCTION(BlueprintCallable)
	FText GetSpellDescription() { return spellDescription; }

	UFUNCTION(BlueprintCallable)
	UTexture2D* GetSpellIcon() { return spellIcon; }

	UFUNCTION(BlueprintCallable)
	float GetCoolDownTime() { return coolDownTime; }

	UFUNCTION(BlueprintCallable)
	float GetCoolDownRemaining();
};
