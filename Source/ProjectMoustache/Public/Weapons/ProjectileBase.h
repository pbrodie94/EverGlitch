// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ProjectileBase.generated.h"

UCLASS()
class PROJECTMOUSTACHE_API AProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USphereComponent* sphereCollider;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* projectileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* projectileMovement;

	UPROPERTY(BlueprintReadOnly)
	float damage;

	UPROPERTY(BlueprintReadOnly)
	APawn* myOwner;

	UFUNCTION()
	virtual void OnBeginOverlapEvent(UPrimitiveComponent* overlappedComp, class AActor* otherActor,
		class UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep,
		const FHitResult& sweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Initialize(float projectileDamage, APawn* owner);

};
