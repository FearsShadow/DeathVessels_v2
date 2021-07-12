// Copyright Jonathan Bejarano, DeathVessels C++

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.h"
#include "Crossbow.generated.h"


UCLASS()
class DEATHVESSELS_API ACrossbow : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACrossbow();

	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void ProjectileSetup();

	TSubclassOf<AProjectile> ArrowClass;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* CrossBow;
	
	class AProjectile* Arrow;

	void ArrowCalculations();
	// void PullTrigger(class AMyCharacter* Character);
};
