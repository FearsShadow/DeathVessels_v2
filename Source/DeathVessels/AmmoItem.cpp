// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoItem.h"
#include "MyCharacter.h"

void UAmmoItem::Use(class AMyCharacter* Player)
{
    //may have  to be uammoitem
    if(Player)
    {
        Player->Ammo += AmmoToAdd;
        if(OwningInventory)
        {
            OwningInventory->RemoveItems(this);
        }   
    }
}

void UAmmoItem::AddMag()
{
    OwningInventory->AddItem(this);
}







