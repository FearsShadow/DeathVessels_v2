// Copyright Jonathan Bejarano, DeathVessels C++


#include "Inventory.h"
#include "InteractionComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UInventoryTest::UInventoryTest()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	

	// ...
}

void UInventoryTest::GetLifetimeReplicatedProps(TArray < FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UInventoryTest, ItemName);
	DOREPLIFETIME(UInventoryTest, ItemDescription);
	DOREPLIFETIME(UInventoryTest, ItemNum);
	DOREPLIFETIME(UInventoryTest, ValueSet);
}
// Called when the game starts
void UInventoryTest::BeginPlay()
{
	Super::BeginPlay();


	// ...
	
}

// Called every frame
// needs to be server side
void UInventoryTest::ServerAddAmmo_Implementation(class AMyCharacter* Player)
{
	if(Player)
    {
//needs to give this info to server for sure
		ItemIndex = ItemName.Num(); 

		if( Player->Ammo > 0)
		{
			if(!ItemName.Contains("Ammo"))
			{
				ItemName.AddUnique("Ammo");
				ItemDescription.AddUnique("Ammo, goes very fast and hurts.");
				ItemNum.Add(Player->Ammo);

			}
			else
			{
				ItemNum[ItemName.Find("Ammo")] = Player->Ammo;
			}
			ValueSet = true;
		}
		//may be because it doesn't check if it is 0
		if(Player->Wood > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("in wood"))
			if(!ItemName.Contains("Wood"))
			{
				UE_LOG(LogTemp, Warning, TEXT("itemname is wood"))
				ItemName.AddUnique("Wood");
				ItemDescription.AddUnique("Pulled straight off of a tree or something. Would make a good wacking stick.");
				//probably need to make arrays on client side, 
				ItemNum.Add(Player->Wood);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT(" ItemNumIndex %i"), ItemNum.Num() - 1)
				ItemNum[ItemName.Find("Wood")] = Player->Wood;
				
				UE_LOG(LogTemp, Warning, TEXT("Wood in Add %i"), Player->Wood)
			}
			
			UE_LOG(LogTemp, Warning, TEXT(" ItemsNum %i"), ItemNum.Num())
			ValueSet = true;
		}
		
    }
}

void UInventoryTest::CreateItemWidget(int32& ItemsInInventory)
{
	ItemsInInventory = ItemName.Num(); 
	UE_LOG(LogTemp, Warning, TEXT(" Items%i"), ItemsInInventory )
	 //so you can create the widget that many times, index number that way you can get the amount and title for each
}

void UInventoryTest::ItemInfo(class AMyCharacter* Player, int32& Amount, FString& Title, FString& Description)
{
	if(ItemIndex == ItemName.Num())
	{
		ItemIndex = 0;
	}
	if(ItemIndex < ItemName.Num() && ValueSet)
	{
		// this stuff could run on the server, make a variable that is replicated in order to store the data and pass it to the client, the client would then be able to pass it into bp
		Amount = ItemNum[ItemIndex];
		Title = ItemName[ItemIndex];
		Description = ItemDescription[ItemIndex];
		
		if(Amount == 0)
		{
			//must be apart of the server
			if(!Player->HasAuthority())
			{
				ServerArrayReduction(ItemIndex);
			}
			
			//set variable that is == to the itemname and that way you can check if you need to re-add
		}
		ItemIndex++;
	}
	//this is the issue
	

}

void UInventoryTest::ServerDropItem_Implementation(class AMyCharacter* Player, int32 AmountToRemove, const FString& ItemToDrop)
{
	//needs to for sure go on the server.
	
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
		UE_LOG(LogTemp, Warning, TEXT("Wood in drop %i"), Player->Wood)
		
	}

	if(ItemTypeName != "Null")	
	{
		ItemModel = GetWorld()->SpawnActor<AActor>(AmmoModelClass, Player->GetActorLocation(), Player->GetActorRotation());
		UE_LOG(LogTemp, Warning, TEXT("Wood in Serverdrop %i"), Player->Wood)
	}
	
	
}


float UInventoryTest::AmountToRemove(class AMyCharacter* Player, float ValueToRemove, float Amount)
{
	return Amount * ValueToRemove;
}

void UInventoryTest::ServerArrayReduction_Implementation(int32 Index)
{
	if(ItemNum.IsValidIndex(Index))
	{
		ItemNum.RemoveAt(Index);
		ItemName.RemoveAt(Index);
		ItemDescription.RemoveAt(Index);

	}
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