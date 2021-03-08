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
	

	if(Player)
    {
		//if it is unique then allow it
		//check index before to see if it stays the same if it stays the same return false so it doesn't make that widget
		ItemIndex = ItemName.Num(); 
		
		if( Player->Ammo > 0)
		{
			if(!ItemName.Contains("Ammo"))
			{
				ItemName.AddUnique("Ammo");
				ItemDescription.AddUnique("Ammo, goes very fast and hurts.");
				ItemNum.Add(Player->Ammo);

				//Add it to the array once then change the value from there if deleted add an exception
				// in the if statement if and make sure it is the one that was deleted and if so then it can readd player ammo to the array.
			}
			else
			{
				ItemNum[ItemName.Find("Ammo")] = Player->Ammo;
			}
		}
		if(Player->Wood > 0)
		{
			if(!ItemName.Contains("Wood"))
			{
				ItemName.AddUnique("Wood");
				ItemDescription.AddUnique("Pulled straight off of a tree or something. Would make a good wacking stick.");
				ItemNum.Add(Player->Wood);
			}
			else
			{
				ItemNum[ItemName.Find("Wood")] = Player->Wood;
			}
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
	if(ItemIndex == ItemName.Num())
	{
		ItemIndex = 0;
	}
	if(ItemIndex < ItemName.Num() && ValueSet)
	{
		Amount = ItemNum[ItemIndex];
		Title = ItemName[ItemIndex];
		Description = ItemDescription[ItemIndex];
		
		if(Amount == 0)
		{
			ItemNum.RemoveAt(ItemIndex);
			ItemName.RemoveAt(ItemIndex);
			ItemDescription.RemoveAt(ItemIndex);
			//set variable that is == to the itemname and that way you can check if you need to re-add
		}
		ItemIndex++;
	}
	//this is the issue
	

}

void UInventoryTest::DropItem(class AMyCharacter* Player, int32 AmountToRemove, FString ItemToDrop)
{
	//get index of whatever item you have then change the value in it in order to make the inventory accurate.
	ItemTypeName = "Null";
	if(AmountToRemove <= 0 && ValueSet){return;}
	UE_LOG(LogTemp, Warning, TEXT("brr"))

	//this basically allows it to subtract from whatever it want's should have a selected item only
	if(Player->Ammo - AmountToRemove  >= 0 && ItemToDrop == "Ammo")
	{
		
		ItemTypeName = ItemToDrop;
		Player->Ammo -= AmountToRemove;
		ItemNum[ItemName.Find("Ammo")] = Player->Ammo;
	}
	else if(Player->Wood - AmountToRemove >= 0 && ItemToDrop == "Wood")
	{
		
		ItemTypeName = ItemToDrop;
		Player->Wood -= AmountToRemove;
		ItemNum[ItemName.Find("Wood")] = Player->Wood;
		UE_LOG(LogTemp, Warning, TEXT("%i"), Player->Wood)
		
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
	if(ItemTypeName == "Ammo")
	{
		ItemMesh->SetStaticMesh(Ammo);
		InteractionComponent->SetInteractableNameText(FText::FromString("Ammo"));
	}

	ItemMesh->AddImpulse(FVector(0, 150, 0));
}