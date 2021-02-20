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
	void CreateItemWidget(int32& ItemsInInventory);

	UFUNCTION(BlueprintCallable)
	void ItemInfo(class AMyCharacter* Player, int32& Amount, FString& Title);
	//class AMyCharacter* Player;
	UFUNCTION(BlueprintCallable)
	void DropItem(class AMyCharacter* Player, int32 RemovingAmmo);

	UFUNCTION(BlueprintCallable)
	float AmmoToRemove(class AMyCharacter* Player, float ValueToRemove);

	UPROPERTY(BlueprintReadWrite)
	bool AddToInventory;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Called every frame
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> AmmoModelClass;

	bool ValueSet;


	TArray<FString> ItemName;
	TArray<int32> ItemNum;

	UFUNCTION(Server, Unreliable)
	void ServerDropItem(class AMyCharacter* Player, FVector ClientLocation, FRotator ClientRotation);

	AActor* AmmoModel;

	int32 ItemIndex = 0;

};
