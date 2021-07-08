// Copyright Jonathan Bejarano, DeathVessels C++


#include "Crossbow.h"

// Sets default values
ACrossbow::ACrossbow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

// void AAssaultRifle::PullTrigger(class AMyCharacter* Character)
// {	
// 	AmmoCalculations(Character);
// }
