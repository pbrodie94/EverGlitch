// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Grabber.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	grabDistance = 150;
	holdOffset = FVector(100, 0, -50);
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();

	if (InputComponent)
	{
		InputComponent->BindAction("Action", IE_Pressed, this, &UGrabber::Grab);
		//InputComponent->BindAction("Action", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Input Component has not found "));
	}

	if (GetOwner()->GetClass()->ImplementsInterface(UPlayerCharacter::StaticClass()))
	{
		playerCharacter.SetInterface(GetOwner());
		playerCharacter.SetObject(GetOwner());
	}

}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PhysicsHandle && PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(GetLineEnd());
	}
}

FVector UGrabber::GetLineStart()
{
	return playerCharacter->Execute_GetPlayerLocation(GetOwner()) + UKismetMathLibrary::RotateAngleAxis(holdOffset,
		playerCharacter->Execute_GetPlayerRotation(GetOwner()).Yaw, FVector::UpVector);
}

FVector UGrabber::GetLineEnd()
{
	FVector Start = GetLineStart();
	return Start + (playerCharacter->Execute_GetPlayerForwardDirection(GetOwner()) * grabDistance);
}

FHitResult UGrabber::LineTrace()
{
	FHitResult Hit;

	GetWorld()->LineTraceSingleByObjectType(
		Hit,
		GetLineStart(),
		GetLineEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		FCollisionQueryParams(FName(TEXT("")), false, GetOwner())
	);

	return Hit;
}

void UGrabber::Grab()
{
	if (playerCharacter == nullptr)
	{
		return;
	}

	if (playerCharacter->Execute_GetHasInteractable(GetOwner()))
	{
		return;
	}

	if (PhysicsHandle && PhysicsHandle->GrabbedComponent)
	{
		Release();
		return;
	}

	FHitResult Hit = LineTrace();
	UPrimitiveComponent* ComponentToGrab = Hit.GetComponent();
	if (ComponentToGrab != nullptr && PhysicsHandle)
	{
		PhysicsHandle->GrabComponentAtLocationWithRotation(ComponentToGrab, NAME_None, ComponentToGrab->GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
	}
}

void UGrabber::Release()
{
	if (PhysicsHandle)
	{
		PhysicsHandle->ReleaseComponent();
	}
}
