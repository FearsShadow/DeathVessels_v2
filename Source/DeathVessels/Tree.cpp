// Copyright Jonathan Bejarano, DeathVessels C++

#include "Tree.h"
#include "Hatchet.h"
#include "Net/UnrealNetwork.h"
//If I set up an inputcomponet here then I can call a function from hatchet that has info from this

// Sets default values
ATree::ATree()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>("Capsule Component");
	SetRootComponent(Capsule);


	//TreeBase =  CreateDefaultSubobject<UStaticMeshComponent>("Tree Base");
	
	
	
}


// Called when the game starts or when spawned
void ATree::BeginPlay()
{
	Super::BeginPlay();
	TreeHealth = 100;
	//ServerTreePhysics();
	
}

// Called every frame
void ATree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	


}


void ATree::ServerTreePhysics_Implementation()
{
	Capsule->SetSimulatePhysics(true);
	UE_LOG(LogTemp, Warning, TEXT("server"))
	MulticastTreePhysics();
}

void ATree::MulticastTreePhysics_Implementation()
{
	Capsule->SetSimulatePhysics(true);
	UE_LOG(LogTemp, Warning, TEXT("multi"))
}

float ATree::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
    //checks if hatchet is the one doing the damage
	const auto* IsHatchetDamaging = Cast<AHatchet>(DamageCauser);
	if(IsHatchetDamaging)
	{
		float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		DamageApplied = FMath::Min(TreeHealth, DamageApplied);
		TreeHealth -= DamageApplied;		
		
		UE_LOG(LogTemp, Warning, TEXT("%f"), TreeHealth)

		if(TreeHealth <= 0)
		{
			
			if(!HasAuthority())
			{
				ServerTreePhysics();
			}
			if(HasAuthority())
			{
				MulticastTreePhysics();
			}

		}//typically the amount of damage;
    	return DamageApplied;
	}
	else 
	{
		return 0;
	}
}

float ATree::GetTreeHealth()
{
	return TreeHealth;
}