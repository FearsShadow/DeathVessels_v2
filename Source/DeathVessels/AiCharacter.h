// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AiCharacter.generated.h"

class AMyCharacter;
UCLASS()
class DEATHVESSELS_API AAiCharacter : public AAIController
{
	GENERATED_BODY()

private:
	FVector StartLocation;

	UPROPERTY(EditAnywhere, Category = "BulletDelay")
	int32 AIReloadRate = 3;

	UPROPERTY(EditAnywhere, Category = "Pathfinding")
	int32 Radius = 500;
	
	FTimerHandle PatrolWaitDelay;
	
	bool AllowFiring = true;
	
	bool PatrolForward;
	bool PatrolSideways;
	bool AllowPatrol;
	bool GoBack;

	
	void AIFire();
	void Patrol();
	void PatrolLeft();
	void HeadToStart();
	
	void GetThePlayer();


protected:

public:
	AMyCharacter * ACharacter;

	int32 PlayerID;
	
	void Tick(float DeltaTime);

	virtual void BeginPlay() override;
};
