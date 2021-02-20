// Copyright Jonathan Bejarano, DeathVessels C++
#include "Hatchet.h"
#include "Tree.h"
#include "Kismet/KismetSystemLibrary.h"


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

// Called every frame
void AHatchet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}



void AHatchet::ServerSwing_Implementation(class AController* Pawn)
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
		
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), Location, End, FLinearColor::Red, 10, 20);
		bool AxeHit = GetWorld()->LineTraceSingleByChannel(OutHit, Location, End, ECC_GameTraceChannel3, TraceParams);
		
		if(AxeHit)
		{
			UE_LOG(LogTemp, Warning, TEXT("gzere"))
				//set a bool for it being se
				
				if(Tree == nullptr)
				{
					Tree = GetWorld()->SpawnActor<ATree>(TreeClass);
					Tree->SetActorLocation(OutHit.GetActor()->GetActorLocation());
					Tree->SetOwner(this);
					//error
					
					MulticastSwing(Pawn, OutHit.GetActor());
					
				}
				
				if(Tree != nullptr)
				{

					FPointDamageEvent DamageEvent(DamageAmount, OutHit, -Rotation.Vector(), nullptr);
					Tree->TakeDamage(DamageAmount, DamageEvent, Pawn, this);
					
					
					
				//store a value for the tree on the actual health as if you don't when some player switches health resets
				}
		}
	}
	
}

void AHatchet::MulticastSwing_Implementation(AController * Pawn, AActor* HitActor)
{
	if(HitActor != nullptr)
	{
		HitActor->Destroy();	
		//get health before
		
	}
		
}