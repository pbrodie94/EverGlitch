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
}


// Called every frame
void UInventoryComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
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
		UseItem(quickSlot1.inventoryIndex);
		SetQuickSlotItem(1, quickSlot1.inventoryIndex);
	}
}

void UInventoryComponentBase::UseQuickSlotTwo()
{
	if (quickSlot2.inventoryItem.quantity > 0)
	{
		UseItem(quickSlot2.inventoryIndex);
		SetQuickSlotItem(2, quickSlot2.inventoryIndex);
	}
}

void UInventoryComponentBase::UseQuickSlotThree()
{
	if (quickSlot3.inventoryItem.quantity > 0)
	{
		UseItem(quickSlot3.inventoryIndex);
		SetQuickSlotItem(3, quickSlot3.inventoryIndex);
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
	for (auto item : inventory)
	{
		item.isSelected = false;
	}
}
