// Copyright Jonathan Bejarano, DeathVessels C++

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyCharacter.h"
#include "Inventory.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEATHVESSELS_API UInventoryTest : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryTest();

	UFUNCTION(BlueprintCallable)
	void AddAmmo(class AMyCharacter* Player);

	UFUNCTION(BlueprintCallable)
	void ItemAdjustment_BP(class AMyCharacter* Player, UStaticMeshComponent* ItemMesh, UInteractionComponent* InteractionComponent);

	UFUNCTION(BlueprintCallable)
	void CreateItemWidget(int32& ItemsInInventory);

	UFUNCTION(BlueprintCallable)
	void ItemInfo(class AMyCharacter* Player, int32& Amount, FString& Title, FString& Description);
	//class AMyCharacter* Player;
	UFUNCTION(BlueprintCallable)
	void DropItem(class AMyCharacter* Player, int32 AmountToRemove, FString ItemToDrop);

	UFUNCTION(BlueprintCallable)
	float AmountToRemove(class AMyCharacter* Player, float ValueToRemove, float Amount);

	UPROPERTY(BlueprintReadWrite)
	bool AddToInventory;

	UPROPERTY(EditAnywhere, Category = "Items")
	UStaticMesh* Ammo;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Called every frame
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> AmmoModelClass;

	bool ValueSet;

	TArray<FString> ItemName;
	TArray<FString> ItemDescription;
	TArray<int32> ItemNum;

	UFUNCTION(Server, Unreliable)
	void ServerDropItem(class AMyCharacter* Player, FVector ClientLocation, FRotator ClientRotation);

	AActor* ItemModel;

	FString ItemTypeName;

	int32 ItemIndex = 0;

	int32 ItemTypeIndex = 0;


};
