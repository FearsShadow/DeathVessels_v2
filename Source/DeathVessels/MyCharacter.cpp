// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Floor.h"
#include "Hatchet.h"
#include "AssaultRifle.h"
#include "Components/CapsuleComponent.h"
#include "InteractionComponent.h"
#include "Net/UnrealNetwork.h"




//
// Sets default values
AMyCharacter::AMyCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Health = MaxHealth;
	// ...


	InventoryTest = CreateDefaultSubobject<UInventoryTest>("InventoryTest");

	Handle = CreateDefaultSubobject<UPhysicsHandleComponent>("Handle");

	InteractionCheckFrequency = 0;
	InteractionCheckDistance = 1000;	
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerCapsule = this->GetCapsuleComponent();
	PlayerCapsule->GetScaledCapsuleSize(OutRadius, OutHalfHeight);
	
	
	
	if(Waves != nullptr)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Waves, FVector(0,0,0));
	}

	if(FloorClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Floorclass is nullptr"))
	}
	//need a better check here for ai
	// if (IsPlayerControlled() == false)
	// {
	// 	AR = GetWorld()->SpawnActor<AAssaultRifle>(GunClass);
	// 	AR->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("hand_r"));
	// 	AR->SetOwner(this);
	// }

	TraceParams.AddIgnoredActor(this);
	

	FloorSnapLocation.Add(FVector(-350, 0, 0));
	FloorSnapLocation.Add(FVector(350, 0, 0));
	FloorSnapLocation.Add(FVector(0, 350, 0));
	FloorSnapLocation.Add(FVector(0, -350, 0));
	FloorSnapLocation.Add(FVector(350, 350, 0));
	FloorSnapLocation.Add(FVector(-350, 350, 0));
	FloorSnapLocation.Add(FVector(-350, -350, 0));
	FloorSnapLocation.Add(FVector(350, -350, 0));

	FloorSnapConversions.Add(FVector(-100, 0, 0));
	FloorSnapConversions.Add(FVector(100, 0, 0));
	FloorSnapConversions.Add(FVector(0, 100, 0));
	FloorSnapConversions.Add(FVector(0, -100, 0));
	FloorSnapConversions.Add(FVector(100, 100, 0));
	FloorSnapConversions.Add(FVector(-100, 100, 0));
	FloorSnapConversions.Add(FVector(-100, -100, 0));
	FloorSnapConversions.Add(FVector(100, -100, 0));
	


	WallSnapLocation.Add(FVector(-165, 0, 0));
	WallSnapLocation.Add(FVector(165, 0, 0));
	WallSnapLocation.Add(FVector(0, 165, 0));
	WallSnapLocation.Add(FVector(0, -165, 0));

	WallSnapConversion.Add(FVector(-47, 0, 147));
	WallSnapConversion.Add(FVector(47, 0, 147));
	WallSnapConversion.Add(FVector(0, 47, 147));
	WallSnapConversion.Add(FVector(0, -47, 147));
	//-47 positive and negative x and y && 147 solid z

	RoofSnapLocation.Add(FVector(-350, 0 , 0));
	RoofSnapLocation.Add(FVector(350, 0 , 0));
	RoofSnapLocation.Add(FVector(0, 350, 0));
	RoofSnapLocation.Add(FVector(0, -350 , 0));

	RoofSnapConversion.Add(FVector(-100, 0 , 0));
	RoofSnapConversion.Add(FVector(100, 0 , 0));
	RoofSnapConversion.Add(FVector(0, 100, 0));
	RoofSnapConversion.Add(FVector(0, -100 , 0));

	//824 pos negative x solid 53 z

	BuildingTypes.Add(FVector(3.5f, 3.5f, 1.f));
	BuildingTypes.Add(FVector(0.2f, 3.5f, 3.f));
	BuildingTypes.Add(FVector(3.5f, 3.5f, 0.3f));
	BuildingTypes.Add(FVector(1.9f, 1.8f, 0.7f));

	FindPlayer();
}

void AMyCharacter::Tick(float DeltaTime)
{
	//Don't use GetOwner before actor location in tick
	Super::Tick(DeltaTime);
	
	if(GetController() != nullptr)
	{
		if(AllowBuilding || Handle->GetGrabbedComponent() != nullptr)
		{
			GetController()->GetPlayerViewPoint(PlacementLocation, PlacementRotation);
			LineTraceEnd = (PlacementLocation + PlacementRotation.Vector() * 600);
			LengthOfGrabber = (PlacementLocation + PlacementRotation.Vector() * GrabLength);
			GetWorld()->LineTraceSingleByChannel(OutHit, PlacementLocation, LineTraceEnd, ECC_Visibility, TraceParams);
		}
	}
	
	
	
	if (Handle->GetGrabbedComponent() != nullptr)
	{
		//testing
		if (FVector::Distance(GetActorLocation(), Handle->GetGrabbedComponent()->GetComponentLocation()) < 100 || PlayerIsJumping == true)
		{
			ReleaseDrop();
		}
		Handle->SetTargetLocationAndRotation(LengthOfGrabber, PlacementRotation);
		
	}
	else
	{
		
	}
	//interactionsystem
	
	const bool IsInteractingOnServer = (HasAuthority() && IsInteracting());

	if(!HasAuthority() || IsInteractingOnServer && GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		PerformInteractionCheck();
	}
	
}

//INPUT
//Movement, firing, reloading
// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("XLookDirection"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("YLookDirection"), this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis(TEXT("XMovement"), this, &AMyCharacter::XMovement);
	PlayerInputComponent->BindAxis(TEXT("YMovement"), this, &AMyCharacter::YMovement);

	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Pressed, this, &AMyCharacter::Sprint);
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Released, this, &AMyCharacter::SprintRelease);
	PlayerInputComponent->BindAction(TEXT("Extend"), EInputEvent::IE_Repeat, this, &AMyCharacter::GrabExtend);
	PlayerInputComponent->BindAction(TEXT("Detract"), EInputEvent::IE_Repeat, this, &AMyCharacter::GrabDetract);

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AMyCharacter::StartJump);
	//do not switch these two crouches doing so will result in the toggle breaking CrouchEnd always first here, Crouch second. The reason for this is you need the crouchend to run before crouch so Crouchend doesn't try to run first and break the process
	//PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &AMyCharacter::CrouchEnd);
	//PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &AMyCharacter::Crouch);
	PlayerInputComponent->BindAction(TEXT("Grab"), EInputEvent::IE_Pressed, this, &AMyCharacter::Grab);
	PlayerInputComponent->BindAction(TEXT("Grab"), EInputEvent::IE_Released, this, &AMyCharacter::ReleaseThrow);

	PlayerInputComponent->BindAction(TEXT("Interact"), EInputEvent::IE_Pressed, this, &AMyCharacter::BeginInteract);
	PlayerInputComponent->BindAction(TEXT("Interact"), EInputEvent::IE_Released, this, &AMyCharacter::EndInteract);

	//PlayerInputComponent->BindAction(TEXT("BuildMenu"), EInputEvent::IE_Pressed, this, &AMyCharacter::BuildMenu);

	
	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Pressed, this, &AMyCharacter::Fire);
	PlayerInputComponent->BindAction(TEXT("Reload"), EInputEvent::IE_Pressed, this, &AMyCharacter::Reload);

	PlayerInputComponent->BindAction(TEXT("Weapon1"), EInputEvent::IE_Pressed, this, &AMyCharacter::ServerWeapon1);
	PlayerInputComponent->BindAction(TEXT("Weapon2"), EInputEvent::IE_Pressed, this, &AMyCharacter::ServerWeapon2);
}
void AMyCharacter::GetLifetimeReplicatedProps(TArray < FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AMyCharacter, YInput);
	DOREPLIFETIME(AMyCharacter, XInput);
	DOREPLIFETIME(AMyCharacter, IsHatchet);
	DOREPLIFETIME(AMyCharacter, IsAR);
	DOREPLIFETIME(AMyCharacter, AR);
	DOREPLIFETIME(AMyCharacter, Hatchet)
	DOREPLIFETIME(AMyCharacter, WalkSpeed);
	DOREPLIFETIME(AMyCharacter, Health);
	DOREPLIFETIME(AMyCharacter, SwingAxe);
	DOREPLIFETIME(AMyCharacter, Wood);
}
void AMyCharacter::ServerYMovement_Implementation(float AxisValue)
{
	YMovement(AxisValue);
}
void AMyCharacter::ServerXMovement_Implementation(float AxisValue)
{
	XMovement(AxisValue);
}

void AMyCharacter::XMovement(float AxisValue)
{	
	if(!HasAuthority())
	{
		ServerXMovement(AxisValue);
	}
	if(XInput != 0 && YInput == 0 && WalkSpeed != 1)
	{
		WalkSpeed = 0.55;
	}
	AddMovementInput(GetActorRightVector()* AxisValue * WalkSpeed);
	XInput = AxisValue;

}
void AMyCharacter::YMovement(float AxisValue)
{	
	if(!HasAuthority())
	{
		ServerYMovement(AxisValue);
	}
	else if(XInput != 0 && YInput != 0 && WalkSpeed != 1)
	{
		WalkSpeed = 0.35;
		
	}
	else if(WalkSpeed != 1)
	{
		WalkSpeed = 0.60;
		
	}

	AddMovementInput(GetActorForwardVector()* AxisValue, WalkSpeed);
	YInput = AxisValue;
	
	
}


void AMyCharacter::Sprint()
{
	if(!HasAuthority())
	{
		ServerSprint();
	}
	if(!(XInput < 0 && YInput < 0) && !(XInput > 0 && YInput < 0))
	{
		WalkSpeed = 1;
		
	}
}
void AMyCharacter::ServerSprint_Implementation()
{
	Sprint();
}
void AMyCharacter::ServerSprintRelease_Implementation()
{
	SprintRelease();
}
void AMyCharacter::SprintRelease()
{
	if(!HasAuthority())
	{
		ServerSprintRelease();
	}
	WalkSpeed = 0.6;
}
//Weapon Switching
void AMyCharacter::ServerWeapon1_Implementation()
{
	if (IsAR == false && IsHatchet == false && IsGrabbing == false && PlayerIsJumping == false)
	{
		AR = GetWorld()->SpawnActor<AAssaultRifle>(GunClass);
		AR->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("hand_r"));
		AR->SetOwner(this);
		IsAR = true;
		
	}
	else if (IsAR == true && PlayerIsJumping == false )
	{
		AR->Destroy();
		IsAR = false;
		
	}
	else if (IsHatchet == true && PlayerIsJumping == false)
	{
		Hatchet->Destroy();
		IsHatchet = false;
		ServerWeapon1();	
	}
	if(AR == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed"))
	}
}

void AMyCharacter::ServerWeapon2_Implementation()
{
	//might have some issues without having isgrabbing and player is jumping replicated
	if (IsAR == false && IsHatchet == false && IsGrabbing == false && PlayerIsJumping == false)
	{
		Hatchet = GetWorld()->SpawnActor<AHatchet>(HatchetClass);
		Hatchet->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("HatchetSocket"));
		Hatchet->SetOwner(this);
		IsHatchet = true;
		
	}
	else if (IsHatchet == true && PlayerIsJumping == false && Hatchet != nullptr)
	{
		Hatchet->Destroy();
		IsHatchet = false;
		
	}
	//switches between weapons
	else if (IsAR == true && PlayerIsJumping == false && AR != nullptr)
	{
		AR->Destroy();
		IsAR = false;
		ServerWeapon2();
	}
	
}


void AMyCharacter::ServerFire_Implementation(int32 Bullets)
{
	if(Bullets > 0 )
	{
		AR->Bullet();
	}
	UE_LOG(LogTemp, Warning, TEXT("ammo %i"), Bullets)
	--Bullets;

}

void AMyCharacter::Fire()
{
	
	if(BulletsInMag > 0)
	{
		if (!IsReloading && IsAR || IsPlayerControlled() == false && AR != nullptr)
		{
			AR->PullTrigger(this);
		}
		if(!HasAuthority() && !IsReloading && IsAR || IsPlayerControlled() == false)
		{
			ServerFire(BulletsInMag);
		}
		else if(AR == nullptr)
		{
			//this will sometimes happen just means you have to reset what the gunclass is in the character bp
			UE_LOG(LogTemp, Error, TEXT("AR == nullptr"))
		}
	}
	else if(!IsAR)
	{
		LeftClick();
	}
}

void AMyCharacter::ServerLeftClick_Implementation()
{
	
	if(!GetWorldTimerManager().IsTimerActive(HatchetSwingDelay) && SwingAxe == false)
	{
		
		//to make them swing faster all you have to do is increase the playrate of the chop and then decrease the timer.
		GetWorldTimerManager().SetTimer(HatchetSwingDelay, this, &AMyCharacter::SwingAxeTimer, 1.6f, false);
		SwingAxe = true;
		
		Hatchet->ServerSwing(GetController(), this);
	}

}

void AMyCharacter::LeftClick()
{
	
	if (!IsAR && IsHatchet)
	{
		
			if(!HasAuthority() && Hatchet != nullptr)
			{
				ServerLeftClick();
				
			}
			else if(Hatchet != nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("Hatchet == nullptr"))
			}	
	}

	else if (!IsAR && !IsHatchet)
	{
		ReleaseDrop();
		
	}
}
	
void AMyCharacter::Reload()
{
	//this is never created on the client therefore it causes the game to crash when ran
	
	if (PlayerIsJumping == false && IsAR && AR != nullptr)
	{
		IsReloading = true;
		GetWorld()->GetTimerManager().SetTimer(FireRateDelay, this, &AMyCharacter::CanFire, ReloadRate, true);
		AR->ReloadMagazine(this);
	}
	// else if(AR == nullptr)
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("Reload AR == nullptr"))
	// }
}
void AMyCharacter::StartJump()
{
	//It may be time to switch over to using enums
	if (TurnOffCrouch == false && PlayerIsJumping == false && IsReloading == false)
	{
		GetWorld()->GetTimerManager().SetTimer(FireRateDelay, this, &AMyCharacter::JumpEndAnim, 1.6, false);
		PlayerIsJumping = true;
		bPressedJump = true;
		

	}
}

void AMyCharacter::ServerGrab_Implementation()
{
	Grab();
}
void AMyCharacter::ServerReleaseThrow_Implementation()
{
	ReleaseThrow();
}
void AMyCharacter::ServerReleaseDrop_Implementation()
{
	ReleaseDrop();
}
//Input Related to the GrabberSystem
void AMyCharacter::Grab()
{
	//find some way to reduce the length that cube is at
	if(!HasAuthority())
	{
		ServerGrab();
	}
	// if you really want to remove the bug with the block sliding when you attempt to grab, you could use a linetrace in order to check if a block is under you and then not allow them to pick stuff up
	if (IsAR == false && IsHatchet == false && AllowBuilding == false)
	{
		FVector GrabbingLocation;
		FRotator GrabRotation;

		GetController()->GetPlayerViewPoint(GrabbingLocation, GrabRotation);

		FVector GrabEnd = (GrabRotation.Vector() * 600 + GrabbingLocation);

		bool AxeHit = GetWorld()->LineTraceSingleByChannel(OutHit, GrabbingLocation, GrabEnd, ECC_Visibility, TraceParams);
		if (AxeHit)
		{
			if (OutHit.GetActor() && OutHit.GetComponent()->IsSimulatingPhysics() && Handle != nullptr)
			{
				Handle->GrabComponentAtLocation(OutHit.GetComponent(), FName("thumb_03_l"), OutHit.GetActor()->GetActorLocation() );
				UE_LOG(LogTemp, Warning, TEXT("%s"), *OutHit.GetActor()->GetActorLocation().ToString())
				IsGrabbing = true;
			}
		}
	}
}
void AMyCharacter::ServerGrabDetract_Implementation()
{
	GrabDetract();
}

void AMyCharacter::ServerGrabExtend_Implementation()
{
	GrabExtend();
}

void AMyCharacter::GrabExtend()
{
	if(!HasAuthority())
	{
		ServerGrabExtend();
	}
	if(GrabLength < 600)
	{
		GrabLength += 20;

	}
}
void AMyCharacter::GrabDetract()
{
	if(!HasAuthority())
	{
		ServerGrabDetract();
	}
	if(GrabLength > 160)
	{
		GrabLength -= 20;
	}
}

void AMyCharacter::ReleaseThrow()
{
	
	if(!HasAuthority())
	{
		ServerReleaseThrow();
	}
	if (Handle->GetGrabbedComponent() != nullptr)
	{
		Handle->GetGrabbedComponent()->SetPhysicsLinearVelocity(FVector(GetActorForwardVector().X * 1200, GetActorForwardVector().Y * 1200, 0));
	}
	Handle->ReleaseComponent();
	IsGrabbing = false;
}
void AMyCharacter::ReleaseDrop()
{
	if(!HasAuthority())
	{
		ServerReleaseDrop();
	}
	if (Handle->GetGrabbedComponent() != nullptr || AllowBuilding == true && Handle->GetGrabbedComponent() != nullptr)
	{
		Handle->GetGrabbedComponent()->SetPhysicsLinearVelocity(FVector(0, 0, 0));
		Handle->ReleaseComponent();
		IsGrabbing = false;
	}
}
//UI
//These 3 are UI Related
void AMyCharacter::ServerHealthPercentage_Implementation() const
{
	HealthPercentage();
}

float AMyCharacter::HealthPercentage() const
{
	if(!HasAuthority())
	{
		ServerHealthPercentage();
	}
	return (Health / 100.f);
}
int32 AMyCharacter::DisplayAmmo()
{
	return Ammo;
}
int32 AMyCharacter::DisplayMagazineAmmo()
{
	return BulletsInMag;
}

//ANIMATION
//These 4 are Anim Related
bool AMyCharacter::AnimationReload()
{
	//Small bug here when reloading the last amount of your ammo it won't do the animations
	if (IsReloading)
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool AMyCharacter::IsDead() const
{
	if (Health <= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// bool AMyCharacter::StandToCrouch()
// {
// 	if(TurnOffCrouch)
// 	{
// 		return true;
// 	}
// 	else
// 	{
// 		return false;
// 	}
// }

//ANIMATION
//These 2 ANIMATION toggle
// void AMyCharacter::Crouch()
// {
// 	if(StartCrouch && PlayerIsJumping == false)
// 	{
// 		IsCrouched = 0.5;
// 		TurnOffCrouch = true;
// 		PlayerCapsule->SetCapsuleSize(OutRadius, (OutHalfHeight + 30) /2 );
// 		GetMesh()->SetRelativeLocation(GetMesh()->GetRelativeLocation() + FVector(0, 0, 30));
// 	}
// 	StartCrouch = true;
// }

// void AMyCharacter::CrouchEnd()
// {
// 	if(TurnOffCrouch)
// 	{
// 		IsCrouched = 1;
// 		PlayerCapsule->SetCapsuleSize(OutRadius, OutHalfHeight);
// 		GetMesh()->SetRelativeLocation(GetMesh()->GetRelativeLocation() + FVector(0, 0, -30));
// 		StartCrouch = false;
// 		TurnOffCrouch = false;
// 	}
// }

void AMyCharacter::JumpEndAnim()
{
	PlayerIsJumping = false;
	bPressedJump = false;
}

//Other Logic
float AMyCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser)
{
	
	int32 DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageApplied = FMath::Min(Health, DamageApplied);
	Health -= DamageApplied;
	
	if (IsDead())
	{
		DetachFromControllerPendingDestroy();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Destroy();
	}
	UE_LOG(LogTemp, Warning, TEXT("%i"), Health)
	return DamageApplied;
}

int32 AMyCharacter::FindPlayer()
{
	APlayerController *MainPlayer = Cast<APlayerController>(GetOwner());
	int32 PlayerID = UGameplayStatics::GetPlayerControllerID(MainPlayer);
	if (IsPlayerControlled())
	{
		return PlayerID;
	}
	else
	{
		return 0;
	}
}



void AMyCharacter::CanFire()
{
	IsReloading = false;
}

//maybe needs to be bool
void AMyCharacter::BP_CleanupBuild()
{
	if(!HasAuthority())
	{
		if(Floor != nullptr)
		{
			ServerCleanupBuild();
			
		}	
	}
	if(Floor != nullptr)
	{	
			
		Floor->Destroy();
		AllowBuilding = false;
		AllowedPlacement = false;
	}
}
//set for multiplayer from down here
void AMyCharacter::ServerCleanupBuild_Implementation()
{
	//use nullchecks
	//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%s"), *MainActor->GetName()));
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%i"), FloorsArray.Num()));
	AllowBuilding = false;
	for(int32 i = 0; i < FloorsArray.Num(); i++)
	{
		FloorsLocations.Add(FloorsArray[i]->GetActorLocation());
		FloorsArray[i]->Destroy();
	}
	if(MainActor != nullptr)
	{
		MainActor->Destroy();
	}
		
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("%s"), *FloorsLocations[0].ToString()));
	
	//use for loop to get the locations of the array and then add it to an fvector array
	//then try destroying either the parent actor first and then if not try destroying all the attached actors

	//Spawn all the actors back in attaching them to the og parent actor, and set them to their og locations, then moving the owning actor towards the water or a dock
	//Simulate physics if something is it a linetrace is hitting the water and nothing is obstructing it.
	
}

//perfect for system with bp just needs to be called upon
void AMyCharacter::BP_FindPlacementLocation(const int32 BuildingPiece)
{
	//input function ran off leftclick
	if(!HasAuthority())
	{	
		
		if(OutHit.GetActor() != nullptr && AllowedPlacement && AllowBuilding)
		{	
			ServerFindPlacementLocation(Floor->GetActorLocation(), Floor->GetActorRotation(), BuildingPiece, OutHit.GetActor(), LandScapeHit, IndexOfShortest, RoofLocation);
	
		}	

	}
}
void AMyCharacter::ServerFindPlacementLocation_Implementation(const FVector Client, const FRotator ClientRotation, const int32 BuildObjectNum,  AActor* FloorActor, const bool LandHit, const int32 ShortestIndex, const FVector RoofLoc)
{
	Floor = GetWorld()->SpawnActor<AFloor>(FloorClass);
	//Maybe instead of using all these things like roofloc and stuff maybe you can just set a value at the end of buildkit
	MulticastFindPlacementLocation(Client, ClientRotation, BuildObjectNum, ShortestIndex, LandHit, RoofLoc);
	//really need to setup a linetrace for this but it's alright for now
	if(LandHit)
	{
		MainActor = Floor;
		//Bug that has to do with landhit being activated when 

	}
	else
	{
		Floor->AttachToActor(FloorActor, FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false));
		ChildActors = Floor;
		FloorsArray.Add(ChildActors);

		//use for loop in order to get the world location and then check what it is cloest to on snap locations
		
		//
		
	}
	
}

void AMyCharacter::MulticastFindPlacementLocation_Implementation(const FVector Client, const FRotator ClientRotation, const int32 BuildObjectNum, const int32 ShortestIndex, const bool LandHit, const FVector RoofLoc)
{
	//only works for the first block
	//perfect for land hit need to use info from buildkit in order to get the correct snap location for some of these relative one's however
	if(Floor != nullptr )
	{
		//landscape is hit
		
		if(BuildObjectNum == 0 )
		{
			Floor->SetActorScale3D(BuildingTypes[BuildObjectNum] + FVector(0,0, 0.5));
			
		}
		else
		{
			Floor->SetActorScale3D(BuildingTypes[BuildObjectNum]);
		}
		
		if(LandHit)
		{
			Floor->SetActorLocation(Client);
		}
		else if(BuildObjectNum == 0)
		{
			Floor->SetActorLocation(FloorSnapConversions[ShortestIndex]);
		}
		else if(BuildObjectNum == 1)
		{
			//time to switch this
			Floor->SetActorLocation(WallSnapConversion[ShortestIndex]);
			// gonna need to mess with this
		}
		else if(BuildObjectNum == 2)
		{
			Floor->SetActorLocation(RoofLoc);
		}
		
			
		Floor->SetActorRotation(ClientRotation);	
	}
	
}


void AMyCharacter::BP_BuildMenu(int32 BuildingPiece)
{
	if(Floor != nullptr)
	{
		Floor->Destroy();
	}
	Floor = GetWorld()->SpawnActor<AFloor>(FloorClass);
	
	Floor->SetFloorShape(BuildingPiece);
	Floor->SetActorScale3D(BuildingTypes[BuildingPiece]);
	Floor->Overlap();
	BuildingPiece = 0; //may cause issue
	AllowBuilding = true; 
	AllowedPlacement = false;
	
}
void AMyCharacter::BP_BuildKit(int32 BuildingPiece)
{
	//BuildingPiece 0 is floors && roofs
	GetController()->GetPlayerViewPoint(Location, Rotation);
	End = (Location + Rotation.Vector() * 600);
	
	if(Floor != nullptr && AllowBuilding)
	{
		TraceParams.AddIgnoredActor(Floor);
	
		bool BuildingSnap = GetWorld()->LineTraceSingleByChannel(OutHit, Location, End, ECollisionChannel::ECC_GameTraceChannel1, TraceParams);
		if(BuildingSnap == false)
		{
			LandScapeHit = true;
			GetWorld()->LineTraceSingleByChannel(OutHit, Location, End, ECollisionChannel::ECC_GameTraceChannel2, TraceParams);
		}
		else
		{
			LandScapeHit = false;
		}

		if(OutHit.GetActor() == nullptr)
		{	
			if(Floor != nullptr)
			{
				Floor->SetActorLocation(End + FVector(0, 0, 50));
				Floor->MaterialRed();
			}
			AllowedPlacement = false;
		}
		else if(OutHit.GetActor() != nullptr)
		{
			Floor->OverlapTrace();
			if(BuildingPiece == 0 && LandScapeHit)
			{
			//is this trace needed?
				bool FloorCheck = GetWorld()->LineTraceSingleByChannel(CubeHit, Floor->GetActorLocation() + FVector(0, 0, 35), (Floor->GetActorLocation() + FVector(0, 0, 35) + FRotator(-90, 0,0).Vector() * 140), ECollisionChannel::ECC_GameTraceChannel2, TraceParams);
			
				if (FloorCheck)
				{
						FVector CubeStart = Floor->GetActorLocation() - FVector(0,0, 15);
						//try ecc visibility may help from buildings overlapping
						bool CubeTrace1 = GetWorld()->LineTraceSingleByChannel(CubeHit, CubeStart, (CubeStart + FRotator(0, 135,0).Vector() * 300), ECollisionChannel::ECC_GameTraceChannel1, TraceParams);
						bool CubeTrace2 = GetWorld()->LineTraceSingleByChannel(CubeHit, CubeStart, (CubeStart + FRotator(0, 45,0).Vector() * 300), ECollisionChannel::ECC_GameTraceChannel1, TraceParams);
						bool CubeTrace3 = GetWorld()->LineTraceSingleByChannel(CubeHit, CubeStart, (CubeStart + FRotator(0, -135,0).Vector() * 300), ECollisionChannel::ECC_GameTraceChannel1, TraceParams);
						bool CubeTrace4 = GetWorld()->LineTraceSingleByChannel(CubeHit, CubeStart, (CubeStart + FRotator(0, -45,0).Vector() * 300), ECollisionChannel::ECC_GameTraceChannel1, TraceParams);

						if(CubeTrace1 || CubeTrace2 || CubeTrace3 || CubeTrace4)
					{	
						Floor->MaterialRed();
						AllowedPlacement = false;
					}
						else
					{
						Floor->SetActorLocation(End + FVector(0, 0, 50));
						Floor->MaterialGreen();
						AllowedPlacement = true;
					}
				}
			
			}
			else if(BuildingSnap)
			{
			LandScapeHit = false;
			Floor->BlockTrace();
			Floor->OverlapDisplay();

			float ShortestDistance = FVector::Distance(End, FloorSnapLocation[0] + OutHit.GetActor()->GetActorLocation());
			float ShortestDistanceRoof = FVector::Distance(GetOwner()->GetActorLocation(), RoofSnapLocation[0] + OutHit.GetActor()->GetActorLocation());
			IndexOfShortest = 0;

			//don't try to use buildtype values that use floats as then it leads to the system getting bugged.
			//by using indexofshortest and floornsnaplocation
			if (BuildingPiece == 0 )
			{
				//Floor snap
				
				for (int i = 1; i < FloorSnapLocation.Num(); i++)
				{
					if (FVector::Distance(LineTraceEnd, FloorSnapLocation[i] + OutHit.GetActor()->GetActorLocation()) < ShortestDistance)
					{
						ShortestDistance = FVector::Distance(LineTraceEnd, FloorSnapLocation[i] + OutHit.GetActor()->GetActorLocation());
						IndexOfShortest = i;
					}
				}

				if(OutHit.GetActor()->GetActorScale3D().X == BuildingTypes[0].X)
				{
					Floor->OverlapTrace();
					Floor->SetActorLocation(OutHit.GetActor()->GetActorLocation()  + FloorSnapLocation[IndexOfShortest]);

					FVector FloorStart = Floor->GetActorLocation() + FVector(0,0, 60);
					bool FloorCheck = GetWorld()->LineTraceSingleByChannel(CubeHit, FloorStart, (FloorStart + FRotator(-90, 0,0).Vector() * 140), ECollisionChannel::ECC_GameTraceChannel2, TraceParams);
					
					if(FloorCheck)
					{
						AllowedPlacement = true;
						Floor->MaterialGreen();
					}
					else
					{
						AllowedPlacement = false;
						Floor->MaterialRed();
					}
						
				}
				else
				{
					AllowedPlacement = false;
					Floor->MaterialRed();
				}
			}
			else if(BuildingPiece == 1)
			{
				//Wall Snap
				
					for (int i = 1; i < WallSnapLocation.Num(); i++)
					{
						if (FVector::Distance(LineTraceEnd, WallSnapLocation[i] + OutHit.GetActor()->GetActorLocation()) < ShortestDistance)
						{
							ShortestDistance = FVector::Distance(LineTraceEnd, WallSnapLocation[i] + OutHit.GetActor()->GetActorLocation());
							IndexOfShortest = i;
						}
					}
					if(OutHit.GetActor()->GetActorScale3D().X == BuildingTypes[0].X)
					{
						//can specify a bit more here on what it's hitting as this outhit allows for two things
						
						Floor->MaterialGreen();
						if(OutHit.GetActor()->GetActorScale3D().Z < 1)
						{
							Floor->SetActorLocation(OutHit.GetActor()->GetActorLocation()  + WallSnapLocation[IndexOfShortest] +  FVector(0,0, 140));
						}
						else
						{
							Floor->SetActorLocation(OutHit.GetActor()->GetActorLocation()  + WallSnapLocation[IndexOfShortest] +  FVector(0,0, 220));
						}
						
						if(IndexOfShortest == 2 || IndexOfShortest == 3)
						{
							Floor->SetActorRotation(FRotator(0, 90, 0));
						}
						else
						{
							Floor->SetActorRotation(FRotator(0, 0, 0));
						}
						AllowedPlacement = true;
					}
					else if(OutHit.GetActor()->GetActorScale3D().Z == BuildingTypes[1].Z)
					{
						//check
						Floor->SetActorLocation(FVector(0, 0, 300) + OutHit.GetActor()->GetActorLocation());
						
						if(OutHit.GetActor()->GetActorRotation() == FRotator(0,0,0))
						{
							Floor->SetActorRotation(FRotator(0,0,0));
						}
						else
						{
							Floor->SetActorRotation(FRotator(0, 90, 0));
						}
						Floor->MaterialGreen();
						AllowedPlacement = true;
					}		
			}
			else if(BuildingPiece == 2)
			{
				if(OutHit.GetActor()->GetActorScale3D().Z == BuildingTypes[1].Z)
				{	
						
						if(OutHit.GetActor()->GetActorRotation() == FRotator(0,0,0))
						{
							if(this->GetActorLocation().X > OutHit.GetActor()->GetActorLocation().X)
							{
								Floor->SetActorLocation(OutHit.GetActor()->GetActorLocation() + FVector(165, 0 , 160));
								RoofLocation = FVector(855, -0.5 , 51);
							}
							else if(this->GetActorLocation().X < OutHit.GetActor()->GetActorLocation().X)
							{
								Floor->SetActorLocation(OutHit.GetActor()->GetActorLocation() + FVector(-165, 0 , 160));
								RoofLocation =  FVector(-855, -0.5 , 51);
								//
							}
						}
						else if(OutHit.GetActor()->GetActorRotation() != FRotator(0,0,0))
						{
							if(this->GetActorLocation().Y < OutHit.GetActor()->GetActorLocation().Y)
							{
								Floor->SetActorLocation(OutHit.GetActor()->GetActorLocation() + FVector(0, -165 , 160));
								RoofLocation =  FVector(-855, -0.5 , 51);
							}
							else if(this->GetActorLocation().Y > OutHit.GetActor()->GetActorLocation().Y)
							{
								Floor->SetActorLocation(OutHit.GetActor()->GetActorLocation() + FVector(0, 165 , 160));
								RoofLocation = FVector(855, -0.5 , 51);
							}
						}
						
						Floor->MaterialGreen();
						AllowedPlacement = true;

					
				}
				else if(OutHit.GetActor()->GetActorScale3D().X == BuildingTypes[2].X && OutHit.GetActor()->GetActorScale3D().X != BuildingTypes[0].X)
				{
						
					IndexOfShortest = 0;
					for (int i = 1; i < WallSnapLocation.Num(); i++)
					{
						if (FVector::Distance(LineTraceEnd, RoofSnapLocation[i] + OutHit.GetActor()->GetActorLocation()) < ShortestDistance)
						{
							ShortestDistance = FVector::Distance(LineTraceEnd, RoofSnapLocation[i] + OutHit.GetActor()->GetActorLocation());
							IndexOfShortest = i;
						}
					}
					Floor->SetActorLocation(OutHit.GetActor()->GetActorLocation() + RoofSnapLocation[IndexOfShortest]);
					RoofLocation = RoofSnapConversion[IndexOfShortest];
					Floor->MaterialGreen();
					AllowedPlacement = true;	
				}
			}
			else
			{
				AllowedPlacement = false;
				Floor->MaterialRed();
			}
			}
		
		}
	}
    
		

}

//
void AMyCharacter::ServerSwitchUp_Implementation(int32 BuildingPiece)
{
	//probably gonna need this to bp
	if(BuildingPiece < 3)
	{
		++BuildingPiece;
	}
	BP_SwitchUp(BuildingPiece);
	
}
int32 AMyCharacter::BP_SwitchUp(int32 BuildingPiece)
{	
	if(!HasAuthority())
	{
		ServerSwitchUp(BuildingPiece);
	}
	if (AllowBuilding && BuildingPiece < 3)
	{
		++BuildingPiece;

		Floor->SetFloorShape(BuildingPiece);
		Floor->SetActorScale3D(BuildingTypes[BuildingPiece]);
		AllowedPlacement = false;
		Floor->MaterialRed();
		
		return BuildingPiece;
	}
	return BuildingPiece;
}

void AMyCharacter::ServerSwitchDown_Implementation(int32 BuildingPiece)
{
	if(BuildingPiece >= 1)
	{
		--BuildingPiece;
	}
	BP_SwitchDown(BuildingPiece);
}
int32 AMyCharacter::BP_SwitchDown(int32 BuildingPiece)
{
	if(!HasAuthority())
	{
		ServerSwitchDown(BuildingPiece);
	}
	if (AllowBuilding && BuildingPiece >= 1)
	{
		--BuildingPiece;

		Floor->SetFloorShape(BuildingPiece);
		Floor->SetActorScale3D(BuildingTypes[BuildingPiece]);
		AllowedPlacement = false;
		Floor->MaterialRed();

		return BuildingPiece;
	}
	return BuildingPiece;
}


//interaction system from rueban ward tutorial
void AMyCharacter::PerformInteractionCheck()
{
	if(GetController() == nullptr){return;}
	FVector EyesLoc;
	FRotator EyesRot;

	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();
	
	GetController()->GetPlayerViewPoint(EyesLoc, EyesRot);

	FVector TraceStart = EyesLoc;
	FVector TraceEnd = ((EyesRot.Vector()* InteractionCheckDistance) + TraceStart);
	FHitResult TraceHit;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if(GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		if(TraceHit.GetActor())
		{
			if(UInteractionComponent* InteractionComponent = Cast<UInteractionComponent>(TraceHit.GetActor()->GetComponentByClass(UInteractionComponent::StaticClass())))
			{
				float Distance = (TraceStart - TraceHit.ImpactPoint).Size();

				if(InteractionComponent != GetInteractable() && Distance <= InteractionComponent->InteractionDistance)
				{
					FoundNewInteractable(InteractionComponent);
				}
				else if(Distance > InteractionComponent->InteractionDistance && GetInteractable())
				{
					CouldntFindInteractable();
				}
				return;
			}
		}
	}
	CouldntFindInteractable();
}
void AMyCharacter::FoundNewInteractable(UInteractionComponent * Interactable)
{
	EndInteract();

	if(UInteractionComponent* OldInteractable = GetInteractable())
	{
		OldInteractable->EndFocus(this);
	}
	InteractionData.ViewedInteractionComponent = Interactable;
	Interactable->BeginFocus(this);
}
void AMyCharacter::CouldntFindInteractable()
{
	if(GetWorldTimerManager().IsTimerActive(TimerHandle_Interact))
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Interact);
	}

	if(UInteractionComponent* Interactable = GetInteractable())
	{
		Interactable->EndFocus(this);

		if(InteractionData.bInteractionHeld)
		{
			EndInteract();
		}
	}

	InteractionData.ViewedInteractionComponent = nullptr;
}

void AMyCharacter::BeginInteract()
{
	if(!HasAuthority())
	{
		ServerBeginInteract();
	}
	
	InteractionData.bInteractionHeld = true;
	if(UInteractionComponent * Interactable = GetInteractable())
	{
		Interactable->BeginInteract(this);

		if(FMath::IsNearlyZero(Interactable->InteractionTime))
		{
			Interact();
		}
		else
		{
			GetWorldTimerManager().SetTimer(TimerHandle_Interact, this, &AMyCharacter::Interact, Interactable->InteractionTime, false);
		}
	}
}

void AMyCharacter::EndInteract()
{
	if(!HasAuthority())
	{
		ServerBeginInteract();
	}
	InteractionData.bInteractionHeld = false;

	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);
	
	if(UInteractionComponent * Interactable = GetInteractable())
	{
		Interactable->EndInteract(this);
	}
}

void AMyCharacter::ServerBeginInteract_Implementation()
{
	BeginInteract();
}
bool AMyCharacter::ServerBeginInteract_Validate()
{
	return true;
}

void AMyCharacter::ServerEndInteract_Implementation()
{
	BeginInteract();
}
bool AMyCharacter::ServerEndInteract_Validate()
{
	return true;
}

void AMyCharacter::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("AMyInter"))
	GetWorldTimerManager().ClearTimer(TimerHandle_Interact);
	
	if(UInteractionComponent * Interactable = GetInteractable())
	{
		Interactable->Interact(this);
	}
}

bool AMyCharacter::IsInteracting() const
{
	return GetWorldTimerManager().IsTimerActive(TimerHandle_Interact);
}

float AMyCharacter::GetRemainingInteractTime() const
{
	return GetWorldTimerManager().GetTimerRemaining(TimerHandle_Interact);
}

void AMyCharacter::SwingAxeTimer()
{
	SwingAxe = false;
}