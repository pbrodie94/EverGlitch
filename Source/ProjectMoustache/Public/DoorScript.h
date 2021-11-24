// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TriggerVolume.h"
#include "DoorScript.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTMOUSTACHE_API UDoorScript : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDoorScript();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void OpenDoor();
	void CloseDoor();
		

private:
	AActor* Owner;
	UPROPERTY(EditAnywhere)
		ATriggerVolume* TriggerVolume;
	AActor* DefaultPawn;

	float DoorOpenDuration = 0.5;
	float LastDoorOpenTime = 0;

	float GetTotalMassInTrigger();
};
