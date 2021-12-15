// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Grabber.h"
#include "Engine/World.h"

#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();

	if (InputComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Input Component has found "));
		InputComponent->BindAction("Action", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Action", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Input Component has not found "));
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
	FVector Start;
	FRotator ViewRotator;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(Start, ViewRotator);
	return Start;
}

FVector UGrabber::GetLineEnd()
{
	FVector Start;
	FRotator ViewRotator;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(Start, ViewRotator);
	return Start + ViewRotator.Vector() * 300;;
}

FHitResult UGrabber::LineTrace()
{
	DrawDebugLine(GetWorld(), GetLineStart(), GetLineEnd(), FColor(255, 0, 0), false, 0, 0, 10);
	FHitResult Hit;

	GetWorld()->LineTraceSingleByObjectType(
		Hit,
		GetLineStart(),
		GetLineEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		FCollisionQueryParams(FName(TEXT("")), false, GetOwner())
	);

	AActor* Actor = Hit.GetActor();
	if (Actor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Line Hit:%s "), *Actor->GetName());
	}
	return Hit;
}

void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab has pressed "));
	FHitResult Hit = LineTrace();
	UPrimitiveComponent* ComponentToGrab = Hit.GetComponent();
	if (Hit.GetActor() && PhysicsHandle)
	{
		PhysicsHandle->GrabComponentAtLocationWithRotation(ComponentToGrab, NAME_None, ComponentToGrab->GetOwner()->GetActorLocation(), GetOwner()->GetActorRotation());
		UE_LOG(LogTemp, Warning, TEXT("Grabed! "));
	}
}

void UGrabber::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("Release has pressed "));
	if (PhysicsHandle)
	{
		PhysicsHandle->ReleaseComponent();
	}
}

