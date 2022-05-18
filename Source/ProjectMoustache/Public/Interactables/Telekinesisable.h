
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Telekinesisable.generated.h"

UINTERFACE(BlueprintType)
class UTelekinesisable : public UInterface
{
	GENERATED_BODY()
};

class PROJECTMOUSTACHE_API ITelekinesisable
{
	GENERATED_BODY()

public:

	FVector GetPosition();

	UFUNCTION(BlueprintImplementableEvent)
	class UBoxComponent* GetBoxCollider();

	// Set appropriate settings in preparation for being picked up
	UFUNCTION(BlueprintImplementableEvent)
	void SetPickedUp();

	// Revert settings to default after being dropped
	UFUNCTION(BlueprintImplementableEvent)
	void SetDropped();

	UFUNCTION(BlueprintImplementableEvent)
	void SetHighlightOutline();

	UFUNCTION(BlueprintImplementableEvent)
	void RemoveHighlightOutline();
};