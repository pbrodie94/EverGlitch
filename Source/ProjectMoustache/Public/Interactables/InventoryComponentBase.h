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

	FInventoryItem()
	{
		itemName = FText::FromString(TEXT("Item"));
		itemType = Health;
		itemIcon = nullptr;
		itemClass = nullptr;
		
		itemDescription = FText::FromString(TEXT("A usable item"));
		isStackable = true;
		maxStackSize = 1;
	}
	
	FInventoryItem(FText name, TEnumAsByte<EItemType> type, UTexture2D* icon, TSubclassOf<class AItemBase> item)
		: itemName(name), itemType(type), itemIcon(icon), itemClass(item)
	{
		itemDescription = FText::FromString(TEXT("A usable item"));
		isStackable = true;
		maxStackSize = 1;
	}

	// Item's name
	UPROPERTY(BlueprintReadWrite)
	FText itemName;

	// Item type
	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<EItemType> itemType;
	
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
		isSelected = false;
	}

	// Item struct
	UPROPERTY(BlueprintReadWrite)
	FInventoryItem item;
	
	// The quantity of the item in the slot
	UPROPERTY(BlueprintReadWrite)
	int quantity;

	bool isSelected;
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuickslotsUpdated);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTMOUSTACHE_API UInventoryComponentBase : public UActorComponent
{
	GENERATED_BODY()

	/*UPROPERTY()
	UInputComponent* InputComponent;*/

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<AItemBase> slot1Item;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSubclassOf<AItemBase> slot2Item;

	FInventorySlot slot1;
	FInventorySlot slot2;

	// The maximum amount of items in the slot
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	int maxSlotSize;

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

	int selectedItem;

public:	
	// Sets default values for this component's properties
	UInventoryComponentBase();

protected:
	
	// Array of items in the inventory
	UPROPERTY(BlueprintReadWrite)
	TArray<FInventorySlot> inventory;

	// Number of inventory slots available
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int numberOfSlots;

	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void RemoveItem(int index);

	void UseQuickSlot1();
	void UseQuickSlot2();
	void UseQuickSlot3();
	void UpdateQuickSlots();
	
public:

	UPROPERTY(BlueprintAssignable)
	FOnQuickslotsUpdated OnQuickslotsUpdated;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool UseItem(int index);
	bool UseItem_Implementation(int index) { return false; }

	UFUNCTION(BlueprintCallable)
	bool UseQuickSlot(int slotNumber);

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
	FInventorySlot GetInventoryItemAtIndex(int index);

	UFUNCTION(BlueprintCallable)
	TArray<FInventorySlot> GetInventory() { return inventory; }

	UFUNCTION(BlueprintCallable)
	void SetQuickslotItem(int inventoryIndex, int quickSlot);

	/**
	 * Returns a reference to the quick slot.
	 * If an index outside of 1 - 3 is passed in, a default quick slot
	 * object with slot index of -1 will be returned
	 */
	UFUNCTION(BlueprintCallable)
	FQuickSlot GetQuickSlot(int quickSlotIndex);

	UFUNCTION(BlueprintCallable)
	bool SetSelectedItem(int itemIndex);

	UFUNCTION(BlueprintCallable)
	void ClearSelectedItem();

	UFUNCTION(BlueprintCallable)
	int GetSelectedItem() { return selectedItem; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FInventorySlot GetQuickSlot1() const { return slot1;}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FInventorySlot GetQuickSlot2() const { return slot2;}
};
