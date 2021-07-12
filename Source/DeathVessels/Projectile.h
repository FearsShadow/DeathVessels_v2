// Copyright Jonathan Bejarano, DeathVessels C++

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectile.generated.h"

UCLASS()
class DEATHVESSELS_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	virtual void Tick(float DeltaTime) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Called every frame
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* ArrowMesh;

	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* ArrowProjectileMovement;

	void ProjectileSetup();
};
