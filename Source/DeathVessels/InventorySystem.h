//  Inventory system from reuban ward 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items.h"
#include "InventorySystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEATHVESSELS_API UInventorySystem : public UActorComponent
{
	GENERATED_BODY()

public:	

	UInventorySystem();
	
	//override ?
	bool AddItem(class UItems* Item);

	bool RemoveItems(class UItems* Item);

	UPROPERTY(EditDefaultsOnly, Instanced)
	TArray<class UItems*> DefaultItems;
	
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	int32 Capacity;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	TArray<class UItems*> Items;
	

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;


	virtual void BeginPlay() override;


protected:
	// Called when the game starts

private:

		
};
