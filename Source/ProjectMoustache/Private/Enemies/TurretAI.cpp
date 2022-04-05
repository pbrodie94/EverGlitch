// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemies/TurretAI.h"
#include "Components/BoxComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Player/PlayerBase.h"
//#include "Projectile.h"

// Sets default values
ATurretAI::ATurretAI()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DamageCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Damage Collision"));
	DamageCollision->SetupAttachment(RootComponent);

	AIPerComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));

	SightConfig->SightRadius = 1250.0f;
	SightConfig->LoseSightRadius = 1280.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->SetMaxAge(0.1f);

	AIPerComp->ConfigureSense(*SightConfig);
	AIPerComp->SetDominantSense(SightConfig->GetSenseImplementation());
	AIPerComp->OnPerceptionUpdated.AddDynamic(this, &ATurretAI::OnSensed);

	HandMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hand"));
	HandMesh->bCastDynamicShadow = false;
	HandMesh->CastShadow = false;
	HandMesh->SetupAttachment(RootComponent);

	ThrowLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Throw Location"));
	ThrowLocation->SetupAttachment(HandMesh);
	ThrowLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
}

// Called when the game starts or when spawned
void ATurretAI::BeginPlay()
{
	Super::BeginPlay();

	DamageCollision->OnComponentBeginOverlap.AddDynamic(this, &ATurretAI::OnHit);

	World = GetWorld();

	CanFire = true;

}

// Called every frame
void ATurretAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATurretAI::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ATurretAI::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
}

void ATurretAI::OnSensed(const TArray<AActor*>& UpdatedActors)
{
	for (int i = 0; i < UpdatedActors.Num(); i++)
	{
		FActorPerceptionBlueprintInfo Info;

		AIPerComp->GetActorsPerception(UpdatedActors[i], Info);

		if (Info.LastSensedStimuli[0].WasSuccessfullySensed())
		{
			FVector dir = UpdatedActors[i]->GetActorLocation() - GetActorLocation();
			dir.Z = 0.0f;

			SetNewRotation(UpdatedActors[i]->GetActorLocation(), GetActorLocation());

			Fire();

		}
	}

}

void ATurretAI::SetNewRotation(FVector TargetPosition, FVector CurrentPosition)
{
	FVector NewDirection = TargetPosition - CurrentPosition;
	NewDirection.Z = 0.0f;

	TurretRotation = NewDirection.Rotation();

	SetActorRotation(TurretRotation);
}

void ATurretAI::Fire()
{



	if (World != NULL)
	{

		if (CanFire)
		{

			SpawnRotation = GetControlRotation();

			SpawnLocation = ((ThrowLocation != nullptr) ? ThrowLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(ThrowOffset);

			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		//	World->SpawnActor<AProjectile>(Projectile, SpawnLocation, SpawnRotation, ActorSpawnParams);

			CeaseFire();

			//UE_LOG(LogTemp, Warning, TEXT("Output: %b"), MuzzleLocation != nullptr);
		}
	}

}

void ATurretAI::DealDamage(float DamageAmount)
{

	Health -= DamageAmount;

	if (Health <= 0.0f)
	{
		Destroy();
	}

}

void ATurretAI::CeaseFire()
{
	CanFire = false;
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ATurretAI::ResetFire, 1.0f);
}


void ATurretAI::ResetFire()
{
	CanFire = true;
}

