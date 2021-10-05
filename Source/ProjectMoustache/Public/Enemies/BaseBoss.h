// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "EnemyComponents/StatueSpawner.h"
#include "GameFramework/Character.h"
#include "Weapons/ProjectileBase.h"

#include "BaseBoss.generated.h"

UENUM(BlueprintType)
enum EAttackType
{
	Melee UMETA(DisplayName = "Melee"),
	Projectile UMETA(DisplayName = "Projectile"),
	Beam UMETA(DisplayName = "Beam"),
	GroundSlam UMETA(DisplayName = "GroundSlam")
};

UENUM(BlueprintType)
enum EChangeSizeState
{
	Neutral UMETA(DisplayName = "Neutral"),
	Growing UMETA(DisplayName = "Growing"),
	Shrinking UMETA(DisplayName = "Shrinking")
};

UCLASS()
class PROJECTMOUSTACHE_API ABaseBoss : public ACharacter
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	FName throwningSocketName;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float growSizeCoolDown;
	
	float timeLastGrowSize;
	float timeBeganChangeSize;

	bool battleBegun;
	bool isLarge;

	FVector GetThrowingDirection(FVector baseDirection, float angle);
	
public:
	// Sets default values for this character's properties
	ABaseBoss();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly)
	int bossPhase;

	//The final boss phase
	UPROPERTY(BlueprintReadOnly)
	int finalBossPhase;

	UPROPERTY(BlueprintReadOnly)
	float health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float maxHealth;

	UPROPERTY(BlueprintReadOnly)
	float walkSpeed;

	UPROPERTY(BlueprintReadOnly)
	float runSpeed;

	//The distance threshold when the boss will transition from running to walking
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float runDistance;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EChangeSizeState> changeSizeState;
	
	UPROPERTY(BlueprintReadOnly)
	FVector normalSize;

	UPROPERTY(BlueprintReadOnly)
	FVector largeSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float growingTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float meleeDistance;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float meleeDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float projectileDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float groundSlamDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float beamDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStatueSpawner* statueSpawner;

	UFUNCTION(BlueprintCallable)
	void ThrowProjectiles();

	UFUNCTION(BlueprintCallable)
	void GroundSlamAttack();

	UFUNCTION(BlueprintCallable)
	void BeamAttack();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintImplementableEvent)
	void PlayAnimationMontage();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SpawnEffects(const TEnumAsByte<EAttackType>& attackType, const FTransform& spawnTransform);

	UFUNCTION(BlueprintCallable)
	void SetMoveSpeed(bool running);
	
	UFUNCTION(BlueprintImplementableEvent)
	void Die();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void BeginBattle();

	UFUNCTION(BlueprintCallable)
	float BeginGrowingSize();

	UFUNCTION(BlueprintCallable)
	float BeginShrinkSize();

	UFUNCTION(BlueprintCallable)
	int GetCurrentBossPhase() { return bossPhase; }

	UFUNCTION(BlueprintCallable)
	float GetCurrentHealth() { return health; }

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() { return maxHealth; }

	UFUNCTION(BlueprintCallable)
	bool GetIsLarge() { return isLarge; }

	UFUNCTION(BlueprintCallable)
	float GetMeleeDistance() { return meleeDistance; }

	UFUNCTION(BlueprintCallable)
	float GetRunDistance() { return runDistance; }

	UFUNCTION(BlueprintCallable)
	bool GetIsBattleBegun() { return battleBegun; }
};
