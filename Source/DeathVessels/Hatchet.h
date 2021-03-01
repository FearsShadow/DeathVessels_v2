// Copyright Jonathan Bejarano, DeathVessels C++

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Hatchet.generated.h"

class ATree;
UCLASS()
class DEATHVESSELS_API AHatchet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHatchet();

	
	float MultiplayerTreeCheck;

	UFUNCTION(Server, Unreliable)
	void ServerSwing(AController * Pawn, class AMyCharacter* Player);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<ATree> TreeClass;
	
	FVector Location;
	FRotator Rotation;
	FVector End;

	ATree * Tree;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* HatchetBody;

	UPROPERTY(EditAnywhere,  Category = "HatchetSetup")
	float AxeSwingDistance = 470;
	
	UPROPERTY(EditAnywhere,  Category = "HatchetSetup")
	float DamageAmount = 10;

};