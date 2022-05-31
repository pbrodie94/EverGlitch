// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UCLASS()
class PROJECTMOUSTACHE_API AWeaponBase : public AActor
{
	GENERATED_BODY()

	// Staff Mesh
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	USkeletalMeshComponent* staffMesh;

	// Position in local space the projectile will fire from
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	USceneComponent* firePoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float fireRate;

	float timeNextFire;

	bool isFiring;

	void Fire();
	
protected:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AProjectileBase> projectile;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this actor's properties
	AWeaponBase();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent)
	bool OnFireDown();
	bool OnFireDown_Implementation();
	
	UFUNCTION(BlueprintNativeEvent)
	bool OnFireUp();
	bool OnFireUp_Implementation();

	FORCEINLINE USceneComponent* GetFirePoint() const { return firePoint; }

};
