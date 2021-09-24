// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseBossPowerStatue.generated.h"

DECLARE_DELEGATE_OneParam(FOnStatueDestroyed, class ABaseBossPowerStatue*)

UCLASS()
class PROJECTMOUSTACHE_API ABaseBossPowerStatue : public AActor
{
	GENERATED_BODY()

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	float health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float defaultHealth;
	
public:	
	// Sets default values for this actor's properties
	ABaseBossPowerStatue();

	FOnStatueDestroyed OnStatueDestroyed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
