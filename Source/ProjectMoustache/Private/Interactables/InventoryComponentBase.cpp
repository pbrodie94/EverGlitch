// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/InventoryComponentBase.h"

// Sets default values for this component's properties
UInventoryComponentBase::UInventoryComponentBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
}


// Called when the game starts
void UInventoryComponentBase::BeginPlay()
{
	Super::BeginPlay();

	// ...

	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		InputComponent->BindAction("QuickSlot1", IE_Pressed, this, &UInventoryComponentBase::UseQuickSlot1);
		InputComponent->BindAction("QuickSlot2", IE_Pressed, this, &UInventoryComponentBase::UseQuickSlot2);
		InputComponent->BindAction("QuickSlot3", IE_Pressed, this, &UInventoryComponentBase::UseQuickSlot3);
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("Error: InventoryComponentBase failed to get input component reference."));
	}

	inventory.SetNum(numberOfSlots);
}


// Called every frame
void UInventoryComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UInventoryComponentBase::AddToInventory(FInventorySlot itemToAdd)
{
	if (!itemToAdd.item.isStackable)
	{
		return CreateStack(itemToAdd);
	}

	if (GetHasPartialStack(itemToAdd))
	{
		return AddToStack(itemToAdd, GetPartialStackIndex(itemToAdd));
	}

	return CreateStack(itemToAdd);
}

/**
* Creates a new stack of items
* Does not add the item if inventory is full
* Adds multiple items if the item is stackable and has more than 1
* Otherwise, just adds one of the item
*/
bool UInventoryComponentBase::CreateStack(FInventorySlot itemToAdd)
{
	int freeIndex = GetNextEmptySlot();
	if (freeIndex == -1)
	{
		return false;
	}

	inventory[freeIndex].item = itemToAdd.item;
	if (itemToAdd.item.isStackable && itemToAdd.quantity > 1)
	{
		inventory[freeIndex].quantity = itemToAdd.quantity;
		return true;
	}

	inventory[freeIndex].quantity = 1;
	return true;
}

/**
* Adds the picked up item to an existing stack
* If there are too many items to fit into the stack, it will recursively try to find other partial stacks
* If no other partial stacks found, it will try to create a new stack
*/
bool UInventoryComponentBase::AddToStack(FInventorySlot itemToAdd, int index)
{
	if (itemToAdd.quantity + inventory[index].quantity <= inventory[index].item.maxStackSize)
	{
		inventory[index].quantity = inventory[index].quantity + itemToAdd.quantity;
		return true;
	}
	
	int overflow = (itemToAdd.quantity + inventory[index].quantity) - inventory[index].item.maxStackSize;
	inventory[index].quantity = inventory[index].item.maxStackSize;

	FInventorySlot overflowItem;
	overflowItem.item = itemToAdd.item;
	overflowItem.quantity = overflow;

	if (GetHasPartialStack(itemToAdd))
	{
		int partialStackIndex = GetPartialStackIndex(overflowItem);
		AddToStack(overflowItem, partialStackIndex);
		return true;
	}

	// No other partial stacks, try to create a new one
	if (!CreateStack(overflowItem))
	{
		// Could not add the rest of the overflow
	}
	
	return true;
}

void UInventoryComponentBase::UseQuickSlot1()
{
	UseQuickSlot(quickSlot1);
}

void UInventoryComponentBase::UseQuickSlot2()
{
	UseQuickSlot(quickSlot2);
}

void UInventoryComponentBase::UseQuickSlot3()
{
	UseQuickSlot(quickSlot3);
}

void UInventoryComponentBase::UseQuickSlot(FQuickSlot quickSlot)
{
	if (quickSlot.itemIndex < 0)
	{
		return;
	}

	if (quickSlot.item.quantity <= 0)
	{
		return;
	}

	if (UseItem(quickSlot.itemIndex))
	{
		if (quickSlot.item.quantity <= 1)
		{
			
		}
	}
}


void UInventoryComponentBase::RemoveItem(int index)
{
	for (int i = index; i < inventory.Num() - 1; ++i)
	{
		if (inventory[i + 1].quantity <= 0)
		{
			inventory[i] = FInventorySlot();
			break;
		}

		inventory[i] = inventory[i + 1];
	}
}

/**
* Gets the index of a partial stack of the queried item
* returns -1 if no partial stacks are found
*/
int UInventoryComponentBase::GetPartialStackIndex(FInventorySlot queryItem)
{
	if (!GetHasPartialStack(queryItem))
	{
		return -1;
	}

	for (int i = 0; i < inventory.Num() - 1; ++i)
	{
		if (inventory[i].item.itemClass != queryItem.item.itemClass)
		{
			continue;
		}

		if (inventory[i].quantity >= inventory[i].item.maxStackSize)
		{
			continue;
		}

		if (!inventory[i].item.isStackable)
		{
			continue;
		}

		return i;
	}

	return -1;
}

/**
* Checks if the query item already has a partial stack in the inventory
*/
bool UInventoryComponentBase::GetHasPartialStack(FInventorySlot queryItem)
{
	for (auto item : inventory)
	{
		if (item.item.itemClass != queryItem.item.itemClass)
		{
			continue;
		}

		if (!item.item.isStackable)
		{
			continue;
		}

		if (item.quantity < item.item.maxStackSize)
		{
			return true;
		}
	}

	return false;
}

// Returns -1 if inventory is full
int UInventoryComponentBase::GetNextEmptySlot()
{
	for (int i = 0; i < inventory.Num() - 1; ++i)
	{
		if (inventory[i].quantity <= 0)
		{
			return i;
		}
	}

	return -1;
}

/**
* Checks if the inventory has an item
*/
bool UInventoryComponentBase::GetHasItem(TSubclassOf<class AItemBase> queryItem)
{
	for (auto item : inventory)
	{
		if (item.item.itemClass == queryItem)
		{
			return true;
		}

		if (item.quantity <= 0)
		{
			return false;
		}
	}

	return false;
}

/**
* Checks if the inventory has a specific item and quantity of item
*/
bool UInventoryComponentBase::GetHasItemWithQuantity(TSubclassOf<AItemBase> queryItem, int queryQuantity)
{
	int quantityCount = 0;
	for (auto item : inventory)
	{
		if (item.item.itemClass != queryItem)
		{
			continue;
		}

		quantityCount += item.quantity;
		if (quantityCount >= queryQuantity)
		{
			return true;
		}
	}

	return false;
}

/**
* Returns a reference to the quick slot.
* If an index outside of 1 - 3 is passed in, a default quick slot
* object with slot index of -1 will be returned
*/
FQuickSlot UInventoryComponentBase::GetQuickSlot(int quickSlotIndex)
{
	switch(quickSlotIndex)
	{
		case 1:
			return quickSlot1;

		case 2:
			return quickSlot2;

		case 3:
			return quickSlot3;
		
		default:
			return FQuickSlot();
	}
}
