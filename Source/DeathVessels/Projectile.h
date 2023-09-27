// Copyright Jonathan Bejarano, DeathVessels C++

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectile.generated.h"

class UProjectileMovementComponent;

UCLASS()
class DEATHVESSELS_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();
	
	UFUNCTION(Server, unreliable)
	void ServerProjectileSetup(const FVector& ShotDirection, const FVector& StartLocation, const FRotator& StartRotation);

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* ArrowProjectileMovement;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Called every frame
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ArrowMesh;


	
	FHitResult ProjectileHit;
};
