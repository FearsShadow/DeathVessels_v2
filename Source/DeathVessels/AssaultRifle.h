// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyCharacter.h"
#include "AssaultRifle.generated.h"



class UAmmoItem;
UCLASS()
class DEATHVESSELS_API AAssaultRifle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAssaultRifle();
	
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastBullet(FVector FlashLocation, FVector End, bool Hit);

	void Bullet();
	
	void AmmoCalculations(class AMyCharacter* Character);
protected:
	// Called when the game starts or when spawned
	void BeginPlay();

private:
	// Called every frame
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* GunMesh;

	UPROPERTY(EditAnywhere, Category = "FX")
	UParticleSystem * HitParticle;
	
	UPROPERTY(EditAnywhere, Category = "FX")
	UParticleSystem* MuzzleFlash;
	
	UPROPERTY(EditAnywhere, Category = "FX")
	USoundBase* ShotMiss;

	UPROPERTY(EditAnywhere, Category = "FX")
	USoundBase* ShotFired;

	UPROPERTY(EditAnywhere, Category = "FX")
	USoundBase* ShotHit;

	int32 BulletShot = 1;

	UPROPERTY(EditAnywhere, Category = "Bullet")
	int32 DamageAmount = 10;

	UPROPERTY(EditAnywhere, Category = "Bullet")
	float BulletTravelDistance = -1000;

	bool IsReloaded;

	FVector BulletEnd;

	FVector MuzzleFlashLocation;

	AActor* HitActor;

	

	bool AllowFiring;

	bool CharacterHit;
	
	UAmmoItem* AmmoUI;

	FHitResult OutHit;
	FCollisionQueryParams TraceParams;

	
};
