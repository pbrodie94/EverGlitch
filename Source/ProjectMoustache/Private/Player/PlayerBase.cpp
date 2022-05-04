// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerBase.h"
#include "Camera/CameraComponent.h"
#include "Debug/ReporterBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
APlayerBase::APlayerBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set default variable values
	aimSensitivityX = 45.0f;
	aimSensitivityY = 45.0f;

	jumpHeight = 600.0f;
	airControl = 0.2f;

	abilityEnergy = 100.0f;
	maxAbilityEnergy = 100.0f;
	energyRechargeDelay = 3.0f;
	energyRechargeRate = 30.0f;
	
	dashPower = 1500;
	dashEnergyCost = 25.0f;
	dashDelayInterval = 0.7f;
	numAirDashes = 1;
	timesDashedInAir = 0;

	combatStanceTime = 3;
	fireDelay = 0.5f;
	percentageDamageChange = 1;
	projectileDamage = 20;
	meleeDamage = 15;

	// Don't rotate when the camera rotates
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = jumpHeight;
	GetCharacterMovement()->AirControl = airControl;

	// Create camera boom arm component, and set default values
	cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoomArm"));
	cameraBoom->SetupAttachment(GetMesh());
	cameraBoom->TargetArmLength = 500.0f;
    cameraBoom->SetRelativeLocation(FVector(0, 0, 170));
    cameraBoom->bUsePawnControlRotation = true;

	// Create camera component, and set default values
    followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	followCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName);
	followCamera->SetRelativeLocation(FVector(0, 50, 0));
	followCamera->bUsePawnControlRotation = false;

	// Magic component
	magicComponent = CreateDefaultSubobject<UMagicComponent>(TEXT("MagicComponent"));

	// Combat manager
	combatManager = CreateDefaultSubobject<UCombatManagerComponent>(TEXT("CombatManager"));

	/*// Create inventory component
	inventoryComponent = CreateDefaultSubobject<UInventoryComponentBase>(TEXT("InventoryComponent"));*/

	hasControl = true;
	
	godMode = false;

	fireEffectiveness = 100;
	iceEffectiveness = 100;
	lightningEffectiveness = 100;
	waterEffectiveness = 100;
}

// Called when the game starts or when spawned
void APlayerBase::BeginPlay()
{
	Super::BeginPlay();

	if (meleeDamage <= 0)
	{
		meleeDamage = 15;
	}

	if (projectileDamage <= 0)
	{
		projectileDamage = 20;
	}

	if (aimSensitivityX <= 0)
	{
		aimSensitivityX = 45;
	}

	if (aimSensitivityY <= 0)
	{
		aimSensitivityY = 45;
	}

	if (jumpHeight <= 0)
	{
		jumpHeight = 600;
	}

	if (maxAbilityEnergy <= 0)
	{
		maxAbilityEnergy = 100.0f;
	}

	abilityEnergy = maxAbilityEnergy;

	if (energyRechargeDelay <= 0)
	{
		energyRechargeDelay = 3.0f;
	}

	if (energyRechargeRate < 1)
	{
		energyRechargeRate = 30.0f;
	}

	if (dashPower <= 0)
	{
		dashPower = 1500;
	}

	if (dashEnergyCost < 0)
	{
		dashEnergyCost = 25.0f;
	}

	if (combatStanceTime <= 0)
	{
		combatStanceTime = 3;
	}

	//Sets player's default values for reference
	runSpeed = GetCharacterMovement()->MaxWalkSpeed;
	if (aimSpeed <= 0)
	{
		aimSpeed = runSpeed / 2;
	}
	GetCharacterMovement()->JumpZVelocity = jumpHeight;
	GetCharacterMovement()->AirControl = airControl;
}

// Called every frame
void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Reset air dash counter when on the ground
	if (timesDashedInAir > 0)
	{
		if (!GetCharacterMovement()->IsFalling())
		{
			timesDashedInAir = 0;
		}
	}

	// Recharge energy
	if (abilityEnergy < maxAbilityEnergy && GetWorld()->GetTimeSeconds() > timeBeginRecharge)
	{
		abilityEnergy += (DeltaTime * energyRechargeRate);
	}

	// Detect melee hits when attacking
	if (isMeleeAttacking)
	{
		DetectMeleeHits();
	}

}

// Called to bind functionality to input
void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Add controller axis bindings
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerBase::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APlayerBase::LookX);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerBase::LookY);

	//Add controller action bindings
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerBase::BeginJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerBase::EndJump);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &APlayerBase::Dash);
	PlayerInputComponent->BindAction("Action", IE_Pressed, this, &APlayerBase::InteractWithObject);
	PlayerInputComponent->BindAction("Inventory", IE_Pressed, this, &APlayerBase::ToggleInventory);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &APlayerBase::BeginAiming);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &APlayerBase::EndAiming);
	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &APlayerBase::Fire);
	PlayerInputComponent->BindAction("Shoot", IE_Released, this, &APlayerBase::FireUp);
}

void APlayerBase::MoveForward(float value)
{
	if (GetIsDead() || !hasControl)
	{
		return;
	}

	if (Controller != nullptr && value != 0)
	{
		//Get forward direction
		FRotator rotation = Controller->GetControlRotation();
		rotation = FRotator(0, rotation.Yaw, 0);

		//Get movement direction
		FVector direction = FRotationMatrix(rotation).GetUnitAxis(EAxis::X);
		AddMovementInput(direction, value);
	}
}

void APlayerBase::MoveRight(float value)
{
	if (GetIsDead() || !hasControl)
	{
		return;
	}

	if (Controller != nullptr && value != 0)
	{
		//Get forward direction
		FRotator rotation = Controller->GetControlRotation();
		rotation = FRotator(0, rotation.Yaw, 0);

		//Get movement direction
		FVector direction = FRotationMatrix(rotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(direction, value);
	}
}

void APlayerBase::LookX(float value)
{
	if (!hasControl)
	{
		return;
	}

	AddControllerYawInput(value);
}

void APlayerBase::LookY(float value)
{
	if (!hasControl)
	{
		return;
	}

	AddControllerPitchInput(value);
}

void APlayerBase::BeginJump()
{
	if (GetIsDead() || !hasControl)
	{
		return;
	}

	Jump();
}

void APlayerBase::EndJump()
{
	if (GetIsDead() || !hasControl)
	{
		return;
	}

	StopJumping();
}

/**
* Takes the movement direction of the player, excludes the vertical direction
* then applies a dash force, as well as a slight upwards force to keep from getting stuck on floor
*/
void APlayerBase::Dash()
{
	if (GetIsDead() || !hasControl)
	{
		return;
	}

	if (abilityEnergy < dashEnergyCost)
	{
		return;
	}

	if (numAirDashes > -1 && GetCharacterMovement()->IsFalling() && timesDashedInAir >= numAirDashes)
	{
		return;
	}

	if (GetWorld()->GetTimeSeconds() < timeNextDash || isAiming)
	{
		return;
	}

	abilityEnergy -= dashEnergyCost;

	if (numAirDashes > -1 && GetCharacterMovement()->IsFalling())
	{
		++timesDashedInAir;
	}

	//Get direction to dash in, excluding upwards velocity
	FVector moveDirection = GetActorForwardVector();
	moveDirection.Normalize();
	moveDirection.Z = 0;
	moveDirection *= dashPower;
	moveDirection.Z = 200;

	GetCharacterMovement()->Launch(moveDirection);

	HandleDashEffects();

	const float worldTime = GetWorld()->GetTimeSeconds();

	timeNextDash = worldTime + dashDelayInterval;
	timeBeginRecharge = worldTime + energyRechargeDelay;
}

void APlayerBase::HandleDashEffects_Implementation()
{
}

/**
* Fire projectiles on main fire button.
* Will be changed to call a fire function on a weapon when created
*/
void APlayerBase::Fire()
{
	if (GetIsDead() || !hasControl)
	{
		return;
	}

	const UWorld* world = GetWorld();
	if (world->GetTimeSeconds() < timeNextShot)
	{
		return;
	}

	if (currentWeapon != nullptr)
	{
		if (currentWeapon->OnFireDown())
		{
			//Switch to combat stance
			GetCharacterMovement()->bOrientRotationToMovement = false;
			world->GetTimerManager().ClearTimer(rangedCombatTimerHandle);
		}
	}
}

void APlayerBase::FireUp()
{
	if (GetIsDead() || !hasControl)
	{
		return;
	}
	
	if (currentWeapon != nullptr)
	{
		currentWeapon->OnFireUp();
	}

	// Set timer to end combat stance
	if (!GetCharacterMovement()->bOrientRotationToMovement)
	{
		const UWorld* world = GetWorld();
		world->GetTimerManager().ClearTimer(rangedCombatTimerHandle);
		world->GetTimerManager().SetTimer(rangedCombatTimerHandle, this, &APlayerBase::OnCombatStanceEnd, combatStanceTime);
	}
}


/**
* Expiry function for the combat stance timer
* Resets player to turn in direction of movement a set time after doing ranged attacks
*/
void APlayerBase::OnCombatStanceEnd()
{
	if (isAiming)
	{
		return;
	}

	GetCharacterMovement()->bOrientRotationToMovement = true;
}

/**
* Hit detection for melee hits
* Will be moved to weapon class when created
*/
void APlayerBase::DetectMeleeHits()
{
	//Temporarily use a point just in front of the player
	FVector detectionPoint = GetActorLocation() + (GetActorForwardVector() * 150);

	TArray<FHitResult> hitResults;

	const bool sphereTraceHit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(),
		detectionPoint, detectionPoint, 100, UEngineTypes::ConvertToTraceType(ECC_Camera),
		false, hitActors, EDrawDebugTrace::None, hitResults, true);

	if (sphereTraceHit)
	{
		for (FHitResult hit : hitResults)
		{
			if (hit.Actor == nullptr)
			{
				continue;
			}

			//Check if melee attack has hit an enemy
			if (hit.Actor->ActorHasTag("Enemy") && !hitActors.Contains(hit.Actor))
			{
				hitActors.AddUnique(hit.Actor.Get());
				FDamageEvent damageEvent;
				hit.Actor->TakeDamage(meleeDamage, damageEvent, GetController(), this);
			}
		}
	}
}


//Takes in damage, and returns the actual damage
float APlayerBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//Take no damage if god mode is enabled
	if (godMode)
	{
		return 0;
	}

	if (GetIsDead())
	{
		return 0;
	}

	const float damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (damage > 0)
	{
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(cameraShake, 1);
	}

	return damage;
}

float APlayerBase::TakeIncomingDamage_Implementation(float damageAmount, AActor* damageCauser, AController* eventInstigator, FDamageData damageData)
{
	//Take no damage if god mode is enabled
	if (godMode)
	{
		return 0;
	}

	if (GetIsDead())
	{
		return 0;
	}
	
	const float damage = Super::TakeIncomingDamage_Implementation(damageAmount, damageCauser, eventInstigator, damageData);

	if (damage > 0)
	{
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(cameraShake, 1);
	}

	return damage;
}


//Begins melee hit detection
void APlayerBase::BeginMeleeAttackDamage()
{
	hitActors.Empty();
	isMeleeAttacking = true;
}

//Ends melee hit detection
void APlayerBase::EndMeleeAttackDamage()
{
	isMeleeAttacking = false;
	hitActors.Empty();
}

void APlayerBase::BeginAiming_Implementation()
{
	if (GetIsDead())
	{
		return;
	}

	isAiming = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->MaxWalkSpeed = aimSpeed;

	// Call to observers if any exist
	if (observers.Num() > 0)
	{
		for (auto obs : observers)
		{
			if (obs != nullptr)
			{
				obs->Execute_OnPlayerBeginAiming(obs.GetObject());
			}
		}
	}
}

void APlayerBase::EndAiming_Implementation()
{
	isAiming = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->MaxWalkSpeed = runSpeed;

	// Call to observers if any exist
	if (observers.Num() > 0)
	{
		for (auto obs : observers)
		{
			if (obs != nullptr)
			{
				obs->Execute_OnPlayerEndAiming(obs.GetObject());
			}
		}
	}
}

void APlayerBase::Die_Implementation()
{
	//Reset all temporary values
	ApplyDamageChange(0, 0);
	ApplySpeedChange(0, 0);
	ApplyJumpChange(0, 0);

	EndMeleeAttackDamage();

	if (isAiming)
	{
		EndAiming();
	}

	//Remove any interactable objects
	if (currentInteractableObject != nullptr)
	{
		RemoveSelfAsInteractable(currentInteractableObject);
	}
}

void APlayerBase::ToggleInventory_Implementation()
{
	/*if (inventoryComponent != nullptr)
	{
		inventoryComponent->ToggleInventory();
	}*/
}


/**
* If player has a current interactable object reference, interact with it
*/
void APlayerBase::InteractWithObject()
{
	if (currentInteractableObject != nullptr)
	{
		currentInteractableObject->Execute_Interact(currentInteractableObject.GetObject());
	}
}

/**
* Used to add health to player, and returns false if health is full
* The absolute value will be used, so any negative values will become positive
* Use Apply Damage for health removal
*/
bool APlayerBase::ApplyHealth_Implementation(float amount)
{
	if (currentHealth >= maxHealth)
	{
		return false;
	}

	currentHealth += FMath::Abs(amount);
	if (currentHealth > maxHealth)
	{
		currentHealth = maxHealth;
	}

	return true;
}

/**
* Applies a damage change to all of player's attacks
* Takes in the percentage of change from -99% to 200%
* Duration is the time in seconds the effect will last for, durations of 0 or less will be indefinite and must be
* undone by calling function again taking in (0, 0) as parameters.
*/
void APlayerBase::ApplyDamageChange_Implementation(float percentage, float duration)
{
	//Apply percentage change
	percentageDamageChange = (FMath::Clamp(percentage, -99.0f, 200.0f) / 100) + 1;

	//Handle duration
	UWorld* world = GetWorld();

	if (duration <= 0)
	{
		world->GetTimerManager().ClearTimer(damageTimerHandle);
		return;
	}

	world->GetTimerManager().SetTimer(damageTimerHandle, this, &APlayerBase::OnDamageChangeExpired, duration);
}

/**
* Applies a speed change to player's movement speed
* Takes in the percentage of change from -99% to 100%
* Duration is the time in seconds the effect will last for, durations of 0 or less will be indefinite and must be
* undone by calling function again taking in (0, 0) as parameters.
*/
void APlayerBase::ApplySpeedChange_Implementation(float percentage, float duration)
{
	float changePercentage = (FMath::Clamp(percentage, -99.0f, 100.0f) / 100) + 1;
	GetCharacterMovement()->MaxWalkSpeed = runSpeed * changePercentage;

	//Handle duration
	UWorld* world = GetWorld();

	if (duration <= 0)
	{
		world->GetTimerManager().ClearTimer(speedTimerHandle);
		return;
	}

	world->GetTimerManager().SetTimer(speedTimerHandle, this, &APlayerBase::OnSpeedChangeExpired, duration);
}

/**
* Applies a change to the player's jump height
* Takes in the percentage of change from -100% to 100%
* Duration is the time in seconds the effect will last for, durations of 0 or less will be indefinite and must be
* undone by calling function again taking in (0, 0) as parameters.
*/
void APlayerBase::ApplyJumpChange_Implementation(float percentage, float duration)
{
	float changePercentage = (FMath::Clamp(percentage, -99.0f, 200.0f) / 100) + 1;
	GetCharacterMovement()->JumpZVelocity = jumpHeight * changePercentage;

	//Handle duration
	UWorld* world = GetWorld();

	if (duration <= 0)
	{
		world->GetTimerManager().ClearTimer(jumpTimerHandle);
		return;
	}

	world->GetTimerManager().SetTimer(jumpTimerHandle, this, &APlayerBase::OnJumChangeExpired, duration);
}

/**
* Takes in an interactable object, and sets it as the current interactable object
*/
void APlayerBase::SetInteractable_Implementation(const TScriptInterface<IInteractable>& interactableObject)
{
	currentInteractableObject.SetInterface(interactableObject.GetObject());
	currentInteractableObject.SetObject(interactableObject.GetObject());
}

/**
* Takes in an interactable reference, and removes it from the player's interactable pointer
* if the player currently has a pointer to the calling interactable object
*/
void APlayerBase::RemoveSelfAsInteractable_Implementation(const TScriptInterface<IInteractable>& interactableReference)
{
	if (currentInteractableObject.GetObject() != interactableReference.GetObject())
	{
		return;
	}

	currentInteractableObject.SetInterface(nullptr);
	currentInteractableObject.SetObject(nullptr);
}

/**
* Returns whether or not the player currently has a reference to an interactable object
*/
bool APlayerBase::GetHasInteractable_Implementation()
{
	return currentInteractableObject != nullptr;
}

/**
* Returns the unique interactable message for each interactable
*/
FText APlayerBase::GetInteractableMessage_Implementation()
{
	return currentInteractableObject->Execute_GetInteractableMessage(currentInteractableObject.GetObject());
}

/**
* Returns the camera's current location
*/
FVector APlayerBase::GetCameraLocation_Implementation()
{
	return followCamera->GetComponentLocation();
}

/**
* Expiry functions for stat change timers
* Resets the values to default once expired
*/
void APlayerBase::OnDamageChangeExpired()
{
	percentageDamageChange = 1;
	GetWorld()->GetTimerManager().ClearTimer(damageTimerHandle);
}

void APlayerBase::OnSpeedChangeExpired()
{
	GetCharacterMovement()->MaxWalkSpeed = runSpeed;
	GetWorld()->GetTimerManager().ClearTimer(speedTimerHandle);
}

void APlayerBase::OnJumChangeExpired()
{
	GetCharacterMovement()->JumpZVelocity = jumpHeight;
	GetWorld()->GetTimerManager().ClearTimer(jumpTimerHandle);
}

/**
* Returns player's current location
*/
FVector APlayerBase::GetPlayerLocation_Implementation()
{
	return GetActorLocation();
}

/**
* Returns player's current forward direction
*/
FVector APlayerBase::GetPlayerForwardDirection_Implementation()
{
	return GetActorForwardVector();
}

/**
* Returns player's current rotation
*/
FRotator APlayerBase::GetPlayerRotation_Implementation()
{
	return GetActorRotation();
}

/**
* Returns player's current velocity
*/
float APlayerBase::GetCurrentPlayerVelocity_Implementation()
{
	return GetCharacterMovement()->Velocity.Size();
}

/**
* Subscribes actors as a new player observer
* Must implement the Player Observer Interface
*/
void APlayerBase::SubscribeAsObserver_Implementation(const TScriptInterface<IPlayerObserver>& newObserver)
{
	observers.AddUnique(newObserver);
}

/**
* Unsubscribes an actor as a player observer
*/
void APlayerBase::UnSubscribePlayerObserver_Implementation(const TScriptInterface<IPlayerObserver>& oldObserver)
{
	if (!observers.Contains(oldObserver))
	{
		return;
	}

	observers.Remove(oldObserver);
}
