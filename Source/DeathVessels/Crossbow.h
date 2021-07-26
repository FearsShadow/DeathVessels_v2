// Copyright Jonathan Bejarano, DeathVessels C++

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.h"
#include "MyCharacter.h"
#include "Crossbow.generated.h"

class AMyCharacter;
UCLASS()
class DEATHVESSELS_API ACrossbow : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACrossbow();

	
	void ArrowCalculations(const FVector& ShotDirection, const FVector& StartLocation, const FRotator& StartRotation);
	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void ProjectileSetup();

	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> ArrowClass;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* CrossBow;

	class AProjectile* Arrow;

	// void PullTrigger(class AMyCharacter* Character);
};
