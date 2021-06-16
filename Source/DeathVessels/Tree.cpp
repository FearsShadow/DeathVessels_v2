// Copyright Jonathan Bejarano, DeathVessels C++

#include "Tree.h"
#include "Hatchet.h"
#include "TreeLog.h"
#include "Net/UnrealNetwork.h"
//If I set up an inputcomponet here then I can call a function from hatchet that has info from this

// Sets default values
ATree::ATree()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Tree = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tree"));
	SetRootComponent(Tree);
	
	TreeHealth = 100;
}


// Called when the game starts or when spawned
void ATree::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void ATree::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	


}



void ATree::MulticastTreePhysics_Implementation()
{

//So the plan is to give the player some wood for hitting the tree and then get most of it by picking it up once a tree breaks

	Tree->SetSimulatePhysics(true);
	Tree->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	GetWorld()->GetTimerManager().SetTimer(TimerTreeDestory, this, &ATree::ServerCleanupTree, 3, true);

	if(TreeLog == nullptr)
	{
		TreeLog = GetWorld()->SpawnActor<ATreeLog>(TreeLogClass);
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("treelog null"));  
	}
	//might need to run this on server
	
}

void ATree::ServerTreePhysics_Implementation()
{
	MulticastTreePhysics();
}

void ATree::ServerCleanupTree_Implementation()
{
	
	// if(Tree != nullptr)
	// {
	// 	Tree->Destroy();
	// }
}

float ATree::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
    //checks if hatchet is the one doing the damage
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("arrived take"));  
	const auto* IsHatchetDamaging = Cast<AHatchet>(DamageCauser);
	if(IsHatchetDamaging)
	{
		float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		DamageApplied = FMath::Min(TreeHealth, DamageApplied);
		TreeHealth -= DamageApplied;		
		
		UE_LOG(LogTemp, Warning, TEXT("%f"), TreeHealth)

		if(TreeHealth == 0)
		{
			
			

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