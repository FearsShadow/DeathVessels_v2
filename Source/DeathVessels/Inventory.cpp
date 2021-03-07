// Copyright Jonathan Bejarano, DeathVessels C++


#include "Inventory.h"
#include "InteractionComponent.h"


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
		if(Player->Ammo > 0)
		{
			ItemName.AddUnique("Ammo");
			ItemDescription.AddUnique("5.56 Ammo, goes very fast and hurts.");
			ItemNum.Add(Player->Ammo);
		}
		if(Player->Wood > 0)
		{
			ItemName.AddUnique("Wood");
			ItemDescription.AddUnique("Pulled straight off of a tree or something. Would make a good wacking stick.");
			ItemNum.Add(Player->Wood);
		}
		ValueSet = true;
		
    }
}

void UInventoryTest::CreateItemWidget(int32& ItemsInInventory)
{
	ItemsInInventory = ItemName.Num() - 1; //so you can create the widget that many times, index number that way you can get the amount and title for each
}

void UInventoryTest::ItemInfo(class AMyCharacter* Player, int32& Amount, FString& Title, FString& Description)
{
	//basically you could make an array, add in whatever item get in your inventory, then you have all the data of what is in the players inventory,
	// when it comes to the player removing something from their inventory what you would do is decrease the indexer.
	//(Which is just increasing the a value by one to get the correct index of what to add in next to the array.)

	//you just get the array, here and set the value from the array to either amount or title
	
	//I should be able to just use this
	if(ItemIndex == ItemName.Num())
	{
		ItemIndex = 0;
	}
	if(ItemIndex < ItemName.Num() && ValueSet)
	{
		Amount = ItemNum[ItemIndex];
		Title = ItemName[ItemIndex];
		Description = ItemDescription[ItemIndex];
		ItemIndex++;
	}
}

void UInventoryTest::DropItem(class AMyCharacter* Player, int32 AmountToRemove)
{
	//get index of whatever item you have then change the value in it in order to make the inventory accurate.
	ItemTypeName = "Null";
	if(AmountToRemove <= 0 && ValueSet){return;}

	if(Player->Ammo - AmountToRemove  >= 0)
	{
		
		ItemTypeName = "5.56 Ammo";
		Player->Ammo -= AmountToRemove;
		
		for(int32 i = 0; ItemName[i]  == "5.56 Ammo" && i < ItemName.Num(); i++)
		{
			ItemNum[i] = Player->Ammo;
			ItemTypeIndex = i;
			
		}
		if(ItemName[0] == "5.56 Ammo")
		{
			ItemNum[0] = Player->Wood;
		}
	}
	else if(Player->Wood - AmountToRemove >= 0 )
	{
		
		ItemTypeName = "Wood";
		Player->Wood -= AmountToRemove;

		for(int32 i = 0; ItemName[i]  == "Wood" && i < ItemName.Num() - 1; i++)
		{
			ItemNum[i] = Player->Wood;
			ItemTypeIndex = i;
		}
		if(ItemName[0] == "Wood")
		{
			ItemNum[0] = Player->Wood;
		}

		
		if(Player->Wood == 0 )
		{
			ItemNum.RemoveAt(ItemTypeIndex);
			ItemName.RemoveAt(ItemTypeIndex);
			ItemDescription.RemoveAt(ItemTypeIndex);
		}
	}

	if(ItemTypeName != "Null")	
	{
		ServerDropItem(Player, Player->GetActorLocation(), Player->GetActorRotation());
	}
	
	
}

void UInventoryTest::ServerDropItem_Implementation(class AMyCharacter* Player, FVector ClientLocation, FRotator ClientRotation)
{
	if(AmmoModelClass != nullptr)
	{

		ItemModel = GetWorld()->SpawnActor<AActor>(AmmoModelClass, ClientLocation, ClientRotation);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No model set for ammo"))
	}
}

float UInventoryTest::AmountToRemove(class AMyCharacter* Player, float ValueToRemove, float Amount)
{
	return Amount * ValueToRemove;
}

void UInventoryTest::ItemAdjustment_BP(class AMyCharacter* Player, UStaticMeshComponent* ItemMesh, UInteractionComponent* InteractionComponent)
{
	if(ItemMesh == nullptr || InteractionComponent == nullptr){return;}
	//can probably remove ammo and stuff here that way you are not dealing with it elsewhere
	if(ItemTypeName == "5.56 Ammo")
	{
		ItemMesh->SetStaticMesh(Ammo);
		InteractionComponent->SetInteractableNameText(FText::FromString("5.56 Ammo"));
	}
	ItemMesh->AddImpulse(FVector(0, 150, 0));
}