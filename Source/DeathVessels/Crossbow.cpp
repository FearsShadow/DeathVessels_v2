// Copyright Jonathan Bejarano, DeathVessels C++


#include "Crossbow.h"

// Sets default values
ACrossbow::ACrossbow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CrossBow = CreateDefaultSubobject<UStaticMeshComponent>("CrossBow");
	SetRootComponent(CrossBow);

}

// Called when the game starts or when spawned
void ACrossbow::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACrossbow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACrossbow::ArrowCalculations(const FVector& ShotDirection, const FVector& StartLocation, const FRotator& StartRotation)
{
	
	Arrow = GetWorld()->SpawnActor<AProjectile>(ArrowClass);
	Arrow->ServerProjectileSetup(ShotDirection, StartLocation, StartRotation);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("server Arrow check")));
	UE_LOG(LogTemp, Warning, TEXT("Arrow spawn"))
}