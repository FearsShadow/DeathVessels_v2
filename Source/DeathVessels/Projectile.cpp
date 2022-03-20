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

	//may need to use this for network smoothing reasons bInterpMovement
	ArrowProjectileMovement->InitialSpeed = 1000.0f;
    ArrowProjectileMovement->MaxSpeed = 1000.0f;
    ArrowProjectileMovement->bRotationFollowsVelocity = true;
    // ArrowProjectileMovement->bShouldBounce = true;
    // ArrowProjectileMovement->Bounciness = 0.3f;
    ArrowProjectileMovement->ProjectileGravityScale = 1.0f;

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void AProjectile::ServerProjectileSetup_Implementation(const FVector& ShotDirection, const FVector& StartLocation, const FRotator& StartRotation)
{		
	
	float SubTickTimeRemaining;
	UE_LOG(LogTemp, Warning, TEXT("%s ShotDirection"), *ShotDirection.ToString())
	ArrowMesh->SetRelativeLocation(FVector(90,90, 100) * ShotDirection + StartLocation);
	ArrowProjectileMovement->Velocity = ShotDirection * ArrowProjectileMovement->InitialSpeed;
	//float not working?
	ArrowProjectileMovement->HandleBlockingHit(ProjectileHit, ArrowProjectileMovement->PreviousHitTime, ArrowProjectileMovement->ComputeMoveDelta(ArrowProjectileMovement->Velocity,1), SubTickTimeRemaining);
	
}

