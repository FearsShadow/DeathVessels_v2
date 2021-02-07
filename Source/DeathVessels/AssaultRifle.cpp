// Fill out your copyright notice in the Description page of Project Settings.

#include "AssaultRifle.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAssaultRifle::AAssaultRifle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	GunMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("GunMesh"));
	SetRootComponent(GunMesh);

	//Ammo = SetAmmo;
}

// Called when the game starts or when spawned
void AAssaultRifle::BeginPlay()
{	
	Super::BeginPlay();
	
}

void AAssaultRifle::PullTrigger(class AMyCharacter* Character)
{	
	AmmoCalculations(Character);
}

void AAssaultRifle::MulticastBullet_Implementation(FVector FlashLocation, FVector End, bool Hit)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, MuzzleFlashLocation);
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ShotFired, MuzzleFlashLocation);
	if(Hit)
	{
					//Possible Bug with Bullet_Miss as I set a limit to amount being played under the base sound then pressed edit and then under concurrency
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ShotHit, MuzzleFlashLocation);
	}
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, End, FRotator(0,0,0));
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ShotMiss, End);
}

void AAssaultRifle::Bullet()
{		
		if(HasAuthority())
		{
			MulticastBullet(MuzzleFlashLocation, OutHit.Location, CharacterHit);
		}
		APawn* OwnerPawn = Cast<APawn>(GetOwner());
		if(OwnerPawn != nullptr)
		{

			FVector Location;
			FRotator Rotation;
			if(OwnerPawn->GetController() != nullptr)
			{
				OwnerPawn->GetController()->GetPlayerViewPoint(Location, Rotation);
			}
			if(OwnerPawn->IsPlayerControlled() == false)
			{
				// find a way for this to show that the ai are missing in the players vision cone
				Location = Location + FVector(GetOwner()->GetActorForwardVector().X* FMath::RandRange(0, 175) , GetOwner()->GetActorForwardVector().Y * FMath::RandRange(0, 175), FMath::RandRange(-200, 150));
			}
			
			BulletEnd = (Location + Rotation.Vector() * BulletTravelDistance);
			
			TraceParams.AddIgnoredActor(GetOwner());
			
			CharacterHit = GetWorld()->LineTraceSingleByChannel(OutHit, Location, BulletEnd, ECC_Visibility, TraceParams);
			//
			MuzzleFlashLocation = ((this->GetActorForwardVector() * -40) + this->GetActorLocation());
			
			if(CharacterHit)
			{
				
				//The SpawnSoundAtLocation may be bugged if someone gets close
				HitActor = OutHit.GetActor();
				

				FPointDamageEvent DamageEvent(DamageAmount, OutHit, -Rotation.Vector(), nullptr);
				HitActor->TakeDamage(DamageAmount, DamageEvent, OwnerPawn->GetController(), this);
			}
			
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Ownerpawn Cast could not find an owner for the player"))
		}
		
}



void AAssaultRifle::AmmoCalculations(class AMyCharacter* Character)
{	
	APawn* PlayerPawn = Cast<APawn>(GetOwner());
	//check here
	if(Character->BulletsInMag > 0) 
    {	
		Bullet();
		Character->BulletsInMag -= BulletShot;
    }
	else if(PlayerPawn->IsPlayerControlled() == false)
	{
		Bullet();
	}
}


void AAssaultRifle::ReloadMagazine(class AMyCharacter* Character)
{	
	//when ammo left is bigger then bullets in mag
	for(;  Character->BulletsInMag < Character->Ammo;)
	{
		Character->Ammo = Character->Ammo - 1;
		Character->BulletsInMag = Character->BulletsInMag + 1;
	
	}
	
	//When bullets in mag is bigger then ammo left
	if (Character->Ammo <= AmmoForMag)
	{
		for(; Character->Ammo > 0 && Character->BulletsInMag < AmmoForMag ;)
		{
			Character->Ammo = Character->Ammo - 1;
			Character->BulletsInMag = Character->BulletsInMag + 1;

		}
	}
}



