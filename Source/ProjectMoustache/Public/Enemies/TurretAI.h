// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TurretAI.generated.h"

UCLASS()
class PROJECTMOUSTACHE_API ATurretAI : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATurretAI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FVector SpawnLocation;
	FRotator SpawnRotation;

	bool CanFire;

	class UWorld* World;

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

	UPROPERTY(EditAnywhere)
		class UBoxComponent* DamageCollision;

	UPROPERTY(VisibleDefaultsOnly, Category = Enemy)
		class UAIPerceptionComponent* AIPerComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Enemy)
		class UAISenseConfig_Sight* SightConfig;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* HandMesh;

	UPROPERTY(VisibleAnywhere, Category = Mesh)
		class USceneComponent* ThrowLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector ThrowOffset;

	UFUNCTION()
		void OnSensed(const TArray<AActor*>& UpdatedActors);

	UPROPERTY(VisibleAnywhere, Category = Movement)
		FRotator TurretRotation;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AProjectile> Projectile;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Health = 100.0f;


	void SetNewRotation(FVector TargetPosition, FVector CurrentPosition);

	void Fire();

	void DealDamage(float DamageAmount);

	void CeaseFire();

	void ResetFire();




private:

	FTimerHandle CountDownTImerHandle = FTimerHandle();


};
