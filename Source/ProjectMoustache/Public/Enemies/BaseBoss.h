// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyComponents/StatueSpawner.h"
#include "GameFramework/Character.h"
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

	//Reference to the player's actor 
	AActor* playerReference;

	//Names of sockets exposed to be edited in engine in case of changes
	UPROPERTY(EditDefaultsOnly)
	FName throwningSocketName;

	UPROPERTY(EditDefaultsOnly)
	FName meleeSocketName;

	UPROPERTY(EditDefaultsOnly)
	FName meleeArmSocketName;

	//The cooldown after boss has grown
	UPROPERTY(EditDefaultsOnly, Category = Abilities, meta = (AllowPrivateAccess = true))
	float growSizeCoolDown;
	
	float timeLastGrowSize;
	float timeBeganChangeSize;
	
	bool isGroundSlamSequence;
	int spawnedShockwaves;
	float timeLastShockwave;

	//Percentage (%) of damage reduction per phase
	UPROPERTY(EditDefaultsOnly, Category = Status, meta = (AllowPrivateAccess = true))
	int damageReductionPercentage;

	//The multiplier used to reduce damage intake
	float damageReductionMultiple;

	//The radius used by the sphere check for detecting hits on melee attacks
	UPROPERTY(EditDefaultsOnly, Category = Combat)
	float meleeHitRadius;

	//Interval between ground slam shockwaves
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	float groundSlamInterval;

	//Holder for beam attack particle system
	UParticleSystemComponent* beamParticles;

	//The radius used for the sphere trace to detect what the beam attack hits
	UPROPERTY(EditDefaultsOnly)
	float beamHitRadius;

	//The charging interval before the beam attack is fired
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	float beamChargingTime;

	//The total range the beam attack has
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	float beamRange;

	//The speed the beam follows the player
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	float beamSpeed;

	//The interval between damage ticks when player is in the beam attack
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	float beamDamageInterval;

	float timeLastDamaged;

	//Max angle the beam can be fired from the forward position, before the target gets too far from boss's front
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	float maxBeamAngle;

	FVector beamTarget;
	FVector beamEnd;

	bool battleBegun;
	bool isLarge;

	/**
	 * Takes in a vector direction and angle and returns the rotated vector
	 */
	FVector GetThrowingDirection(FVector baseDirection, float angle);

	//Function called tick function to handle the size changing sequence
	void HandleBossSizeChange();
	
public:
	// Sets default values for this character's properties
	ABaseBoss();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Current phase the boss is in
	UPROPERTY(BlueprintReadOnly)
	int bossPhase;

	//The final boss phase
	UPROPERTY(BlueprintReadOnly)
	int finalBossPhase;

	UPROPERTY(BlueprintReadOnly)
	float health;

	UPROPERTY(EditDefaultsOnly, Category = Status, BlueprintReadOnly)
	float maxHealth;

	UPROPERTY(BlueprintReadOnly)
	float walkSpeed;

	UPROPERTY(BlueprintReadOnly)
	float runSpeed;

	//The distance threshold when the boss will transition from running to walking
	UPROPERTY(EditDefaultsOnly, Category = Abilities, BlueprintReadOnly)
	float runDistance;

	//State the boss is in (Growing, Shrinking, Neutral)
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EChangeSizeState> changeSizeState;

	//Reference to the normal size of the boss
	UPROPERTY(BlueprintReadOnly)
	FVector normalSize;

	//The boss' large size scale
	UPROPERTY(BlueprintReadOnly)
	FVector largeSize;

	//Time it takes for the boss to reach the desired size
	UPROPERTY(EditDefaultsOnly, Category = Abilities, BlueprintReadOnly)
	float growingTime;

	//The distance the boss will melee attack
	UPROPERTY(EditDefaultsOnly, Category = Combat, BlueprintReadOnly)
	float meleeDistance;
	
	UPROPERTY(EditDefaultsOnly, Category = Combat, BlueprintReadOnly)
	float meleeDamage;

	UPROPERTY(EditDefaultsOnly, Category = Combat, BlueprintReadOnly)
	float projectileDamage;

	UPROPERTY(EditDefaultsOnly, Category = Combat, BlueprintReadOnly)
	float groundSlamDamage;

	UPROPERTY(EditDefaultsOnly, Category = Combat, BlueprintReadOnly)
	float beamDamage;

	UPROPERTY(BlueprintReadWrite)
	bool isAttacking;
	
	UPROPERTY(BlueprintReadOnly)
	float timeBeganBeamAttack;

	UPROPERTY(BlueprintReadWrite)
	bool isMeleeAttacking;
	
	UPROPERTY(BlueprintReadWrite)
	bool isBeamAttacking;

	/**
	 * Reference to the statue spawner component class
	 * Handles the spawning of statues when the boss enters into the growing/large state
	 * Keeps track of the statues as they are destroyed, then triggers the boss to shrink when all are destroyed
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UStatueSpawner* statueSpawner;

	/**
	 * Gets a random number of projectiles to throw between 1 and a high end range of 3 - 8 depending on the boss phase
	 * Calculates the angles needed to throw multiple projectiles in a fan pattern,
	 * with the origin in the direction of the player
	 */
	UFUNCTION(BlueprintCallable)
	void ThrowProjectiles();

	/**
	 * Finds the ground position below the boss, and gets a position close to the ground to spawn shockwaves
	 * If the boss phase is greater than 1, a sequence is started where shockwaves are spawned for each phase
	 */
	UFUNCTION(BlueprintCallable)
	void GroundSlamAttack();

	//Begins the beam attack sequence
	UFUNCTION(BlueprintCallable)
	void BeginBeamAttack();

	/**
	 * Performs the beam attack calculations updating the beam's target point, and slowly
	 * aiming towards the player.
	 * Handles hit detection, and damages on a tick
	 */
	void BeamAttack(float deltaTime);

	/**
	 * Initiates the melee attack detection
	 */
	UFUNCTION(BlueprintCallable)
	void BeginMeleeDamage();

	/**
	 * Detects hits with a sphere cast at a point in the boss' attacking arm at a set radius
	 * and stores hit actors in a TArray once they've been damaged
	 */
	void DetectMeleeHits();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/**
	 * Calls to the blueprint class to play animations
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void PlayAnimationMontage();

	/**
	 * Calls to the blueprint class to spawn actors
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	AActor* SpawnEffects(const TEnumAsByte<EAttackType>& attackType, const FTransform& spawnTransform);

	/**
	 * Calls to the blueprint class to spawn the beam particle system for the beam attack, then returns a pointer reference
	 */
	UFUNCTION(BlueprintImplementableEvent)
	UParticleSystemComponent* SpawnBeamParticles();

	/**
	 * Stores a list of hit actors for the beam attack and melee attacks to manage how many times individual actors are hit
	 */
	TArray<AActor*> hitActors;
	
	/**
	 * Called to clean up attack sequences
	 */
	UFUNCTION(BlueprintCallable)
	void OnEndingDamage();

	/**
	 * Called to change move speeds between walking and running
	 * Boss cannot run while in the large state
	 */
	UFUNCTION(BlueprintCallable)
	void SetMoveSpeed(bool running);
	
	/**
	 * Used to get a relevant move speed for the animation blueprint
	 * Move speed is adjusted when boss enters large state, this returns the relative move speed without
	 * adjustments for the size state to ensure accurate locomotion animation
	 */
	UFUNCTION(BlueprintCallable)
	float GetModifiedMoveSpeed();
	
	UFUNCTION(BlueprintImplementableEvent)
	void Die();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Begins the battle when the player enters the arena
	UFUNCTION(BlueprintCallable)
	void BeginBattle();

	//Sets boss to growing state, and returns growing time
	UFUNCTION(BlueprintCallable)
	float BeginGrowingSize();

	//Sets boss to shrinking state, and returns shrinking time
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

	UFUNCTION(BlueprintCallable)
	bool GetIsAttacking() { return isAttacking; }
};
