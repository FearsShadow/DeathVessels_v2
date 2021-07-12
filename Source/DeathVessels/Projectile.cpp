// Copyright Jonathan Bejarano, DeathVessels C++


#include "Projectile.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>("ArrowMesh");
	SetRootComponent(ArrowMesh);
	ArrowProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ArrowProjectileMovement");

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	ProjectileSetup();
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::ProjectileSetup()
{
	UE_LOG(LogTemp,Warning, TEXT("%s Velocity"), *ArrowProjectileMovement->ComputeVelocity(FVector(1000, 1000, 100), 2).ToString())
}

