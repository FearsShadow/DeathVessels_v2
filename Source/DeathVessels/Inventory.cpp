// Copyright Jonathan Bejarano, DeathVessels C++


#include "Inventory.h"


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
	//UE_LOG(LogTemp, Warning, TEXT("Howdy outside of conditional"))

	if(Player)
    {
		//if it is unique then allow it
		//check index before to see if it stays the same if it stays the same return false so it doesn't make that widget
		ItemIndex = ItemName.Num(); 
		if(Player->Ammo)
		{
			ItemName.AddUnique("Ammo");
			ItemNum.Add(Player->Ammo);
		}
		if(Player->Wood > 0)
		{
			ItemName.AddUnique("Wood");
			ItemNum.Add(Player->Wood);
		}
		ValueSet = true;
		
    }
}

void UInventoryTest::CreateItemWidget(int32& ItemsInInventory)
{
	ItemsInInventory = ItemName.Num() - 1; //so you can create the widget that many times, index number that way you can get the amount and title for each
}

void UInventoryTest::ItemInfo(class AMyCharacter* Player, int32& Amount, FString& Title)
{
	//basically you could make an array, add in whatever item get in your inventory, then you have all the data of what is in the players inventory,
	// when it comes to the player removing something from their inventory what you would do is decrease the indexer.
	//(Which is just increasing the a value by one to get the correct index of what to add in next to the array.)

	//you just get the array, here and set the value from the array to either amount or title
	if(ItemIndex == ItemName.Num())
	{
		ItemIndex = 0;
	}
	if(ItemIndex < ItemName.Num() && ValueSet)
	{
		Amount = ItemNum[ItemIndex];
		Title = ItemName[ItemIndex];
		ItemIndex++;
	}
}

void UInventoryTest::DropItem(class AMyCharacter* Player, int32 RemovingAmmo)
{
	if(Player->Ammo > 0 && Player != nullptr && !Player->HasAuthority() && Player->Ammo - RemovingAmmo  > 0 && RemovingAmmo > 0)
	{
		Player->Ammo -= RemovingAmmo;
		ServerDropItem(Player, Player->GetActorLocation(), Player->GetActorRotation());
		UE_LOG(LogTemp, Warning, TEXT("occured"))
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player does not have ammo to drop"))
	}
}

void UInventoryTest::ServerDropItem_Implementation(class AMyCharacter* Player, FVector ClientLocation, FRotator ClientRotation)
{
	AmmoModel = GetWorld()->SpawnActor<AActor>(AmmoModelClass, ClientLocation, ClientRotation);
}

float UInventoryTest::AmmoToRemove(class AMyCharacter* Player, float ValueToRemove)
{
	return Player->Ammo * ValueToRemove;
}