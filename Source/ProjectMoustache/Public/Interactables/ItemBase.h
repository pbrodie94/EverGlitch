#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

UENUM(BlueprintType, Blueprintable)
enum EItemType
{
	Health UMETA(DisplayName = "Health"),
	Damage UMETA(DisplayName = "Damage"),
};

UCLASS()
class PROJECTMOUSTACHE_API AItemBase : public AActor
{
	GENERATED_BODY()

	protected:
	
	// Item's display name
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = ItemProperties)
	FText itemName;

	// Item type
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = ItemProperties)
	TEnumAsByte<EItemType> itemType;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = InventoryProperties)
	int quantity;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	

	// Sets default values for this actor's properties
	AItemBase();
	
	/**
	 * Use function to be overridden by item subclasses to perform their unique use functions
	 * Takes in the user actor, and returns whether or not the use was successful
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool Use(AActor* userActor);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FText GetItemName() const { return itemName; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE TEnumAsByte<EItemType> GetItemType() const { return itemType;}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FText GetItemDescription() const { return itemDescription; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UTexture2D* GetItemIcon() const { return itemIcon; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsStackable() const { return isStackable; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetMaxStackSize() const { return maxStackSize; }
};