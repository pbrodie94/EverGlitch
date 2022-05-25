// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UCLASS()
class PROJECTMOUSTACHE_API AWeaponBase : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float fireRate;

	float timeNextFire;
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

protected:

	UPROPERTY(BlueprintReadOnly)
	APawn* currentUser;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetUser(APawn* user) { currentUser = user; }

	UFUNCTION(BlueprintNativeEvent)
	bool OnFireDown();
	bool OnFireDown_Implementation();
	
	UFUNCTION(BlueprintNativeEvent)
	bool OnFireUp();
	bool OnFireUp_Implementation();

};
