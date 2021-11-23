#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

UCLASS()
class PROJECTMOUSTACHE_API AItemBase : public AActor
{
	GENERATED_BODY()
	
	public:	
	// Sets default values for this actor's properties
	AItemBase();

	protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Item's display name
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties)
	FText itemName;

	// Items description, and use effects
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties)
	FText itemDescription;

	// Item's ui display icon
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = InventoryProperties)
	UTexture2D* itemIcon;

	// Whether or not the item can be stacked in inventory
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = InventoryProperties)
	bool isStackable;

	// Max stack size of the item in inventory
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = InventoryProperties)
	int maxStackSize;
	
	public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**
	 * Use function to be overridden by item subclasses to perform their unique use functions
	 * Takes in the user actor, and returns whether or not the use was successful
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool Use(AActor* userActor);

	UFUNCTION(BlueprintCallable)
	FText GetItemName() { return itemName; }

	UFUNCTION(BlueprintCallable)
	FText GetItemDescription() { return itemDescription; }

	UFUNCTION(BlueprintCallable)
	UTexture2D* GetItemIcon() { return itemIcon; }

	UFUNCTION(BlueprintCallable)
	bool GetIsStackable() { return isStackable; }

	UFUNCTION(BlueprintCallable)
	int GetMaxStackSize() { return maxStackSize; }
};