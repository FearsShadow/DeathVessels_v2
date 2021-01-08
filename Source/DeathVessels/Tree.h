// Copyright Jonathan Bejarano, DeathVessels C++

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"
#include "Tree.generated.h"

class AHatchet;
UCLASS()
class DEATHVESSELS_API ATree : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATree();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCapsuleComponent* Capsule;



	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:	
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastTreePhysics();

	UFUNCTION(Server, Unreliable)
	void ServerTreePhysics();

	UPROPERTY(EditAnywhere, Category = "Confing")
	float TreeHealth;

};
