// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "Components/ActorComponent.h"
#include "InventoryComponentBase.generated.h"

USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

	// Item's name
	UPROPERTY(BlueprintReadWrite)
	FText itemName;
	
	// Item's description
	UPROPERTY(BlueprintReadWrite)
	FText itemDescription;
	
	// Item's icon
	UPROPERTY(BlueprintReadWrite)
	UTexture2D* itemIcon;
	
	// The item reference for the inventory slot
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<class AItemBase> itemClass;

	// Is the item stackable?
	UPROPERTY(BlueprintReadWrite)
	bool isStackable;

	// How much can the item be stacked?
	UPROPERTY(BlueprintReadWrite)
	int maxStackSize;
};

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

	FInventorySlot()
	{
		item = FInventoryItem();
		quantity = 0;
	}

	// Item struct
	UPROPERTY(BlueprintReadWrite)
	FInventoryItem item;
	
	// The quantity of the item in the slot
	UPROPERTY(BlueprintReadWrite)
	int quantity;
};

USTRUCT(BlueprintType)
struct FQuickSlot
{
	GENERATED_BODY()

	FQuickSlot()
	{
		itemIndex = -1;
		item = FInventorySlot();
	}

	UPROPERTY(BlueprintReadWrite)
	int itemIndex;

	UPROPERTY(BlueprintReadWrite)
	FInventorySlot item;
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTMOUSTACHE_API UInventoryComponentBase : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	UInputComponent* InputComponent;

	FQuickSlot quickSlot1;
	FQuickSlot quickSlot2;
	FQuickSlot quickSlot3;
	
	/**
	 * Creates a new stack of items
	 * Does not add the item if inventory is full
	 * Adds multiple items if the item is stackable and has more than 1
	 * Otherwise, just adds one of the item
	 */
	bool CreateStack(FInventorySlot itemToAdd);
	
	/**
	 * Adds the picked up item to an existing stack 
	 */
	bool AddToStack(FInventorySlot itemToAdd, int index);
	
	/**
	 * Gets the index of a partial stack of the queried item
	 * returns -1 if no partial stacks are found
	 */
	int GetPartialStackIndex(FInventorySlot queryItem);
	
	/**
	 * Checks if the query item already has a partial stack in the inventory
	 */
	bool GetHasPartialStack(FInventorySlot queryItem);

	// Returns -1 if inventory is full
	int GetNextEmptySlot();

	void UseQuickSlot(FQuickSlot quickSlot);

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

	UFUNCTION(BlueprintCallable)
	void RemoveItem(int index);

	void UseQuickSlot1();
	void UseQuickSlot2();
	void UseQuickSlot3();
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool UseItem(int index);
	bool UseItem_Implementation(int index) { return false; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ToggleInventory();
	void ToggleInventory_Implementation() { }

	UFUNCTION(BlueprintCallable)
	bool AddToInventory(FInventorySlot itemToAdd);

	/**
	 * Checks if the inventory has an item
	 */
	UFUNCTION(BlueprintCallable)
	bool GetHasItem(TSubclassOf<class AItemBase> queryItem);

	/**
	 * Checks if the inventory has a specific item and quantity of item
	 */
	UFUNCTION(BlueprintCallable)
	bool GetHasItemWithQuantity(TSubclassOf<class AItemBase> queryItem, int queryQuantity);

	UFUNCTION(BlueprintCallable)
	int GetInventorySize() { return inventory.Num(); }

	UFUNCTION(BlueprintCallable)
	TArray<FInventorySlot> GetInventory() { return inventory; }

	/**
	 * Returns a reference to the quick slot.
	 * If an index outside of 1 - 3 is passed in, a default quick slot
	 * object with slot index of -1 will be returned
	 */
	UFUNCTION(BlueprintCallable)
	FQuickSlot GetQuickSlot(int quickSlotIndex);
};
