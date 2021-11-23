// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "Components/ActorComponent.h"
#include "InventoryComponentBase.generated.h"

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

	// The item reference for the inventory slot
	UPROPERTY(BlueprintReadWrite)
	AItemBase* item;

	// The quantity of the item in the slot
	UPROPERTY(BlueprintReadWrite)
	int quantity;
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTMOUSTACHE_API UInventoryComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponentBase();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Array of items in the inventory
	UPROPERTY(BlueprintReadWrite)
	TArray<FInventorySlot> inventory;

	// Number of inventory slots available
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int numberOfSlots;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ToggleInventory();
	void ToggleInventory_Implementation() { }

	UFUNCTION(BlueprintCallable)
	int GetInventorySize() { return inventory.Num(); }

	UFUNCTION(BlueprintCallable)
	TArray<FInventorySlot> GetInventory() { return inventory; }
};
