// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/BaseBoss.h"
#include "Enemies/EnemyComponents/StatueSpawner.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Weapons/ProjectileBase.h"

// Sets default values
ABaseBoss::ABaseBoss()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	statueSpawner = CreateDefaultSubobject<UStatueSpawner>(TEXT("StatueSpawner"));

	battleBegun = false;
	maxHealth = 5000;
	
	bossPhase = 1;
	finalBossPhase = 3;

	walkSpeed = 150;
	runSpeed = 600;

	runDistance = 1500;

	changeSizeState = Neutral;
    growingTime = 5;
    isLarge = false;

	normalSize = GetActorScale3D();
	largeSize = normalSize * 5;
	growSizeCoolDown = 30;

	damageReductionPercentage = 15;

	meleeHitRadius = 150.0f;
	meleeDistance = 350;
    meleeDamage = 20;
	projectileDamage = 15;
	groundSlamDamage = 20;
	beamHitRadius = 30;
	beamDamage = 25;

	spawnedShockwaves = 0;
	groundSlamInterval = 0.5f;
	isGroundSlamSequence = false;
}

// Called when the game starts or when spawned
void ABaseBoss::BeginPlay()
{
	Super::BeginPlay();

	playerReference = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	normalSize = GetActorScale3D();
	largeSize = normalSize * 3;

	battleBegun = false;
	
	if (maxHealth <= 0)
	{
		maxHealth = 5000;
	}

	health = maxHealth;
	
	if (runDistance <= 0)
	{
		runDistance = 1500;
	}

	if (growingTime <= 0)
	{
		growingTime = 5;
	}

	if (meleeHitRadius <= 50)
	{
		meleeHitRadius = 150;
	}

	if (meleeDistance <= 0)
	{
		meleeDistance = 350;
	}

	if (meleeDamage <= 0)
	{
		meleeDamage = 20;
	}

	if (projectileDamage <= 0)
	{
		projectileDamage = 15;
	}

	if (groundSlamDamage <= 0)
	{
		groundSlamDamage = 20;
	}

	if (beamHitRadius <= 0)
	{
		beamHitRadius = 30;
	}

	if (beamDamage <= 0)
	{
		beamDamage = 25;
	}

	if (groundSlamInterval <= 0)
	{
		groundSlamInterval = 0.5f;
	}

	if (damageReductionPercentage >= 50)
	{
		damageReductionPercentage = 15;
	}

	damageReductionMultiple = damageReductionPercentage / 100;
}

// Called every frame
void ABaseBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Changing size
	HandleBossSizeChange();

	//Check hit detection on Melee
	if (isMeleeAttacking)
	{
		DetectMeleeHits();
	}

	//Handle beam attack
	if (isBeamAttacking)
	{
		//BeamAttackEffects(DeltaTime);
		BeamAttack(DeltaTime);
	}

	//Handle multi ground slam attack
	if (isGroundSlamSequence)
	{
		if (spawnedShockwaves >= bossPhase)
		{
			spawnedShockwaves = 0;
			isGroundSlamSequence = false;
			isAttacking = false;
		}
		
		if (GetWorld()->GetTimeSeconds() >= timeLastShockwave)
		{
			GroundSlamAttack();
			++spawnedShockwaves;
			timeLastShockwave = GetWorld()->GetTimeSeconds() + groundSlamInterval;
		}
	}
}

//Function called tick function to handle the size changing sequence
void ABaseBoss::HandleBossSizeChange()
{
	//If state is neutral, or the volume reference to spawn statues isn't found, do nothing
	if (changeSizeState == Neutral)
		return;
	
	//Get the wanted size based on the change size state
	FVector wantedSize = largeSize;
	FVector currentSize = normalSize;
	switch (changeSizeState)
	{
	case Growing:
		wantedSize = largeSize;
		currentSize = normalSize;
		isLarge = true;
		break;

	case Shrinking:
		wantedSize = normalSize;
		currentSize = largeSize;
		break;

	default:
		return;
	}

	//Apply lerp to linearly change the size, then apply it to the scale
	float alpha = timeBeganChangeSize - GetWorld()->GetTimeSeconds();
	alpha = (growingTime - alpha) / growingTime;
	FVector changeScale = FMath::Lerp(currentSize, wantedSize, alpha);
	SetActorScale3D(changeScale);

	//Get size difference and apply difference to speed
	float sizeDifference = GetActorScale3D().X / normalSize.X;
	GetCharacterMovement()->MaxWalkSpeed = walkSpeed * sizeDifference;

	//Once the growing time has elapsed, revert to neutral state
	if (GetWorld()->GetTimeSeconds() >= timeBeganChangeSize)
	{
		if (changeSizeState == Shrinking)
		{
			isLarge = false;
			timeLastGrowSize = GetWorld()->GetTimeSeconds() + growSizeCoolDown;
		}
		
		changeSizeState = Neutral;
	}
}

//Begins the beam attack sequence
void ABaseBoss::BeginBeamAttack()
{
	beamTarget = GetActorForwardVector();
	isBeamAttacking = true;

	beamParticles = SpawnBeamParticles();
	
	timeBeganBeamAttack = GetWorld()->GetTimeSeconds() + beamChargingTime;
}

/**
* Performs the beam attack calculations updating the beam's target point, and slowly
* aiming towards the player.
* Handles hit detection, and damages on a tick
*/
void ABaseBoss::BeamAttack(float deltaTime)
{
	//Lerp the beam target towards the player
	if (playerReference == nullptr)
	{
		playerReference = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	}
	
	if (ensure(playerReference))
	{
		beamTarget = FMath::Lerp(beamTarget, playerReference->GetActorLocation(), deltaTime * beamSpeed);
	}

	beamEnd = (beamTarget - GetMesh()->GetSocketLocation(throwningSocketName)) * beamRange;
	beamEnd = beamEnd + GetMesh()->GetSocketLocation(throwningSocketName);

	if (GetWorld()->GetTimeSeconds() < timeBeganBeamAttack)
	{
		return;
	}

	//Update the end of the beam particles
	beamParticles->SetBeamTargetPoint(0, beamEnd, 0);

	//Trace to detect player hit
	TArray<FHitResult> hitResults;
	bool sphereTraceHit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), GetMesh()->GetSocketLocation(throwningSocketName), beamEnd,
		beamHitRadius, UEngineTypes::ConvertToTraceType(ECC_Camera), false, hitActors, EDrawDebugTrace::None, hitResults, true);

	if (sphereTraceHit)
	{
		for (FHitResult hitResult : hitResults)
		{
			if (hitResult.Actor == playerReference)
			{
				FDamageEvent damageEvent;
				hitResult.Actor->TakeDamage(beamDamage, damageEvent, GetController(), this);
				hitActors.AddUnique(hitResult.Actor.Get());
				timeLastDamaged = GetWorld()->GetTimeSeconds() + beamDamageInterval;
				break;
			}
		}
	}

	//Clear all hit actors 
	if (GetWorld()->GetTimeSeconds() >= timeLastDamaged)
	{
		hitActors.Empty();
	}
}

//Initiates the melee attack detection
void ABaseBoss::BeginMeleeDamage()
{
	hitActors.Empty();
	isMeleeAttacking = true;
}

/**
* Detects hits with a sphere cast at a point in the boss' attacking arm at a set radius
* and stores hit actors in a TArray once they've been damaged
*/
void ABaseBoss::DetectMeleeHits()
{
	//Trace to detect player hit
	TArray<FHitResult> hitResults;
	bool sphereTraceHit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(),
		GetMesh()->GetSocketLocation(TEXT("LeftForeArmSocket")),
		GetMesh()->GetSocketLocation(TEXT("LeftForeArmSocket")), meleeHitRadius,
		UEngineTypes::ConvertToTraceType(ECC_Camera), false,
		hitActors, EDrawDebugTrace::None, hitResults, true);

	//Verify player reference
	if (playerReference == nullptr)
	{
		playerReference = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	}

	if (sphereTraceHit)
	{
		for (FHitResult hitResult : hitResults)
		{
			if (hitResult.Actor == playerReference)
			{
				FDamageEvent damageEvent;
				hitResult.Actor->TakeDamage(beamDamage, damageEvent, GetController(), this);
				hitActors.AddUnique(hitResult.Actor.Get());
				timeLastDamaged = GetWorld()->GetTimeSeconds() + beamDamageInterval;
				break;
			}
		}
	}
}

/**
* Gets a random number of projectiles to throw between 1 and a high end range of 3 - 8 depending on the boss phase
* Calculates the angles needed to throw multiple projectiles in a fan pattern,
* with the origin in the direction of the player
*/
void ABaseBoss::ThrowProjectiles()
{
	//Get number of projectiles
	int maxProjectiles = 3 + ((bossPhase - 1) * bossPhase);
	int numberOfProjectiles = FMath::RandRange(1, maxProjectiles);
	
	//Get the base direction to fire in
	if (playerReference == nullptr)
	{
		playerReference = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	}

	FVector playerPosition = GetActorForwardVector();
	
	if (ensure(playerReference))
	{
		playerPosition = playerReference->GetActorLocation();
	}
	FVector throwingPosition = GetMesh()->GetSocketLocation(throwningSocketName);
	FVector baseDirection = playerPosition - throwingPosition;
	
	//Get size of projectiles based on size state
	float size = (GetActorScale3D().X / largeSize.X) + 1;

	//If only throwing a single projectile, fire the single projectile straight at target
	if (numberOfProjectiles == 1)
	{
		//Create a Transform to spawn from
		FRotator rotation = UKismetMathLibrary::MakeRotFromX(baseDirection);
		FTransform spawnTransform(rotation, throwingPosition, FVector(size, size, size));

		//Call spawn effects on Blueprint
		AProjectileBase* projectile = Cast<AProjectileBase>(SpawnEffects(Projectile, spawnTransform));
		if (ensure(projectile))
		{
			projectile->SetDamage(projectileDamage);
			
			//Adjust projectile based on phase
			switch(bossPhase)
			{
				case 1:
					projectile->SetProjectileSpeed(2500);
					projectile->SetShouldBounce(false);
					break;
				case 2:
					projectile->SetProjectileSpeed(3500);
					projectile->SetShouldBounce(!isLarge);
					break;
				case 3:
					projectile->SetProjectileSpeed(5000);
					projectile->SetShouldBounce(!isLarge);
					break;
			}
		}
		isAttacking = false;
		return;
	}

	//Determine the angle increment between each projectile
	float baseFireAngle = 15 * size;
	float angleIncrement = numberOfProjectiles - 2;
	angleIncrement = (angleIncrement * 12.5f) + baseFireAngle;
	angleIncrement = angleIncrement / numberOfProjectiles;
	for (int i = 0; i < numberOfProjectiles; ++i)
	{
		//Get the angle for the current projectile
		float angle = (((numberOfProjectiles - 1) / 2.0f) - i) * angleIncrement;

		//Create a transform to spawn from
		FVector throwingDirection = GetThrowingDirection(baseDirection, angle);
		FRotator rotation = UKismetMathLibrary::MakeRotFromX(throwingDirection);
		FTransform spawnTransform(rotation, throwingPosition, FVector(size, size, size));

		//Call spawn effects on Blueprint
		AProjectileBase* projectile = Cast<AProjectileBase>(SpawnEffects(Projectile, spawnTransform));
		if (ensure(projectile))
		{
			projectile->SetDamage(projectileDamage);
			
			//Adjust projectile based on phase
			switch(bossPhase)
			{
			case 1:
				projectile->SetProjectileSpeed(2500);
				projectile->SetShouldBounce(false);
				break;
			case 2:
				projectile->SetProjectileSpeed(3500);
				projectile->SetShouldBounce(!isLarge);
				break;
			case 3:
				projectile->SetProjectileSpeed(5000);
				projectile->SetShouldBounce(!isLarge);
				break;
			}
		}
	}

	isAttacking = false;
}

/**
* Finds the ground position below the boss, and gets a position close to the ground to spawn shockwaves
* If the boss phase is greater than 1, a sequence is started where shockwaves are spawned for each phase
*/
void ABaseBoss::GroundSlamAttack()
{
	//Allow for multi groundslam attack
	if (bossPhase > 1 && !isGroundSlamSequence)
	{
		isGroundSlamSequence = true;
		return;
	}
	
	//Find position to spawn ground slam effect
	//Get position
	FVector position = GetActorLocation();
	
	//set to a specific height off the ground
	//Get trace to find ground position, then add desired height to get position
	float heightOffGround = 25;
	FVector end = position - FVector(0, 0, GetActorScale3D().Z + 1000);
	FCollisionQueryParams collisionParams(FName("GetGroundPosition"), false,GetOwner());
	FHitResult hitResult;
	if (GetWorld()->LineTraceSingleByChannel(hitResult, position, end, ECC_Visibility, collisionParams))
	{
		position.Z = hitResult.ImpactPoint.Z + heightOffGround;
	} else
	{
		//If trace does not hit anything, take actor's location and lower by an amount
		position = position - FVector(0, 0, 150);
	}

	//Create a Transform to spawn from
	FVector scale = FVector(5, 5, 1);
	FRotator rotation = FRotator::ZeroRotator;
	FTransform spawnTransform(rotation, position, scale);

	//Spawn ground slam effect
	SpawnEffects(GroundSlam, spawnTransform);

    if (!isGroundSlamSequence)
    {
	    isAttacking = false;
	}
}

//Called to clean up attack sequences
void ABaseBoss::OnEndingDamage()
{
	isAttacking = false;
	isBeamAttacking = false;
	isMeleeAttacking = false;

	//If beam attacking, destroy the particles
	if (beamParticles != nullptr)
	{
		beamParticles->DestroyComponent();
	}

	hitActors.Empty();
}

//Sets boss to growing state, and returns growing time
float ABaseBoss::BeginGrowingSize()
{
	//If the statue spawner doesn't have a reference to the volume to get random statue locations,
	//Cannot enter large state
	if (!statueSpawner->HasVolumeReference() || isLarge)
	{
		return 0;
	}

	//Growing cooldown
	if (GetWorld()->GetTimeSeconds() < timeLastGrowSize)
	{
		return 0;
	}

	//Boss cannot grow on 1st phase
	if (bossPhase == 1)
	{
		return 0;
	}
	
	changeSizeState = Growing;
	timeBeganChangeSize = GetWorld()->GetTimeSeconds() + growingTime;
	statueSpawner->SpawnStatues(bossPhase);
	return growingTime;
}

//Sets boss to shrinking state, and returns shrinking time
float ABaseBoss::BeginShrinkSize()
{
	changeSizeState = Shrinking;
	timeBeganChangeSize = GetWorld()->GetTimeSeconds() + growingTime;
	return growingTime;
}

/**
* Takes in a vector direction and angle and returns the rotated vector
*/
FVector ABaseBoss::GetThrowingDirection(FVector baseDirection, float angle)
{
	//Unreal trig math expects rads, so convert first
	float radAngle = FMath::DegreesToRadians(angle);
	float x = (FMath::Cos(radAngle) * baseDirection.X) - (FMath::Sin(radAngle) * baseDirection.Y);
	float y = (FMath::Sin(radAngle) * baseDirection.X) + (FMath::Cos(radAngle) * baseDirection.Y);
	
	return FVector(x, y, baseDirection.Z);
}

float ABaseBoss::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (isLarge)
	{
		return 0;
	}

	//Boss phase damage reduction
	float finalDamage = DamageAmount - (DamageAmount * (damageReductionMultiple * (bossPhase - 1)));
	
	//Decrement damage from health
	health -= finalDamage;

	//Update boss phase
	float bossPhaseThreshold = maxHealth / finalBossPhase;
	bossPhaseThreshold = (finalBossPhase - bossPhase) * bossPhaseThreshold;
	if (health <= bossPhaseThreshold)
	{
		++bossPhase;
		bossPhase = bossPhase > finalBossPhase ? finalBossPhase : bossPhase;
		BeginGrowingSize();
	}

	//If health depleted, die
	if (health <= 0)
	{
		health = 0;
		Die();
	} else
	{
		//Handle damage reactions
		PlayAnimationMontage();
	}
	
	return finalDamage;
}

/**
* Used to get a relative move speed for the animation blueprint
* Move speed is adjusted when boss enters large state, this returns the relative move speed without
* adjustments for the size state to ensure accurate locomotion animation
*/
float ABaseBoss::GetModifiedMoveSpeed()
{
	if (!isLarge)
	{
		return GetCharacterMovement()->Velocity.Size();
	}

	float currentVelocity = GetCharacterMovement()->Velocity.Size();
	float sizeDifference = GetActorScale3D().X / normalSize.X;

	return currentVelocity / sizeDifference;
}

/**
* Called to change move speeds between walking and running
* Boss cannot run while in the large state
*/
void ABaseBoss::SetMoveSpeed(bool running)
{
	if (!isLarge)
	{
		GetCharacterMovement()->MaxWalkSpeed = running ? runSpeed : walkSpeed;
	} 
}

//Begins the battle when the player enters the arena
void ABaseBoss::BeginBattle()
{
	battleBegun = true;
}

