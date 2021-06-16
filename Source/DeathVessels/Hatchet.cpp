// Copyright Jonathan Bejarano, DeathVessels C++
#include "Hatchet.h"
#include "Tree.h"
#include "Kismet/GameplayStatics.h"
#include "MyCharacter.h"

AHatchet::AHatchet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HatchetBody = CreateDefaultSubobject<UStaticMeshComponent>("Hatchet Body");
	SetRootComponent(HatchetBody);
}

// Called when the game starts or when spawned
void AHatchet::BeginPlay()
{
	Super::BeginPlay();

	MultiplayerTreeCheck = 0;
	
	
}


void AHatchet::ServerSwing_Implementation(class AController* Pawn, class AMyCharacter* Player)
{
	//multicast
	if(Pawn != nullptr)
	{
		
		FHitResult OutHit;
		FCollisionQueryParams TraceParams;
		Pawn->GetPlayerViewPoint(Location, Rotation);

		End = (Rotation.Vector() * AxeSwingDistance + Location);
	
		TraceParams.AddIgnoredActor(GetOwner());
		TraceParams.AddIgnoredActor(this);
		
		bool AxeHit = GetWorld()->LineTraceSingleByChannel(OutHit, Location, End, ECC_GameTraceChannel3, TraceParams);
		
		if(AxeHit)
		{
				
			if(OutHit.GetActor() != nullptr)
			{
				FPointDamageEvent DamageEvent(DamageAmount, OutHit, -Rotation.Vector(), nullptr);
				OutHit.GetActor()->TakeDamage(DamageAmount, DamageEvent, Pawn, this);
				
				Player->Wood +=10;
				UE_LOG(LogTemp, Warning, TEXT("%i wood"), Player->Wood)
				UE_LOG(LogTemp, Warning, TEXT("Arrived axe hit"))
				
			}	
		}
	}
	
}

