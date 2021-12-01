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

	// Item struct
	UPROPERTY(BlueprintReadWrite)
	FInventoryItem item;
	
	// The quantity of the item in the slot
	UPROPERTY(BlueprintReadWrite)
	int quantity;

	// Whether the item is selected
	UPROPERTY(BlueprintReadWrite)
	bool isSelected;
};

USTRUCT(BlueprintType)
struct FQuickSlotItem
{
	GENERATED_BODY()

	FQuickSlotItem()
	{
		inventoryItem.quantity = -1;
		inventoryIndex = -1;
	}
	
	// Item and quantity
	UPROPERTY(BlueprintReadWrite)
	FInventorySlot inventoryItem;

	// The index of the item
	UPROPERTY(BlueprintReadWrite)
	int inventoryIndex;
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTMOUSTACHE_API UInventoryComponentBase : public UActorComponent
{
	GENERATED_BODY()

	// Use quick slot items
	void UseQuickSlotOne();
	void UseQuickSlotTwo();
	void UseQuickSlotThree();
	void UpdateQuickslots(int slotIndex);

	void RemoveDuplicateQuickSlotItems(int slotNumber, int itemIndex);

public:	
	// Sets default values for this component's properties
	UInventoryComponentBase();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Array of items in the inventory
	UPROPERTY(BlueprintReadWrite)
	TArray<FInventorySlot> inventory;

	// Quick slot items
	UPROPERTY(BlueprintReadWrite)
	FQuickSlotItem quickSlot1;

	UPROPERTY(BlueprintReadWrite)
	FQuickSlotItem quickSlot2;

	UPROPERTY(BlueprintReadWrite)
	FQuickSlotItem quickSlot3;

	// Number of inventory slots available
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int numberOfSlots;

	// Use inventory item
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool UseItem(int itemIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void RefeshInventory();

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

	/**
	 * Returns an inventory item at an index
	 * if the index is out of bounds, function will return with quantity at -1
	 */
	UFUNCTION(BlueprintCallable)
	FInventorySlot GetInventoryItemAtIndex(int index);

	UFUNCTION(BlueprintCallable)
	FQuickSlotItem GetQuickSlotItem(int quickSlot);

	UFUNCTION(BlueprintCallable)
	void SetQuickSlotItem(int quickSlot, int itemIndex);

	// Deselects any other item, then selects the item passed in
	UFUNCTION(BlueprintCallable)
	void SetSelectedItem(int index);

	UFUNCTION(BlueprintCallable)
	void DeselectItem(int index);

	UFUNCTION(BlueprintCallable)
	void DeselectAll();

	// Gets the index of the selected item, returns a number less than 0 if none are selected
	UFUNCTION(BlueprintCallable)
	int GetSelectedItemIndex();
};
