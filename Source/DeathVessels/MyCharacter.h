// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Inventory.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "MyCharacter.generated.h"

USTRUCT()
struct FInteractionData
{
	GENERATED_BODY()

	FInteractionData()
	{
		ViewedInteractionComponent = nullptr;
		LastInteractionCheckTime = 0;
		bInteractionHeld = false;
	}

	UPROPERTY()
	class UInteractionComponent* ViewedInteractionComponent;

	UPROPERTY()
	float LastInteractionCheckTime;

	UPROPERTY()
	bool bInteractionHeld;
};

class AAssaultRifle;
class AHatchet;
class AFloor;
class UPhysicsHandleComponent;
UCLASS()
class DEATHVESSELS_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:	
//building
	UFUNCTION(BlueprintCallable)
	void BP_FindPlacementLocation(int32 BuildingPiece);

	UFUNCTION(BlueprintCallable)
	void BP_CleanupBuild();

	UFUNCTION(BlueprintCallable)
	void BP_BuildKit(int32 BuildingPiece);

	UFUNCTION(BlueprintCallable)
	void BP_BuildMenu(int32 BuildingPiece);

	UFUNCTION(BlueprintCallable)
	int32 BP_SwitchUp(int32 BuildingPiece);

	UFUNCTION(BlueprintCallable)
	int32 BP_SwitchDown(int32 BuildingPiece);


	AMyCharacter();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const &DamageEvent,class AController * EventInstigator, AActor * DamageCauser) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, Replicated)
	float WalkSpeed = 0.6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, Replicated)
	float YInput;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, Replicated)
	float XInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, Replicated)
	int32 Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config, Replicated)
	bool IsToolless = true;

	UPROPERTY(Replicated)
	bool IsAR = false;

	UPROPERTY(Replicated)
	bool IsHatchet = false;

	UPROPERTY(Replicated)
	AAssaultRifle* AR;
	
	UPROPERTY(Replicated)
	AHatchet* Hatchet;

	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	// UFUNCTION(BlueprintCallable)
	// bool IsJumping();
	
	UFUNCTION(BlueprintPure, Category = "Health")
	float HealthPercentage() const;
	
	UFUNCTION(BlueprintCallable)
	int32 DisplayAmmo();

	UFUNCTION(BlueprintCallable)
	int32 DisplayMagazineAmmo();

	UFUNCTION(BlueprintCallable)
	int32 FindPlayer();



	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAcess = "true"))
	class UInventoryTest* InventoryTest;
	
//inventory variables
	int32 Ammo = 0;
	int32 BulletsInMag;

	int32 Wood = 0;

//other

	UFUNCTION(Server, Reliable)
	void ServerLeftClick();

	void LeftClick();

	void Fire();

	UFUNCTION(BlueprintCallable)
	bool AnimationReload();

	FHitResult OutHit;
	FHitResult CubeHit;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AHatchet> HatchetClass;

	FVector LineTraceEnd;
//interaction

	bool IsInteracting() const;

	float GetRemainingInteractTime() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle FireRateDelay;
private:
//InputRelated
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(Server, Reliable)
	void ServerFire(int32 Bullets);
	
	UFUNCTION(Server, Unreliable)
	void ServerHealthPercentage() const;



	void YMovement(float AxisValue);
	void XMovement(float AxisValue);
	void Sprint();
	void SprintRelease();
	
	UFUNCTION(Server, Unreliable)
	void ServerYMovement(float AxisValue);

	UFUNCTION(Server, Unreliable)
	void ServerXMovement(float AxisValue);

	UFUNCTION(Server, Unreliable)
	void ServerSprint();

	UFUNCTION(Server, Unreliable)
	void ServerSprintRelease();

	void Reload();
	
	void StartJump();
	void JumpEndAnim();

	UFUNCTION(Server, Unreliable)
	void ServerWeapon1();

	UFUNCTION(Server, Unreliable)
	void ServerWeapon2();


	UFUNCTION(Server, Unreliable)
	void ServerGrab();

	UFUNCTION(Server, Unreliable)
	void ServerReleaseThrow();

	UFUNCTION(Server, Unreliable)
	void ServerReleaseDrop();

	void Grab();
	void ReleaseThrow();
	void ReleaseDrop();

//building system

	FVector Location;
	FRotator Rotation; 
	FVector End;

	int32 IndexOfShortest;
	TArray<FVector> BuildingTypes;

	TArray<FVector> RoofSnapLocation;
	TArray<FVector> FloorSnapLocation;
	TArray<FVector> WallSnapLocation;

	TArray<FVector> FloorSnapConversions;
	TArray<FVector>	WallSnapConversion;
	TArray<FVector> RoofSnapConversion;
	
	FVector RoofLocation;

	TArray<FVector> CubeSnapLocations;
	
	TArray<AActor*> FloorsArray;

	TArray<FVector> FloorsLocations;

	AActor* MainActor;
	AActor* ChildActors;

	UFUNCTION(Server, Unreliable)
	void ServerSwitchUp(int32 BuildingPiece);

	UFUNCTION(Server, Unreliable)
	void ServerSwitchDown(int32 BuildingPiece);

	UFUNCTION(Server, Reliable)
	void ServerCleanupBuild();

	UFUNCTION(Server, Reliable)
	void ServerFindPlacementLocation(FVector Client, FRotator ClientRotation, int32 BuildObjectNum, AActor* FloorActor , bool LandHit, int32 ShortestIndex, FVector RoofLoc);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastFindPlacementLocation(FVector Client, FRotator ClientRotation, int32 BuildObjectNum, int32 ShortestIndex, bool LandHit, FVector RoofLoc);


//other	
	float OutRadius;
	float OutHalfHeight;
	float IsCrouched = 1;	

	bool AlreadyPressed1 = false;
	bool AlreadyPressed2 = false;
	bool PlayerIsJumping;
	bool TurnOffCrouch;
	bool StartCrouch = true;

	
	
	bool AllowedPlacement = false;
	
	int32 ValueIncreaser = 1;
	int32 z = 0;

	int32 CubeIncreaser = 0;
	//
	FVector ClientLocation;
	FVector PlacementLocation;
	FRotator PlacementRotation;
	
	FCollisionQueryParams TraceParams;
	

	UCapsuleComponent* PlayerCapsule;
	AFloor* Floor;
	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* GrabLocation;

	UPROPERTY(EditAnywhere)
	UPhysicsHandleComponent* Handle;

	bool LandScapeHit;

	bool IsReloading = false;
	bool IsGrabbing = false;
	
	bool AllowBuilding = false;

	void MayReload();
	void CanFire();

	UPROPERTY(EditAnywhere, Category = "Config")
	USoundBase* Waves;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AAssaultRifle> GunClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AFloor> FloorClass;
	
	UPROPERTY(EditAnywhere, Category = "Health")
	int32 MaxHealth = 100;
	
	UPROPERTY(EditAnywhere, Category = "Ammo")
	float ReloadRate = 2;

//Interactable setup
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionCheckFrequency;

	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	float InteractionCheckDistance;

	void PerformInteractionCheck();

	void CouldntFindInteractable();

	void FoundNewInteractable(UInteractionComponent * Interactable);

	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerBeginInteract();

	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerEndInteract();

	void Interact();

	void BeginInteract();
	void EndInteract();
	
	FTimerHandle TimerHandle_Interact;

	UPROPERTY()
	FInteractionData InteractionData;

	FORCEINLINE class UInteractionComponent* GetInteractable() const {return InteractionData.ViewedInteractionComponent;}
	
};