// Copyright Jonathan Bejarano, DeathVessels C++

#include "Floor.h"
#include "AssaultRifle.h"
#include "Hatchet.h"
#include "UObject/UObjectGlobals.h"


//this whole thing may be uneeded
AFloor::AFloor()
{	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FloorBase = CreateDefaultSubobject<UStaticMeshComponent>("FloorBase");

	SetRootComponent(FloorBase);

	
}


// Called when the game starts or when spawned
void AFloor::SetFloorShape(int32 BuildingNum)
{
	//comment
	if(BuildingNum == 0)
	{
		FloorBase->SetStaticMesh(Floor);
	}
	else if(BuildingNum == 3)
	{
		FloorBase->SetStaticMesh(Triangle);
	}
	else if(BuildingNum == 4)
	{
		FloorBase->SetStaticMesh(TriangleWall);
	}
	else
	{
		FloorBase->SetStaticMesh(Cube);
	}
	FloorBase->SetAbsolute(true, true, true);
	
}
void AFloor::BeginPlay()
{
	Super::BeginPlay();
	FloorHealth = 100;


	
}

// Called every frame
void AFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
	if(FloorHealth <= 0)
	{
		this->Destroy();
	}
}

void AFloor::MaterialGreen()
{
	FloorBase->SetMaterial(0, Green);
}
void AFloor::MaterialRed()
{
	FloorBase->SetMaterial(0, Red);
}




void AFloor::BlockDisplay()
{

	FloorBase->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
}
void AFloor::OverlapDisplay()
{

	FloorBase->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);	
}

void AFloor::Overlap()
{
	if(FloorBase != nullptr)
	{
		FloorBase->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
}
void AFloor::Block()
{
	if(FloorBase != nullptr)
	{
		FloorBase->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		FloorBase->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
		FloorBase->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
		FloorBase->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);	
		FloorBase->SetMaterial(0, Original);
	}
}
void AFloor::BlockTrace()
{
	
	FloorBase->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);	
}

void AFloor::OverlapTrace()
{
	FloorBase->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);	
}

float AFloor::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	AHatchet* IsHatchetDamaging = Cast<AHatchet>(DamageCauser);
	AAssaultRifle* IsRifleDamaging = Cast<AAssaultRifle>(DamageCauser);
    //checks if hatchet is the one doing the damage
	if(IsHatchetDamaging)
	{
		DamageAmount = 4;
	}
	else if(IsRifleDamaging)
	{
		DamageAmount = 35;
	}
	else 
	{
		return 0;
	}
	
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageApplied = FMath::Min(FloorHealth, DamageApplied);
	FloorHealth -= DamageApplied;
	UE_LOG(LogTemp, Warning, TEXT("%f"), FloorHealth)//typically the amount of damage;
    return DamageApplied;
}