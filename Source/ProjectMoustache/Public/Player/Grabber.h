// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "Components/ActorComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Components/InputComponent.h"
#include "Grabber.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTMOUSTACHE_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float grabDistance;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float grabInterpSpeed;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FVector holdOffset;

	UPROPERTY()
	UInputComponent* InputComponent = nullptr;

	UPROPERTY()
	UPhysicsHandleComponent* PhysicsHandle = nullptr;

	UPROPERTY()
	TScriptInterface<IPlayerCharacter> playerCharacter;

	FVector GetLineStart();
	FVector GetLineEnd();
	FHitResult LineTrace();

	void Grab();
	void Release();

public:
	// Sets default values for this component's properties
	UGrabber();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
