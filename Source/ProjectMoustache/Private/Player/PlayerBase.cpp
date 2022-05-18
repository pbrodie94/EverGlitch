// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerBase.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Debug/ReporterBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interactables/Telekinesisable.h"

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

	// Init TK vars
	isUsingTK = false;
	isTKPushing = false;
	isTKPulling = false;
	pushPullSpeed = 500.0f;
	tkEnergyCost = 30.0f;
	tkAccuracy = 3.0f;
	tkViewAngle = 20.0f;
	minTKRange = 750.0f;
	maxTKRange = 3000.0f;
	tkPushForce = 100000000.0f;

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

	if (!isUsingTK)
	{
		DetectTKObjects();
	} else
	{
		// Adjust distance
		if (isTKPushing && targetDistance < maxTKRange)
		{
			targetDistance += (pushPullSpeed * DeltaTime);
		}

		if (isTKPulling && targetDistance > minTKRange)
		{
			targetDistance -= (pushPullSpeed * DeltaTime);
		}
		
		ManipulateTKObject(DeltaTime);
	}

	// Reset air dash counter when on the ground
	if (timesDashedInAir > 0)
	{
		if (!GetCharacterMovement()->IsFalling())
		{
			timesDashedInAir = 0;
		}
	}

	// Recharge energy
	if (abilityEnergy < maxAbilityEnergy && GetWorld()->GetTimeSeconds() > timeBeginRecharge && !isUsingTK)
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

	PlayerInputComponent->BindAction("Ability1", IE_Pressed, this, &APlayerBase::UseAbility1);
	PlayerInputComponent->BindAction("Ability1", IE_Released, this, &APlayerBase::StopPullingTKObject);
	PlayerInputComponent->BindAction("Ability2", IE_Pressed, this, &APlayerBase::UseAbility2);
	PlayerInputComponent->BindAction("Ability2", IE_Released, this, &APlayerBase::StopPushingTKObject);
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
		const FVector direction = FRotationMatrix(rotation).GetUnitAxis(EAxis::X);
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
		const FVector direction = FRotationMatrix(rotation).GetUnitAxis(EAxis::Y);
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
	if (isUsingTK)
	{
		// Push the object
		PushTKObject();

		return;
	}
	
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
			BeginCombatStance();
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
		BeginEndCombatStanceTimer();
	}
}

/**
* Used to detect nearby objects that can be picked up with the TK ability
* Gets a list of objects then highlights one that is within the tkViewAngle.
* If more than one object is within the tkViewAngle the one that is closest
* to the center will be chosen.
*/
void APlayerBase::DetectTKObjects()
{
	if (isUsingTK)
	{
		return;
	}

	const UWorld* world = GetWorld();
	TArray<AActor*> nearbyTKObjects;

	// Get TK objects in proximity
	// TODO: Find more efficient way of detecting TK objects - Sphere Collider or trace for custom context?
	TArray<FHitResult> hitResults;

	const bool sphereHit = UKismetSystemLibrary::SphereTraceMulti(world, GetActorLocation(),
		GetActorLocation(), maxTKRange,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false, nonTKObjects, EDrawDebugTrace::None, hitResults, true);

	if (sphereHit)
	{
		for (FHitResult result : hitResults)
		{
			if (result.GetActor() == nullptr)
			{
				continue;
			}
			
			// If not a TK object, add to ignore list so it is not detected again
			if (!result.GetActor()->GetClass()->ImplementsInterface(UTelekinesisable::StaticClass()))
			{
				nonTKObjects.Add(result.GetActor());
				continue;
			}

			nearbyTKObjects.Add(result.GetActor());
		}
	}

	if (nearbyTKObjects.Num() <= 0)
	{
		RemoveTKObject();
		return;
	}

	TArray<AActor*> tkObjectsInAngle;
	const FVector camPosition = followCamera->GetComponentLocation();
	const FVector camForward = followCamera->GetForwardVector();

	// Find the objects within angle of the forward direction of the camera
	for (AActor* obj : nearbyTKObjects)
	{
		FVector dir = obj->GetActorLocation() - camPosition;
		dir.Normalize();

		float angle = FMath::Acos(FVector::DotProduct(camForward, dir));
		angle = FMath::RadiansToDegrees(angle);

		if (angle <= tkViewAngle)
		{
			tkObjectsInAngle.Add(obj);
		}
	}

	if (tkObjectsInAngle.Num() <= 0)
	{
		RemoveTKObject();
		return;
	}

	// Check if objects are obstructed from view
	TArray<AActor*> objectsInView;

	for (AActor* obj : tkObjectsInAngle)
	{
		const FVector dir = obj->GetActorLocation() - camPosition;
		TArray<AActor*> ignoreActors;
		TArray<TEnumAsByte<EObjectTypeQuery>> queryObjects;
		queryObjects.Add(UEngineTypes::ConvertToObjectType(ECC_Visibility));

		FHitResult result;
		bool traceHit = UKismetSystemLibrary::LineTraceSingleForObjects(world, camPosition,
			obj->GetActorLocation(), queryObjects, false, ignoreActors,
			EDrawDebugTrace::None, result, true);

		if (traceHit)
		{
			if (result.GetActor() == obj)
			{
				objectsInView.Add(obj);
			}
		}
	}

	if (objectsInView.Num() <= 0)
	{
		RemoveTKObject();
		return;
	}

	// If only one object, set it and outline
	if (objectsInView.Num() == 1)
	{
		if (tkObject != tkObjectsInAngle[0])
		{
			RemoveTKObject();
		}
		
		tkObject = tkObjectsInAngle[0];
		ITelekinesisable::Execute_SetHighlightOutline(tkObject);
		return;
	}

	// Find the object closest to the center of the camera
	float smallestAngle = FLT_MAX;
	AActor* closestObject = nullptr;
	
	for (AActor* obj : objectsInView)
	{
		FVector dir = obj->GetActorLocation() - camPosition;
		dir.Normalize();

		float angle = FMath::Acos(FVector::DotProduct(camForward, dir));
		angle = FMath::RadiansToDegrees(angle);

		if (angle < smallestAngle)
		{
			smallestAngle = angle;
			closestObject = obj;
		}
	}

	if (closestObject == nullptr)
	{
		RemoveTKObject();
		return;
	}

	if (tkObject != closestObject)
	{
		RemoveTKObject();
	}
	
	tkObject = closestObject;
	ITelekinesisable::Execute_SetHighlightOutline(tkObject);
}

void APlayerBase::RemoveTKObject()
{
	if (tkObject == nullptr)
	{
		return;
	}

	if (!tkObject->GetClass()->ImplementsInterface(UTelekinesisable::StaticClass()))
	{
		tkObject = nullptr;
		return;
	}

	ITelekinesisable::Execute_RemoveHighlightOutline(tkObject);
	tkObject = nullptr;
}

void APlayerBase::ManipulateTKObject(float deltaTime)
{
	if (tkObject == nullptr)
	{
		return;
	}
	
	// Update the target position
	targetPoint = followCamera->GetComponentLocation() + (followCamera->GetForwardVector() * targetDistance);
	
	// Update the object's position
	const FVector smoothPosition = FMath::VInterpTo(tkObject->GetActorLocation(),
		targetPoint, deltaTime, tkAccuracy);
	tkObject->SetActorLocation(smoothPosition);
}

void APlayerBase::PickupTKObject()
{
	// Error check the object isn't null
	if (tkObject == nullptr)
	{
		return;
	}

	// Double check the object uses interface
	if (!tkObject->GetClass()->ImplementsInterface(UTelekinesisable::StaticClass()))
	{
		return;
	}

	if (abilityEnergy < tkEnergyCost)
	{
		return;
	}
	
	// Stop firing
	FireUp();

	BeginCombatStance();
	
	// Turn off the highlight on the object
	ITelekinesisable::Execute_RemoveHighlightOutline(tkObject);

	// Set the wanted target point to be the distance of the object, in a line from the front of the camera and the player
	targetDistance = FVector::Dist(GetActorLocation(), tkObject->GetActorLocation());
	if (targetDistance < minTKRange)
	{
		targetDistance = minTKRange;
	}

	ITelekinesisable::Execute_SetPickedUp(tkObject);

	isUsingTK = true;

	abilityEnergy -= tkEnergyCost;
}

void APlayerBase::DropTKObject()
{
	if (!isUsingTK)
	{
		return;
	}

	if (tkObject == nullptr)
	{
		return;
	}

	if (!tkObject->GetClass()->ImplementsInterface(UTelekinesisable::StaticClass()))
	{
		return;
	}

	ITelekinesisable::Execute_SetDropped(tkObject);

	timeBeginRecharge = GetWorld()->GetTimeSeconds() + energyRechargeDelay;
	isUsingTK = false;
	
	EndCombatStance();
}

void APlayerBase::PushTKObject()
{
	if (!isUsingTK)
	{
		return;
	}

	if (tkObject == nullptr)
	{
		return;
	}

	if (!tkObject->GetClass()->ImplementsInterface(UTelekinesisable::StaticClass()))
	{
		return;
	}
	
	DropTKObject();

	if (abilityEnergy < tkEnergyCost)
	{
		return;
	}

	// Apply force in the forward direction of the camera
	
	UShapeComponent* collider = Cast<UShapeComponent>
	(tkObject->GetComponentByClass(UShapeComponent::StaticClass()));
	if (collider != nullptr)
	{
		collider->AddForce(followCamera->GetForwardVector() * tkPushForce);
	}

	abilityEnergy -= tkEnergyCost;
}

void APlayerBase::StopPushingTKObject()
{
	isTKPushing = false;
}

void APlayerBase::StopPullingTKObject()
{
	isTKPulling = false;	
}

/**
* Used to cast magic support spell
*/
void APlayerBase::UseAbility1()
{
	if (isUsingTK)
	{
		isTKPushing = false;
		
		// Pull TK object in
		isTKPulling = true;
		return;
	}
	
	if (magicComponent == nullptr)
	{
		return;
	}

	//Switch to combat stance
	BeginCombatStance();
	BeginEndCombatStanceTimer();

	// Cast Spell
	magicComponent->CastSupportSpell();
}

/**
* Used to cast magic destruction spell
*/
void APlayerBase::UseAbility2()
{
	if (isUsingTK)
	{
		isTKPulling = false;
		
		// Push TK object out
		isTKPushing = true;

		return;
	}
	
	if (magicComponent == nullptr)
	{
		return;
	}

	//Switch to combat stance
	BeginCombatStance();
	BeginEndCombatStanceTimer();

	// Cast Spell
	magicComponent->CastDestructionSpell();
}

void APlayerBase::BeginCombatStance()
{
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetWorld()->GetTimerManager().ClearTimer(rangedCombatTimerHandle);
}

void APlayerBase::BeginEndCombatStanceTimer()
{
	GetWorld()->GetTimerManager().SetTimer(rangedCombatTimerHandle, this,
	&APlayerBase::EndCombatStance, combatStanceTime);
}

/**
* Expiry function for the combat stance timer
* Resets player to turn in direction of movement a set time after doing ranged attacks
*/
void APlayerBase::EndCombatStance() const
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
	const FVector detectionPoint = GetActorLocation() + (GetActorForwardVector() * 150);

	TArray<FHitResult> hitResults;

	const bool sphereTraceHit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(),
		detectionPoint, detectionPoint, 100,
		UEngineTypes::ConvertToTraceType(ECC_Camera),
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
float APlayerBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
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

float APlayerBase::TakeIncomingDamage_Implementation(float damageAmount, AActor* damageCauser,
	AController* eventInstigator, FDamageData damageData)
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
	
	const float damage = Super::TakeIncomingDamage_Implementation(damageAmount, damageCauser,
		eventInstigator, damageData);

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
	GetCharacterMovement()->MaxWalkSpeed = aimSpeed;

	//GetCharacterMovement()->bOrientRotationToMovement = false;
	BeginCombatStance();

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

	EndCombatStance();

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

/**
* If player has a current interactable object reference, interact with it
*/
void APlayerBase::InteractWithObject()
{
	if (currentInteractableObject != nullptr)
	{
		currentInteractableObject->Execute_Interact(currentInteractableObject.GetObject());
		return;
	}

	// Pickup and drop TK objects if no interactable
	if (tkObject != nullptr && !isUsingTK)
	{
		PickupTKObject();
	} else if (tkObject != nullptr && isUsingTK)
	{
		DropTKObject();
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
	const UWorld* world = GetWorld();

	if (duration <= 0)
	{
		world->GetTimerManager().ClearTimer(damageTimerHandle);
		return;
	}

	world->GetTimerManager().SetTimer(damageTimerHandle, this,
		&APlayerBase::OnDamageChangeExpired, duration);
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
	const UWorld* world = GetWorld();

	if (duration <= 0)
	{
		world->GetTimerManager().ClearTimer(speedTimerHandle);
		return;
	}

	world->GetTimerManager().SetTimer(speedTimerHandle, this,
		&APlayerBase::OnSpeedChangeExpired, duration);
}

/**
* Applies a change to the player's jump height
* Takes in the percentage of change from -100% to 100%
* Duration is the time in seconds the effect will last for, durations of 0 or less will be indefinite and must be
* undone by calling function again taking in (0, 0) as parameters.
*/
void APlayerBase::ApplyJumpChange_Implementation(float percentage, float duration)
{
	const float changePercentage = (FMath::Clamp(percentage, -99.0f, 200.0f) / 100) + 1;
	GetCharacterMovement()->JumpZVelocity = jumpHeight * changePercentage;

	//Handle duration
	UWorld* world = GetWorld();

	if (duration <= 0)
	{
		world->GetTimerManager().ClearTimer(jumpTimerHandle);
		return;
	}

	world->GetTimerManager().SetTimer(jumpTimerHandle, this,
		&APlayerBase::OnJumpChangeExpired, duration);
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

void APlayerBase::OnJumpChangeExpired()
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

void APlayerBase::ScreenDebugMessage(FString message, FColor displayColor, float displayTime)
{
	GEngine->AddOnScreenDebugMessage(-1, displayTime, displayColor, message);
}
