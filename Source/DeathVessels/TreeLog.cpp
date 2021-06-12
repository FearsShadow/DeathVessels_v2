// Copyright Jonathan Bejarano, DeathVessels C++


#include "TreeLog.h"

// Sets default values
ATreeLog::ATreeLog()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Log = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Log"));
}

// Called when the game starts or when spawned
void ATreeLog::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATreeLog::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

