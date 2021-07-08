// Copyright Jonathan Bejarano, DeathVessels C++

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DEATHVESSELS_API IMyInterface
{
	GENERATED_BODY()


public:
	//can also be used as virtual void  override in another function besides this and can be stall called as an interface 
	void InteractPure();

	void AmmoCalculations(class AMyCharacter* Character, class AAssaultRifle* AR);

	void ReloadMagazine(class AMyCharacter* Character);

	
private:
	int32 BulletShot = 1;
	int32 ARAmmoForMag = 15;

};
