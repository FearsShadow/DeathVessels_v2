// Copyright Jonathan Bejarano, DeathVessels C++


#include "InventoryTest.h"


// Sets default values for this component's properties
UInventoryTest::UInventoryTest()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInventoryTest::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
// needs to be server side
void UInventoryTest::AddAmmo(class AMyCharacter* Player)
{
	if(Player)
    {
        Player->Ammo += 25;

    }
}

void UInventoryTest::ItemInfo(class AMyCharacter* Player, int32& CurrentAmmo)
{
	if(Player->Ammo > 0 && Player != nullptr)
	{
		CurrentAmmo = Player->Ammo;
	}
}

void UInventoryTest::DropItem(class AMyCharacter* Player)
{
	if(Player->Ammo > 0 && Player != nullptr)
	{
		AmmoModel = GetWorld()->SpawnActor<AActor>(AmmoModelClass, Player->GetActorLocation(), Player->GetActorRotation());
		Player->SetActorLocation(Player->GetActorLocation());
		
	
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player does not have ammo to drop"))
	}
}
