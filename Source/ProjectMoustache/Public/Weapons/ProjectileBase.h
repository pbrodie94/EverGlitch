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

	UPROPERTY(EditAnywhere)
	float lifeSpan;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* sphereCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* projectileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UProjectileMovementComponent* projectileMovement;

	UPROPERTY(BlueprintReadOnly)
	float damage;

public:
	// Sets default values for this actor's properties
	AProjectileBase();

	UFUNCTION(BlueprintCallable)
	void SetProjectileSpeed(float speed);

	UFUNCTION(BlueprintCallable)
	void SetShouldBounce(bool shouldBounce);

	UFUNCTION()
	void SetDamage(float damageAmount);

	/*FORCEINLINE void SetDamageMultiplier(float multiplier);*/
};
