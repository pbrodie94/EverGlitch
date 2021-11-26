// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorScript.h"
#include "GameFrameWork/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UDoorScript::UDoorScript()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDoorScript::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();
	DefaultPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	
}


// Called every frame
void UDoorScript::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TriggerVolume && GetTotalMassInTrigger() > 15)
	{
		OpenDoor();
		LastDoorOpenTime = GetWorld()->GetTimeSeconds();
	}

	if (GetWorld()->GetTimeSeconds() - LastDoorOpenTime > DoorOpenDuration)
	{
		CloseDoor();
	}
}

void UDoorScript::OpenDoor()
{
	FRotator NewRotator = FRotator(0, 0, 0);
	Owner->SetActorRotation(NewRotator);
}

void UDoorScript::CloseDoor()
{
	FRotator NewRotator = FRotator(0, -90, 0);
	Owner->SetActorRotation(NewRotator);
}

float UDoorScript::GetTotalMassInTrigger()
{
	TArray<AActor*> Actors;
	if (TriggerVolume == nullptr)
	{
		return 0;
	}

	TriggerVolume->GetOverlappingActors(Actors);

	float TotalMass = 0;
	for (const auto& Actor : Actors)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();

	}
	UE_LOG(LogTemp, Warning, TEXT("Total Mass is %f ! "), TotalMass);
	return TotalMass;
}

