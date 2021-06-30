// Copyright Jonathan Bejarano, DeathVessels C++

#include "Tree.h"
#include "MyCharacter.h"
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



	Tree->SetSimulatePhysics(true);
	Tree->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Tree->SetPhysicsLinearVelocity(FVector(GetActorForwardVector().X * 600, GetActorForwardVector().Y * 600, 0));
	// Add in a particle system effect when treee is falling
	//Add in audio that acts as if the tree is falling over

	
	GetWorld()->GetTimerManager().SetTimer(TimerTreeDestory, this, &ATree::ServerCleanupTree, 1.7, false);
}

void ATree::ServerCleanupTree_Implementation()
{
	
	if(Tree != nullptr)
	{
		FVector TreeLocation = this->GetActorLocation();
		FRotator TreeRotation = this->GetActorRotation();
		FActorSpawnParameters SpawnInfo;

		this->Destroy();

		GetWorld()->SpawnActor<ATreeLog>(TreeLogClass, TreeLocation + FVector(FMath::RandRange(50, 150), FMath::RandRange(50, 150), 0), TreeRotation, SpawnInfo);
		GetWorld()->SpawnActor<ATreeLog>(TreeLogClass, TreeLocation + FVector(FMath::RandRange(50, 150), FMath::RandRange(50, 150), 0), TreeRotation, SpawnInfo);
		GetWorld()->SpawnActor<ATreeLog>(TreeLogClass, TreeLocation + FVector(FMath::RandRange(50, 150), FMath::RandRange(50, 150), 0), TreeRotation, SpawnInfo);

		//If interacted get a certain amount of wood

	}
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

		if(TreeHealth == 0)
		{
			MulticastTreePhysics();
		}
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