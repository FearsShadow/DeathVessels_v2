// Copyright Jonathan Bejarano, DeathVessels C++

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyCharacter.h"
#include "InventoryTest.generated.h"


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
	void ItemInfo(class AMyCharacter* Player, int32& Num);
	//class AMyCharacter* Player;
	UFUNCTION(BlueprintCallable)
	void DropItem(class AMyCharacter* Player);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Called every frame
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> AmmoModelClass;

	AActor* AmmoModel;
};