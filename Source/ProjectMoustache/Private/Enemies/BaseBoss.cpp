// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/BaseBoss.h"
#include "Enemies/EnemyComponents/StatueSpawner.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
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

	normalSize = GetActorScale();
	largeSize = normalSize * 3;

	meleeDistance = 350;
    meleeDamage = 20;
	projectileDamage = 15;
	groundSlamDamage = 20;
	beamDamage = 25;
}

// Called when the game starts or when spawned
void ABaseBoss::BeginPlay()
{
	Super::BeginPlay();

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

	if (beamDamage <= 0)
	{
		beamDamage = 25;
	}
}

// Called every frame
void ABaseBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Changing size
	//If state is neutral, or the volume reference to spawn statues isn't found, do nothing
	if (changeSizeState == Neutral)
		return;

	if (!statueSpawner->HasVolumeReference())
		return;

	//Get the wanted size based on the change size state
	FVector wantedSize = largeSize;
	switch (changeSizeState)
	{
		case Growing:
			wantedSize = largeSize;
			isLarge = true;
			break;

		case Shrinking:
			wantedSize = normalSize;
			break;

		default:
			return;
	}

	//Apply lerp to linearly change the size, then apply it to the scale
	float alpha = timeBeganChangeSize - GetWorld()->GetTimeSeconds();
	alpha = (growingTime - alpha) / growingTime;
	//alpha = alpha / 25.0f;
	FVector changeScale = FMath::Lerp(GetActorScale3D(), wantedSize, alpha);
	SetActorScale3D(changeScale);

	//Once the growing time has elapsed, revert to neutral state
	if (GetWorld()->GetTimeSeconds() >= timeBeganChangeSize)
	{
		if (changeSizeState == Shrinking)
		{
			isLarge = false;
		}
		
		changeSizeState = Neutral;
	}
}

// Called to bind functionality to input
void ABaseBoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseBoss::ThrowProjectiles()
{
	//Get number of projectiles
	int maxProjectiles = 3 + ((bossPhase - 1) * bossPhase);
	int numberOfProjectiles = FMath::RandRange(1, maxProjectiles);
	
	//Get the base direction to fire in
	FVector playerPosition = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation();
	FVector throwingPosition = GetMesh()->GetSocketLocation(throwningSocketName);
	FVector baseDirection = playerPosition - throwingPosition;

	//If only throwing a single projectile, fire the single projectile straight at target
	if (numberOfProjectiles == 1)
	{
		//Create a Transform to spawn from
		FRotator rotation = UKismetMathLibrary::MakeRotFromX(baseDirection);
		FTransform spawnTrasform(rotation, throwingPosition, FVector(1, 1, 1));

		//Call spawn effects on Blueprint
		SpawnEffects(Projectile, spawnTrasform);

		return;
	}

	//Determine the angle increment between each projectile
	float baseFireAngle = 10;
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
		FTransform spawnTransform(rotation, throwingPosition, FVector(1, 1, 1));

		//Call spawn effects on Blueprint
		SpawnEffects(Projectile, spawnTransform);
	}
}

void ABaseBoss::GroundSlamAttack()
{
	//Find position to spawn ground slam effect
	//Get position
	FVector position = GetActorLocation();
	
	//set to a specific height off the ground
	//Get trace to find ground position, then add desired height to get position
	float heightOffGround = 150;
	FVector end = position - FVector(0, 0, GetActorScale3D().Z);
	FCollisionQueryParams collisionParams(FName("GetGroundPosition"), false,GetOwner());
	FHitResult hitResult;
	if (GetWorld()->LineTraceSingleByChannel(hitResult, position, end, ECC_Visibility, collisionParams))
	{
		position = position + FVector(0, 0, hitResult.ImpactPoint.Z + heightOffGround);
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
}

void ABaseBoss::BeamAttack()
{
	
}

float ABaseBoss::BeginGrowingSize()
{
	changeSizeState = Growing;
	timeBeganChangeSize = GetWorld()->GetTimeSeconds() + growingTime;
	return growingTime;
}

float ABaseBoss::BeginShrinkSize()
{
	changeSizeState = Shrinking;
	timeBeganChangeSize = GetWorld()->GetTimeSeconds() + growingTime;
	return growingTime;
}

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
	//Decrement damage
	health -= DamageAmount;

	//Update boss phase
	float bossPhaseThreshold = maxHealth / finalBossPhase;
	bossPhaseThreshold = (finalBossPhase - bossPhase) * bossPhaseThreshold;
	if (health <= bossPhaseThreshold)
	{
		++bossPhase;
		bossPhase = bossPhase > finalBossPhase ? finalBossPhase : bossPhase;
	}

	//If health depleted, die
	if (health <= 0)
	{
		health = 0;
		Die();
	}
	
	return DamageAmount;
}

void ABaseBoss::SetMoveSpeed(bool running)
{
	GetCharacterMovement()->MaxWalkSpeed = running ? runSpeed : walkSpeed;
}

void ABaseBoss::BeginBattle()
{
	battleBegun = true;
}

