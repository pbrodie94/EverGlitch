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
	AMagicSpellBase* destructionSpell;

	UPROPERTY()
	AMagicSpellBase* supportSpell;

	void CastDestructionSpell();
	void CastSupportSpell();
	
public:	
	// Sets default values for this component's properties
	UMagicComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	* Equips a new magic spell
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetPowerMagicSpell(AMagicSpellBase* newMagicSpell);

	/**
	* Equips a new support spell
	*/
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetSupportMagicSpell(AMagicSpellBase* newSupportSpell);

	UFUNCTION(BlueprintCallable)
	AMagicSpellBase* GetCurrentDestructionSpell();

	UFUNCTION(BlueprintCallable)
	AMagicSpellBase* GetCurrentSupportSpell();
	
};
