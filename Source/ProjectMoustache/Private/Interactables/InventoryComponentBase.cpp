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
	UInputComponent* inputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (inputComponent != nullptr)
	{
		inputComponent->BindAction("QuickSlot1", IE_Pressed, this, &UInventoryComponentBase::UseQuickSlotOne);
		inputComponent->BindAction("QuickSlot2", IE_Pressed, this, &UInventoryComponentBase::UseQuickSlotTwo);
		inputComponent->BindAction("QuickSlot3", IE_Pressed, this, &UInventoryComponentBase::UseQuickSlotThree);
	}

	inventory.SetNum(numberOfSlots);

}


// Called every frame
void UInventoryComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UInventoryComponentBase::AddToInventory(FInventorySlot newItem)
{
	if (newItem.item.isStackable)
	{
		if (GetItemHasPartialStack(newItem))
		{
			int partialStack = GetItemPartialStackIndex(newItem);
			if (partialStack >= 0)
			{
				return AddToStack(newItem, GetItemPartialStackIndex(newItem));
			}
		}
	}

	return CreateStack(newItem);
}

bool UInventoryComponentBase::CreateStack(FInventorySlot newItem)
{
	for (int i = 0; i < inventory.Num() - 1; ++i)
	{
		if (inventory[i].quantity <= 0)
		{
			inventory[i] = newItem;
			return true;
		}
	}

	return false;
}

bool UInventoryComponentBase::AddToStack(FInventorySlot newItem, int slotIndex)
{
	if (inventory[slotIndex].quantity + newItem.quantity > inventory[slotIndex].item.maxStackSize)
	{
		int excessItem = (inventory[slotIndex].quantity + newItem.quantity) - inventory[slotIndex].item.maxStackSize;
		FInventorySlot modifiedItem = newItem;
		modifiedItem.quantity = excessItem;
		AddToInventory(modifiedItem);
		return true;
	}

	inventory[slotIndex].quantity += newItem.quantity;
	return true;
}

bool UInventoryComponentBase::GetHasItem(TSubclassOf<AItemBase> queryItem)
{
	for (auto item : inventory)
	{
		if (item.item.itemClass == queryItem)
		{
			return true;
		}
	}

	return false;
}

bool UInventoryComponentBase::GetItemHasPartialStack(FInventorySlot queryItem)
{
	for (auto item : inventory)
	{
		if (item.quantity <= 0)
		{
			return false;
		}
		
		if (queryItem.item.itemClass == item.item.itemClass)
		{
			if (item.quantity < item.item.maxStackSize)
			{
				return true;
			}
		}
	}

	return false;
}

int UInventoryComponentBase::GetItemPartialStackIndex(FInventorySlot queryItem)
{
	for (int i = 0; i < inventory.Num() - 1; ++i)
	{
		if (queryItem.item.itemClass == inventory[i].item.itemClass)
		{
			if (inventory[i].quantity < inventory[i].item.maxStackSize)
			{
				return i;
			}
		}
	}

	return -1;
}


int UInventoryComponentBase::GetItemQuantity(TSubclassOf<AItemBase> queryItem)
{
	int itemQuantity = 0;
	for (auto item : inventory)
	{
		if (item.item.itemClass == queryItem)
		{
			itemQuantity += item.quantity;
		}
	}

	return itemQuantity;
}

void UInventoryComponentBase::RemoveItem(int itemIndex)
{
	if (itemIndex < 0 || itemIndex > inventory.Num() - 1)
	{
		return;
	}

	inventory[itemIndex] = FInventorySlot();
	
	for (int i = itemIndex; i < inventory.Num() - 2; ++i)
	{
		if (inventory[i + 1].quantity > 0)
		{
			inventory[i] = inventory[i + 1];
		} else
		{
			break;
		}
	}
}

int UInventoryComponentBase::GetItemIndex(FInventorySlot queryItem)
{
	for (int i = 0; i < inventory.Num() - 1; ++i)
	{
		if (inventory[i].item.itemClass == queryItem.item.itemClass)
		{
			return i;
		}
	}
	
	return -1;
}


FInventorySlot UInventoryComponentBase::GetInventoryItemAtIndex(int index)
{
	if (index > inventory.Num() - 1 || index < 0)
	{
		FInventorySlot nullSlot;
		nullSlot.quantity = -1;
		return nullSlot;
	}

	return inventory[index];
}


void UInventoryComponentBase::UseQuickSlotOne()
{
	if (quickSlot1.inventoryItem.quantity > 0)
	{
		if (UseItem(quickSlot1.inventoryIndex))
		{
			if (quickSlot1.inventoryItem.quantity <= 1)
			{
				UpdateQuickslots(1);
				quickSlot1 = FQuickSlotItem();
				return;
			}
		}
		SetQuickSlotItem(1, quickSlot1.inventoryIndex);
	}
}

void UInventoryComponentBase::UseQuickSlotTwo()
{
	if (quickSlot2.inventoryItem.quantity > 0)
	{
		if (UseItem(quickSlot2.inventoryIndex))
		{
			if (quickSlot2.inventoryItem.quantity <= 1)
			{
				UpdateQuickslots(2);
				quickSlot2 = FQuickSlotItem();
				return;
			} 
		}
		SetQuickSlotItem(2, quickSlot2.inventoryIndex);
	}
}

void UInventoryComponentBase::UseQuickSlotThree()
{
	if (quickSlot3.inventoryItem.quantity > 0)
	{
		if (UseItem(quickSlot3.inventoryIndex))
		{
			if (quickSlot3.inventoryItem.quantity <= 1)
			{
				UpdateQuickslots(3);
				quickSlot3 = FQuickSlotItem();
				return;
			}
		}
		SetQuickSlotItem(3, quickSlot3.inventoryIndex);
	}	
}

void UInventoryComponentBase::UpdateQuickslots(int slotIndex)
{
	int itemIndex = 0;
	
	switch(slotIndex)
	{
	case 1:
		itemIndex = quickSlot1.inventoryIndex;

		if (quickSlot2.inventoryIndex > itemIndex)
		{
			--quickSlot2.inventoryIndex;
		}

		if (quickSlot3.inventoryIndex > itemIndex)
		{
			--quickSlot3.inventoryIndex;
		}
		
		break;

	case 2:

		itemIndex = quickSlot2.inventoryIndex;

		if (quickSlot1.inventoryIndex > itemIndex)
		{
			--quickSlot1.inventoryIndex;
		}

		if (quickSlot3.inventoryIndex > itemIndex)
		{
			--quickSlot3.inventoryIndex;
		}
		
		break;

	case 3:
		itemIndex = quickSlot3.inventoryIndex;

		if (quickSlot1.inventoryIndex > itemIndex)
		{
			--quickSlot1.inventoryIndex;
		}

		if (quickSlot2.inventoryIndex > itemIndex)
		{
			--quickSlot2.inventoryIndex;
		}
		
		break;

	default:
		
		break;
	}
}


FQuickSlotItem UInventoryComponentBase::GetQuickSlotItem(int quickSlot)
{
	switch(quickSlot)
	{
		case 1:
			return quickSlot1;
			break;

		case 2:
			return quickSlot2;
			break;

		case 3:
			return quickSlot3;
			break;

		default:
			return FQuickSlotItem();
			break;
	}
}

void UInventoryComponentBase::SetQuickSlotItem(int quickSlot, int itemIndex)
{
	DeselectAll();

	FQuickSlotItem newQuickItem;
	newQuickItem.inventoryIndex = itemIndex;
	newQuickItem.inventoryItem = GetInventoryItemAtIndex(itemIndex);

	RemoveDuplicateQuickSlotItems(quickSlot, itemIndex);
	
	switch (quickSlot)
	{
	case 1:
		quickSlot1 = newQuickItem;
		break;

	case 2:
		quickSlot2 = newQuickItem;
		break;

	case 3:
		quickSlot3 = newQuickItem;
		break;

	default:
		// Do nothing
		break;
	}
}

void UInventoryComponentBase::RemoveDuplicateQuickSlotItems(int slotNumber, int itemIndex)
{
	switch (slotNumber)
	{
	case 1:
		if (quickSlot2.inventoryIndex == itemIndex)
		{
			quickSlot2 = FQuickSlotItem();
		}
		if (quickSlot3.inventoryIndex == itemIndex)
		{
			quickSlot3 = FQuickSlotItem();
		}
		
		break;

	case 2:
		if (quickSlot1.inventoryIndex == itemIndex)
		{
			quickSlot1 = FQuickSlotItem();
		}
		if (quickSlot3.inventoryIndex == itemIndex)
		{
			quickSlot3 = FQuickSlotItem();
		}
		break;

	case 3:
		if (quickSlot1.inventoryIndex == itemIndex)
		{
			quickSlot1 = FQuickSlotItem();
		}
		if (quickSlot2.inventoryIndex == itemIndex)
		{
			quickSlot2 = FQuickSlotItem();
		}
		break;

	default:
		// Do nothing
		break;
	}
}


bool UInventoryComponentBase::UseItem_Implementation(int itemIndex)
{
	inventory[itemIndex].isSelected = false;
	return true;
}

void UInventoryComponentBase::SetSelectedItem(int index)
{
	DeselectAll();

	if (inventory[index].quantity > 0)
	{
		inventory[index].isSelected = true;
	}

	RefeshInventory();
}

void UInventoryComponentBase::DeselectItem(int index)
{
	inventory[index].isSelected = false;
}


int UInventoryComponentBase::GetSelectedItemIndex()
{
	for (int i = 0; i < inventory.Num() - 1; ++i)
	{
		if (inventory[i].quantity <= 0)
		{
			break;
		}
		
		if (inventory[i].isSelected)
		{
			return i;
		}
	}

	return -1;
}

void UInventoryComponentBase::DeselectAll()
{
	for (int i = 0; i < inventory.Num() - 1; ++i)
	{
		inventory[i].isSelected = false;
	}
}
