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

	//Set default variable values
	aimSensitivityX = 45.0f;
	aimSensitivityY = 45.0f;

	jumpHeight = 600.0f;
	airControl = 0.2f;
	dashPower = 1000;
	dashDelayInterval = 0.5f;

	meleeDamage = 15;

	//Don't rotate when the camera rotates
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = jumpHeight;
	GetCharacterMovement()->AirControl = airControl;

	//Create camera boom arm component, and set default values
	cameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoomArm"));
	cameraBoom->SetupAttachment(RootComponent);
	cameraBoom->TargetArmLength = 500.0f;
    cameraBoom->SetRelativeLocation(FVector(0, 0, 170));
    cameraBoom->bUsePawnControlRotation = true;

	//Create camera component, and set default values
    followCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	followCamera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName);
	followCamera->SetRelativeLocation(FVector(0, 50, 0));
	followCamera->bUsePawnControlRotation = false;
}

// Called when the game starts or when spawned
void APlayerBase::BeginPlay()
{
	Super::BeginPlay();

	if (meleeDamage <= 0)
	{
		meleeDamage = 15;
	}

	GetCharacterMovement()->JumpZVelocity = jumpHeight;
	GetCharacterMovement()->AirControl = airControl;
}

// Called every frame
void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	//Add controller action bindings
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &APlayerBase::Dash);
}

void APlayerBase::MoveForward(float value)
{
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

void APlayerBase::Dash()
{
	if (GetWorld()->GetTimeSeconds() < timeLastDashed)
	{
		return;
	}

	//Get direction to dash in, excluding upwards velocity
	FVector moveDirection = GetVelocity();
	moveDirection.Normalize();
	moveDirection.Z = 0;
	moveDirection *= dashPower;
	moveDirection.Z = 200;

	GetCharacterMovement()->Launch(moveDirection);

	timeLastDashed = GetWorld()->GetTimeSeconds() + dashDelayInterval;
}


void APlayerBase::DetectMeleeHits()
{
	//Temporarily use a point just in front of the player
	FVector detectionPoint = GetActorLocation() + (GetActorForwardVector() * 150);

	TArray<FHitResult> hitResults;

	bool sphereTraceHit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(),
		detectionPoint, detectionPoint, 100, UEngineTypes::ConvertToTraceType(ECC_Camera),
		false, hitActors, EDrawDebugTrace::None, hitResults, true);

	if (sphereTraceHit)
	{
		for (FHitResult hit : hitResults)
		{
			//Check if melee attack has hit an enemy
			if (hit.Actor->ActorHasTag("Enemy"))
			{
				FDamageEvent damageEvent;
				hit.Actor->TakeDamage(meleeDamage, damageEvent, GetController(), this);
				hitActors.AddUnique(hit.Actor.Get());
			}
		}
	}
}

float APlayerBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	health -= DamageAmount;

	GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(cameraShake, 1);

	if (health <= 0)
	{
		//Dead
		health = 0;
	}

	return DamageAmount;
}


void APlayerBase::BeginMeleeAttackDamage()
{
	hitActors.Empty();
	isMeleeAttacking = true;
}

void APlayerBase::EndMeleeAttackDamage()
{
	isMeleeAttacking = false;
	hitActors.Empty();
}
