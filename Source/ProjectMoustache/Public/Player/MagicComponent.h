// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MagicSpellBase.h"
#include "Components/ActorComponent.h"
#include "MagicComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTMOUSTACHE_API UMagicComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	UMagicSpellBase* destructionSpell;

	UPROPERTY()
	UMagicSpellBase* supportSpell;

	UPROPERTY()
	UMagicSpellBase* currentSpell;

	bool isCasting;
	
public:	
	// Sets default values for this component's properties
	UMagicComponent();

protected:

	// Default destruction to be equipped when game starts
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMagicSpellBase> startingDestructionSpell;

	// Default support to be equipped when game starts
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMagicSpellBase> startingSupportSpell;
	
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	
	void CastDestructionSpell();
	void CastSupportSpell();
	
	/**
	* Creates and Equips a new destruction spell
	*/
	UFUNCTION(BlueprintCallable)
	void SetDestructionMagicSpell(TSubclassOf<UMagicSpellBase> newMagicSpell);

	/**
	* Creates and Equips a new support spell
	*/
	UFUNCTION(BlueprintCallable)
	void SetSupportMagicSpell(TSubclassOf<UMagicSpellBase> newSupportSpell);

	UFUNCTION(BlueprintCallable)
	void CastMagicProjectile();

	void CancelCasting();

	UFUNCTION(BlueprintCallable)
	UMagicSpellBase* GetCurrentDestructionSpell();

	UFUNCTION(BlueprintCallable)
	UMagicSpellBase* GetCurrentSupportSpell();

	FORCEINLINE bool GetIsCasting() const { return isCasting; }
};
