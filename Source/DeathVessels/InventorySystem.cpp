// Inventory Code system from reuban ward 


#include "InventorySystem.h"
#include "Items.h"

// Sets default values for this component's properties
UInventorySystem::UInventorySystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	Capacity = 300;

	// ...
}

void UInventorySystem::BeginPlay()
{
	Super::BeginPlay();

	for(auto& Item : DefaultItems)
	{
		AddItem(Item);
	}

	
}

bool UInventorySystem::AddItem(class UItems* Item)
{
	if(Items.Num() >= Capacity || !Item)
	{
		return false;
	}

	Item->OwningInventory = this;
	Item->World = GetWorld();

	Items.Add(Item);
	OnInventoryUpdated.Broadcast();

	return true;
}

bool UInventorySystem::RemoveItems(class UItems* Item)
{
	if(Item)
	{
		Item->OwningInventory = nullptr;
		Item->World = nullptr;
		Items.RemoveSingle(Item);
		
		OnInventoryUpdated.Broadcast();
		return true;
	}
	return false;
}

