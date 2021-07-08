// Copyright Jonathan Bejarano, DeathVessels C++


#include "MyInterface.h"
#include "MyCharacter.h"
#include "AssaultRifle.h"

// Add default functionality here for any IMyInterface functions that are not pure virtual.

void IMyInterface::InteractPure()
{
    UE_LOG(LogTemp, Warning, TEXT("You got interfaces working! This is also located in Mycharacter Beginplay"))
}

void IMyInterface::AmmoCalculations(class AMyCharacter* Character, class AAssaultRifle* AR)
{	
	//APawn* PlayerPawn = Cast<APawn>(GetOwner());
	if(Character == nullptr || AR == nullptr ){UE_LOG(LogTemp, Warning, TEXT("nullptr in guninterface")) return;}
	//Basicaly should be looking to see if it is a assault rifle equipped or not
	if(Character->BulletsInMag > 0 ) 
    {	
		AR->Bullet();
		Character->BulletsInMag -= BulletShot;
    }
	// else if(PlayerPawn->IsPlayerControlled() == false )
	// {
	// 	AR->Bullet();
	// }
}

void IMyInterface::ReloadMagazine(class AMyCharacter* Character)
{	
	//when ammo left is bigger then bullets in mag
	for(;  Character->BulletsInMag < Character->Ammo;)
	{
		Character->Ammo = Character->Ammo - 1;
		Character->BulletsInMag = Character->BulletsInMag + 1;
	
	}
	
	//When bullets in mag is bigger then ammo left
	if (Character->Ammo <= ARAmmoForMag)
	{
		for(; Character->Ammo > 0 && Character->BulletsInMag < ARAmmoForMag ;)
		{
			Character->Ammo = Character->Ammo - 1;
			Character->BulletsInMag = Character->BulletsInMag + 1;
			
		}
	}
}
