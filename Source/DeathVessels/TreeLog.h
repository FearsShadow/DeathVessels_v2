// Copyright Jonathan Bejarano, DeathVessels C++

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TreeLog.generated.h"

UCLASS()
class DEATHVESSELS_API ATreeLog : public AActor
{
	GENERATED_BODY()
	
private:
	// Sets default values for this actor's properties
	UPROPERTY(EditAnywhere)	
	UStaticMeshComponent* Log;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	ATreeLog();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	

};
