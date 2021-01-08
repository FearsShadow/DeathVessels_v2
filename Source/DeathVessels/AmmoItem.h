// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items.h"
#include "InventorySystem.h"
#include "AmmoItem.generated.h"

UCLASS()
class DEATHVESSELS_API UAmmoItem : public UItems
{
	GENERATED_BODY()

public:
    int32 AmmoToAdd = 25;
	
	void AddMag();
	
	virtual void Use(class AMyCharacter* Player) override;
protected:
	
private:

 	

	
};
