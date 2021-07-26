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


//need to replicate spawning of the projectile
void AProjectile::ServerProjectileSetup_Implementation(const FVector& ShotDirection, const FVector& StartLocation, const FRotator& StartRotation)
{		
	UE_LOG(LogTemp, Warning, TEXT("%s ShotDirection"), *ShotDirection.ToString())
	//UE_LOG(LogTemp,Warning, TEXT("%s Velocity"), *ArrowProjectileMovement->ComputeAcceleration(FVector(1000, 1000, 100), 100).ToString())
	//need a value that basically makes the velocity less if the rotation is aiming down, more if it is aiming up

	

	//ArrowProjectileMovement->SetRelativeRotation(StartRotation);
	//instead of using the shotdirection maybe use the whole direction
	ArrowMesh->SetRelativeLocation(FVector(90,90, 100) * ShotDirection + StartLocation);
	//what we want is the pitch
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("rotation %s "), *StartRotation.ToString()));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("direction %s "), *(FVector(90,90,0) * ShotDirection).ToString()));

	// UE_LOG(LogTemp, Warning, TEXT("Rotation %s"), *StartRotation.ToString())
	// UE_LOG(LogTemp, Warning, TEXT("Direction %s"), *(FVector(90,90,0) * ShotDirection).ToString())
	// UE_LOG(LogTemp, Warning, TEXT("EndValue %s"), *(FVector(90,90,0) * ShotDirection + StartLocation).ToString())
	//Projectile needs to have been affected by the rotation

	//needs to start where the camera is
	// then it is an issue of either the rotation needing to be changed before this line occurs or this velocity being adjusted.

	//Make sure not to set up the rotation to fix this by using aim offsets as that'll just cause possible bugs later.


	//just create the camera in the character class in c++ so you can get a refrence to it and then delete the one in bp

	//So the camera is what dicatates where velocity is direct towards,
	ArrowProjectileMovement->Velocity = ShotDirection * ArrowProjectileMovement->InitialSpeed;
	
}

