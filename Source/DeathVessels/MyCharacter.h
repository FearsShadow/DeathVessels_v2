// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InventorySystem.h"
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

	UFUNCTION(BlueprintCallable, Category = "Items")
	void UseItem(UItems* Item);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAcess = "true"))
	class UInventorySystem* Inventory;
	
	int32 Ammo = 0;
	int32 BulletsInMag;
	
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

	void UnArmedRotation();

	void YMovement(float AxisValue);
	void XMovement(float AxisValue);
	void Sprint();
	void SprintRelease();
	
	UFUNCTION(Server, Unreliable)
	void ServerUnArmedRotation();

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



	UFUNCTION(Server, Reliable)
	void ServerFindPlacementLocation(FVector Client, FRotator ClientRotation, int32 BuildObjectNum, AActor* FloorActor , int32 y);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastFindPlacementLocation(FVector Client, FRotator ClientRotation, int32 BuildObjectNum, int32 y);

	void FindPlacementLocation();

	void BuildMenu();

	UFUNCTION(Server, Unreliable)
	void ServerSwitchUp();

	void SwitchUp();

	UFUNCTION(Server, Unreliable)
	void ServerSwitchDown();

	void SwitchDown();

	void BuildSystem(FVector PlacementLocation);
	
	

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
	
	int32 BuildItemNum = 0;
	int32 ValueIncreaser = 1;
	int32 z = 0;

	int32 CubeIncreaser = 0;
	//
	FVector ClientLocation;
	FVector PlacementLocation;
	FRotator PlacementRotation;
	
	FCollisionQueryParams TraceParams;
	
	TArray<FVector> RoofSnapLocation;
	TArray<FVector> BuildingTypes;
	TArray<FVector> FloorSnapLocation;
	TArray<FVector> WallSnapLocation;
	
	TArray<FVector> CubeSnapLocations;
	
	UCapsuleComponent* PlayerCapsule;
	AFloor* Floor;
	
	UPROPERTY(VisibleAnywhere)
	USceneComponent* GrabLocation;
	UPROPERTY(EditAnywhere)
	UPhysicsHandleComponent* Handle;

	
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