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
	Inventory = CreateDefaultSubobject<UInventorySystem>("Inventory");
	Inventory->Capacity = 20;

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
		
	

	//need a better check here for ai
	// if (IsPlayerControlled() == false)
	// {
	// 	AR = GetWorld()->SpawnActor<AAssaultRifle>(GunClass);
	// 	AR->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("hand_r"));
	// 	AR->SetOwner(this);
	// }

	TraceParams.AddIgnoredActor(this);
	
	// 100 times scale of object
	FloorSnapLocation.Add(FVector(-350, 0, 0));
	FloorSnapLocation.Add(FVector(350, 0, 0));
	FloorSnapLocation.Add(FVector(0, 350, 0));
	FloorSnapLocation.Add(FVector(0, -350, 0));
	FloorSnapLocation.Add(FVector(350, 350, 0));
	FloorSnapLocation.Add(FVector(-350, 350, 0));
	FloorSnapLocation.Add(FVector(-350, -350, 0));
	FloorSnapLocation.Add(FVector(350, -350, 0));

	WallSnapLocation.Add(FVector(-165, 0, 0));
	WallSnapLocation.Add(FVector(165, 0, 0));
	WallSnapLocation.Add(FVector(0, 165, 0));
	WallSnapLocation.Add(FVector(0, -165, 0));
	//WallSnapLocation.Add(FVector(0, 0, 165));

	RoofSnapLocation.Add(FVector(-350, 0 , 0));
	RoofSnapLocation.Add(FVector(350, 0 , 0));
	RoofSnapLocation.Add(FVector(0, 350, 0));
	RoofSnapLocation.Add(FVector(0, -350 , 0));

	BuildingTypes.Add(FVector(3.5, 3.5, 1));
	BuildingTypes.Add(FVector(0.2, 3.5, 3));
	BuildingTypes.Add(FVector(3.5, 3.5, 0.3));

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
		Handle->SetTargetLocationAndRotation(LineTraceEnd, PlacementRotation);
	}
	if(AllowBuilding)
	{
		BuildSystem(LineTraceEnd);
	}
	//interactionsystem
	PerformInteractionCheck();
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

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AMyCharacter::StartJump);
	//do not switch these two crouches doing so will result in the toggle breaking CrouchEnd always first here, Crouch second. The reason for this is you need the crouchend to run before crouch so Crouchend doesn't try to run first and break the process
	//PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &AMyCharacter::CrouchEnd);
	//PlayerInputComponent->BindAction(TEXT("Crouch"), EInputEvent::IE_Pressed, this, &AMyCharacter::Crouch);
	PlayerInputComponent->BindAction(TEXT("Grab"), EInputEvent::IE_Pressed, this, &AMyCharacter::Grab);
	PlayerInputComponent->BindAction(TEXT("Grab"), EInputEvent::IE_Released, this, &AMyCharacter::ReleaseThrow);

	PlayerInputComponent->BindAction(TEXT("Interact"), EInputEvent::IE_Pressed, this, &AMyCharacter::BeginInteract);
	PlayerInputComponent->BindAction(TEXT("Interact"), EInputEvent::IE_Released, this, &AMyCharacter::EndInteract);

	PlayerInputComponent->BindAction(TEXT("BuildMenu"), EInputEvent::IE_Pressed, this, &AMyCharacter::BuildMenu);
	PlayerInputComponent->BindAction(TEXT("SwitchF"), EInputEvent::IE_Pressed, this, &AMyCharacter::SwitchUp);
	PlayerInputComponent->BindAction(TEXT("SwitchB"), EInputEvent::IE_Pressed, this, &AMyCharacter::SwitchDown);

	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Pressed, this, &AMyCharacter::Fire);
	PlayerInputComponent->BindAction(TEXT("LeftClick"), EInputEvent::IE_Pressed, this, &AMyCharacter::LeftClick);
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
	DOREPLIFETIME(AMyCharacter, IsToolless);
}
void AMyCharacter::ServerYMovement_Implementation(float AxisValue)
{
	YMovement(AxisValue);
}
void AMyCharacter::ServerXMovement_Implementation(float AxisValue)
{
	XMovement(AxisValue);
}
void AMyCharacter::ServerUnArmedRotation_Implementation()
{
	UnArmedRotation();
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
	if(IsToolless)
	{
		UnArmedRotation();
	}
	else if(XInput != 0 && YInput != 0)
	{
		WalkSpeed = 0.35;
		GetMesh()->SetRelativeRotation(FMath::Lerp(GetMesh()->GetRelativeRotation(), FRotator(0, -90, 0), 0.13));
	}
	else
	{
		WalkSpeed = 0.60;
		GetMesh()->SetRelativeRotation(FMath::Lerp(GetMesh()->GetRelativeRotation(), FRotator(0, -90, 0), 0.13));
	}

	AddMovementInput(GetActorForwardVector()* AxisValue, WalkSpeed);
	YInput = AxisValue;
	
	
}


void AMyCharacter::UnArmedRotation()
{
	if(XInput != 0 && YInput != 0 )
	{
		if(WalkSpeed != 1)
		{
			WalkSpeed = 0.35;
		}
		if(XInput < 0 && YInput > 0)
		{
			GetMesh()->SetRelativeRotation(FMath::Lerp(GetMesh()->GetRelativeRotation(), FRotator(0, -135, 0), 0.13));
		}
		//backwards
		else if(XInput > 0 && YInput < 0)
		{
			GetMesh()->SetRelativeRotation(FMath::Lerp(GetMesh()->GetRelativeRotation(), FRotator(0, -135, 0), 0.13));
			WalkSpeed = 0.35;
		}
		//backwards
		else if(XInput < 0 && YInput < 0)
		{

			GetMesh()->SetRelativeRotation(FMath::Lerp(GetMesh()->GetRelativeRotation(), FRotator(0, -45, 0), 0.13));
			WalkSpeed = 0.35;
		}
		else if(XInput > 0 && YInput > 0)
		{
			GetMesh()->SetRelativeRotation(FMath::Lerp(GetMesh()->GetRelativeRotation(), FRotator(0, -45, 0), 0.13));
		}
	}
	else if(XInput == 0 && YInput != 0)
	{
		if(WalkSpeed != 1)
		{
			WalkSpeed = 0.55;
		}
		if(YInput < 0)
		{
			WalkSpeed = 0.55;
		}
		GetMesh()->SetRelativeRotation(FMath::Lerp(GetMesh()->GetRelativeRotation(), FRotator(0, -90, 0), 0.13));
	}
	else if(XInput != 0 && YInput == 0)
	{
		if(WalkSpeed != 1)
		{
			WalkSpeed = 0.55;
		}
		if(XInput < 0)
		{
			GetMesh()->SetRelativeRotation(FMath::Lerp(GetMesh()->GetRelativeRotation(), FRotator(0, -180, 0), 0.13));
		}
		else if(XInput > 0)
		{
			//here is where it causes the player to turn the wrong way
			GetMesh()->SetRelativeRotation(FMath::Lerp(GetMesh()->GetRelativeRotation(), FRotator(0, 0, 0), 0.13));
			
		}
	}
	else if(XInput == 0 && YInput == 0)
	{
		if(WalkSpeed != 1)
		{
			WalkSpeed = 0.55;
		}

		GetMesh()->SetRelativeRotation(FMath::Lerp(GetMesh()->GetRelativeRotation(), FRotator(0, -90, 0), 0.13));
	}
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
		IsToolless = false;
		UE_LOG(LogTemp, Warning, TEXT("UnFailed"))
		
	}
	else if (IsAR == true && PlayerIsJumping == false )
	{
		AR->Destroy();
		IsAR = false;
		IsToolless = true;
		
	}
	else if (IsHatchet == true && PlayerIsJumping == false)
	{
		Hatchet->Destroy();
		IsHatchet = false;
		IsToolless = true;
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
	if ( IsAR == false && IsHatchet == false && IsGrabbing == false && PlayerIsJumping == false)
	{
		Hatchet = GetWorld()->SpawnActor<AHatchet>(HatchetClass);
		Hatchet->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("hand_r"));
		Hatchet->SetOwner(this);
		IsHatchet = true;
		IsToolless = false;
		
	}
	else if (IsHatchet == true && PlayerIsJumping == false && Hatchet != nullptr)
	{
		Hatchet->Destroy();
		IsHatchet = false;
		IsToolless = true;
		
	}
	//switches between weapons
	else if (IsAR == true && PlayerIsJumping == false && AR != nullptr)
	{
		AR->Destroy();
		IsAR = false;
		IsToolless = true;	
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
}

void AMyCharacter::ServerLeftClick_Implementation()
{
	Hatchet->ServerSwing(GetController());
}

void AMyCharacter::LeftClick()
{
	if (!IsAR && IsHatchet)
	{
		
		if(!HasAuthority() && Hatchet != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hatchet"))
			ServerLeftClick();
		}
		else if(Hatchet != nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Hatchet == nullptr"))
		}
	}
	else if (!IsAR && !IsHatchet && AllowBuilding && AllowedPlacement)
	{
		//I need to reduce the strain perferabbly not running the linetrace in tick
		FindPlacementLocation();
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
		FVector Location;
		FRotator Rotation;

		GetController()->GetPlayerViewPoint(Location, Rotation);

		FVector End = (Rotation.Vector() * 600 + Location);

		bool AxeHit = GetWorld()->LineTraceSingleByChannel(OutHit, Location, End, ECC_Visibility, TraceParams);
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

void AMyCharacter::UseItem(class UItems *Item)
{
	if (Item)
	{
		Item->Use(this);
		Item->OnUse(this);
		IsReloading = true;
		GetWorld()->GetTimerManager().SetTimer(FireRateDelay, this, &AMyCharacter::CanFire, ReloadRate, true); // bp event
	}
}

void AMyCharacter::CanFire()
{
	IsReloading = false;
}

//maybe needs to be bool

//set for multiplayer from down here
void AMyCharacter::FindPlacementLocation()
{
	//input function ran off leftclick
	if(!HasAuthority())
	{	
		ServerFindPlacementLocation(Floor->GetActorLocation(), Floor->GetActorRotation(), BuildItemNum);
	}
}
void AMyCharacter::ServerFindPlacementLocation_Implementation(FVector Client, FRotator ClientRotation, int32 BuildObjectNum)
{
	Floor = GetWorld()->SpawnActor<AFloor>(FloorClass);
	MulticastFindPlacementLocation(Client, ClientRotation, BuildObjectNum);
	
}
bool AMyCharacter::ServerFindPlacementLocation_Validate(FVector Client, FRotator ClientRotation, int32 BuildObjectNum)
{
	return true;
}

void AMyCharacter::MulticastFindPlacementLocation_Implementation(FVector Client, FRotator ClientRotation, int32 BuildObjectNum)
{
	if(Floor != nullptr)
	{
		if(BuildObjectNum == 0)
		{
			Floor->SetActorRelativeScale3D(BuildingTypes[BuildObjectNum] + FVector(0, 0, 0.5));
		}
		else
		{
			Floor->SetActorRelativeScale3D(BuildingTypes[BuildObjectNum]);
		}
		Floor->SetActorLocation(Client);
		Floor->SetActorRotation(ClientRotation);	
	}
}

void AMyCharacter::BuildMenu()
{
	if (!AllowBuilding)
	{
		Floor = GetWorld()->SpawnActor<AFloor>(FloorClass);
		Floor->SetActorRelativeScale3D(BuildingTypes[BuildItemNum]);
		AllowBuilding = true;
	}
	else
	{
		Floor->Destroy();
		AllowBuilding = false;
	}
}

void AMyCharacter::ServerSwitchUp_Implementation()
{
	if(BuildItemNum < 2)
	{
		++BuildItemNum;
	}
	SwitchUp();
	
}
void AMyCharacter::SwitchUp()
{	
	if(!HasAuthority())
	{
		ServerSwitchUp();
	}
	if (AllowBuilding && BuildItemNum < 2)
	{
		++BuildItemNum;
		Floor->SetActorRelativeScale3D(BuildingTypes[BuildItemNum]);
		AllowedPlacement = false;
		Floor->MaterialRed();
	}
}

void AMyCharacter::ServerSwitchDown_Implementation()
{
	if(BuildItemNum >= 1)
	{
		--BuildItemNum;
	}
	SwitchDown();
}
void AMyCharacter::SwitchDown()
{
	if(!HasAuthority())
	{
		ServerSwitchDown();
	}
	if (AllowBuilding && BuildItemNum >= 1)
	{
		--BuildItemNum;
		Floor->SetActorRelativeScale3D(BuildingTypes[BuildItemNum]);
		AllowedPlacement = false;
		Floor->MaterialRed();
	}
}

void AMyCharacter::BuildSystem(FVector PlacementEnd)
{
	// is all the logic necessary in tick
	if (AllowBuilding && IsAR == false && IsHatchet == false && !HasAuthority())
	{
		bool BuildingSnap = GetWorld()->LineTraceSingleByChannel(OutHit, PlacementLocation, PlacementEnd, ECollisionChannel::ECC_GameTraceChannel1, TraceParams);

		if(BuildingSnap == false)
		{
			GetWorld()->LineTraceSingleByChannel(OutHit, PlacementLocation, PlacementEnd, ECollisionChannel::ECC_GameTraceChannel2, TraceParams);
		}

		if (OutHit.GetActor() == nullptr && Floor != nullptr)
		{
			AllowedPlacement = false;
			Floor->SetActorLocation(PlacementEnd + FVector(0, 0, 50));
			Floor->MaterialRed();
		}
		
		else if (OutHit.GetActor() != nullptr )
		{
			// placing floor
			Floor->OverlapTrace();
			if (!BuildingSnap && BuildItemNum == 0)
			{
				FVector FloorStart = Floor->GetActorLocation() + FVector(0, 0, 35);
				bool FloorCheck = GetWorld()->LineTraceSingleByChannel(CubeHit, FloorStart, (FloorStart + FRotator(-90, 0,0).Vector() * 140), ECollisionChannel::ECC_GameTraceChannel2, TraceParams);
				
				if (FloorCheck)
				{
					FVector Start = Floor->GetActorLocation() - FVector(0,0, 15);

						bool CubeTrace1 = GetWorld()->LineTraceSingleByChannel(CubeHit, Start, (Start + FRotator(0, 135,0).Vector() * 300), ECollisionChannel::ECC_GameTraceChannel1, TraceParams);
						bool CubeTrace2 = GetWorld()->LineTraceSingleByChannel(CubeHit, Start, (Start + FRotator(0, 45,0).Vector() * 300), ECollisionChannel::ECC_GameTraceChannel1, TraceParams);
						bool CubeTrace3 = GetWorld()->LineTraceSingleByChannel(CubeHit, Start, (Start + FRotator(0, -135,0).Vector() * 300), ECollisionChannel::ECC_GameTraceChannel1, TraceParams);
						bool CubeTrace4 = GetWorld()->LineTraceSingleByChannel(CubeHit, Start, (Start + FRotator(0, -45,0).Vector() * 300), ECollisionChannel::ECC_GameTraceChannel1, TraceParams);

						if(CubeTrace1 || CubeTrace2 || CubeTrace3 || CubeTrace4)
						{	
							Floor->MaterialRed();
							AllowedPlacement = false;
						}
						else
						{
							Floor->SetActorLocation(PlacementEnd + FVector(0, 0, 50));
							Floor->MaterialGreen();
							AllowedPlacement = true;
						}
				}
			}
			//for walls and floor snapping
			if(BuildingSnap)
			{	
				Floor->BlockTrace();
				Floor->MaterialGreen();
				Floor->OverlapDisplay();
				float ShortestDistance = FVector::Distance(PlacementEnd, FloorSnapLocation[0] + OutHit.GetActor()->GetActorLocation());
				float ShortestDistanceRoof = FVector::Distance(GetOwner()->GetActorLocation(), RoofSnapLocation[0] + OutHit.GetActor()->GetActorLocation());
				int32 IndexOfShortest = 0;
				
				//Floor only
				if (BuildItemNum == 0 )
				{
					for (int i = 1; i < FloorSnapLocation.Num(); i++)
					{
						if (FVector::Distance(PlacementEnd, FloorSnapLocation[i] + OutHit.GetActor()->GetActorLocation()) < ShortestDistance)
						{
							ShortestDistance = FVector::Distance(PlacementEnd, FloorSnapLocation[i] + OutHit.GetActor()->GetActorLocation());
							IndexOfShortest = i;
						}
					}

					if(OutHit.GetActor()->GetActorRelativeScale3D().X == BuildingTypes[0].X )
					{
						Floor->OverlapTrace();

						Floor->SetActorLocation(OutHit.GetActor()->GetActorLocation()  + FloorSnapLocation[IndexOfShortest]);
						FVector Start = Floor->GetActorLocation() + FVector(0,0, 60);

						bool FloorCheck = GetWorld()->LineTraceSingleByChannel(CubeHit, Start, (Start + FRotator(-90, 0,0).Vector() * 140), ECollisionChannel::ECC_GameTraceChannel2, TraceParams);
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
				else
				{
					for (int i = 1; i < WallSnapLocation.Num(); i++)
					{
						if (FVector::Distance(PlacementEnd, WallSnapLocation[i] + OutHit.GetActor()->GetActorLocation()) < ShortestDistance)
						{
							ShortestDistance = FVector::Distance(PlacementEnd, WallSnapLocation[i] + OutHit.GetActor()->GetActorLocation());
							IndexOfShortest = i;
						}
					}
					
					AllowedPlacement = true;
					if(OutHit.GetActor()->GetActorRelativeScale3D().X == BuildingTypes[0].X && BuildItemNum == 1)
					{
						//can specify a bit more here on what it's hitting as this outhit allows for two things
						if(OutHit.GetActor()->GetActorRelativeScale3D().Z == BuildingTypes[2].Z)
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
							ServerRotate = true;
						}
						
						else
						{
							Floor->SetActorRotation(FRotator(0, 0, 0));
							ServerRotate = false;
						}
					}
					
					//ROOF PLACING
					else if(OutHit.GetActor()->GetActorRelativeScale3D().X == BuildingTypes[1].X && BuildItemNum == 2 )
					{	
						if(OutHit.GetActor()->GetActorRotation() == FRotator(0,0,0))
						{
							if(this->GetActorLocation().X > OutHit.GetActor()->GetActorLocation().X)
							{
								Floor->SetActorLocation(OutHit.GetActor()->GetActorLocation() + FVector(165, 0 , 160));
							}
							else if(this->GetActorLocation().X < OutHit.GetActor()->GetActorLocation().X)
							{
								Floor->SetActorLocation(OutHit.GetActor()->GetActorLocation() + FVector(-165, 0 , 160));
							}
						}
						else if(OutHit.GetActor()->GetActorRotation() != FRotator(0,0,0))
						{
							if(this->GetActorLocation().Y < OutHit.GetActor()->GetActorLocation().Y)
							{
								Floor->SetActorLocation(OutHit.GetActor()->GetActorLocation() + FVector(0, -165 , 160));
								
							}
							else if(this->GetActorLocation().Y > OutHit.GetActor()->GetActorLocation().Y)
							{
								Floor->SetActorLocation(OutHit.GetActor()->GetActorLocation() + FVector(0, 165 , 160));
							}
						}	
					}
					//Wall rotation and check
					else if(OutHit.GetActor()->GetActorRelativeScale3D().Z == BuildingTypes[1].Z && BuildItemNum == 1)
					{
						Floor->SetActorLocation(FVector(0, 0, 300) + OutHit.GetActor()->GetActorLocation());
						if(OutHit.GetActor()->GetActorRotation() == FRotator(0,0,0))
						{
							Floor->SetActorRotation(FRotator(0,0,0));
						}
						else
						{
							Floor->SetActorRotation(FRotator(0, 90, 0));
						}
					}
					else if(OutHit.GetActor()->GetActorRelativeScale3D().Z == BuildingTypes[2].Z && BuildItemNum == 2)
					{
						IndexOfShortest = 0;
						for (int i = 1; i < WallSnapLocation.Num(); i++)
						{
							if (FVector::Distance(PlacementEnd, RoofSnapLocation[i] + OutHit.GetActor()->GetActorLocation()) < ShortestDistance)
							{
								ShortestDistance = FVector::Distance(PlacementEnd, RoofSnapLocation[i] + OutHit.GetActor()->GetActorLocation());
								IndexOfShortest = i;
							}
						}
						Floor->SetActorLocation(OutHit.GetActor()->GetActorLocation() + RoofSnapLocation[IndexOfShortest]);
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

}

//interaction system
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
	if(Interactable)
	{
		Interactable->SetHiddenInGame(false);
		InteractionData.ViewedInteractionComponent = Interactable;
	}
	UE_LOG(LogTemp, Warning, TEXT("found interactable"))
}
void AMyCharacter::CouldntFindInteractable()
{
	if(InteractionData.ViewedInteractionComponent)
	{
		InteractionData.ViewedInteractionComponent->SetHiddenInGame(true);
	}
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

