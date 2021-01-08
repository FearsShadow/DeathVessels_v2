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
		
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), Location, End, FLinearColor::Red, 10, 20);
		bool AxeHit = GetWorld()->LineTraceSingleByChannel(OutHit, Location, End, ECC_Visibility, TraceParams);
		
		if(AxeHit)
		{
			// if(OutHit.GetActor() != nullptr)
			// {
				// if(i == 0)
				// {
				// 	Tree = GetWorld()->SpawnActor<ATree>(TreeClass);
				// 	Tree->SetActorLocation(OutHit.GetActor()->GetActorLocation());
				// 	Tree->SetOwner(this);
				// 	i = 1;
				// }

				FPointDamageEvent DamageEvent(DamageAmount, OutHit, -Rotation.Vector(), nullptr);
				
				if(OutHit.GetComponent()->GetCollisionObjectType() == ECC_GameTraceChannel3)
				{
					OutHit.GetActor()->Destroy();
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("zgg"))
				}
				
				//Tree->TakeDamage(DamageAmount, DamageEvent, Pawn, this);
				
	
				//UE_LOG(LogTemp, Warning, TEXT("%s"), *OutHit.GetActor()->GetName())
			//}
		}
	}
	
}

