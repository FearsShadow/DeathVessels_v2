// Fill out your copyright notice in the Description page of Project Settings.


#include "AiCharacter.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "MyCharacter.h"

void AAiCharacter::BeginPlay()
{
    Super::BeginPlay();

    StartLocation = GetPawn()->GetActorLocation();
}   

void AAiCharacter::Tick(float DeltaTime)
{   
    Super::Tick(DeltaTime);
    GetThePlayer();
}

void AAiCharacter::GetThePlayer()
{

    APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), PlayerID);

    if(LineOfSightTo(Player))
    {   
        FVector ActorLocation = Player->GetActorLocation();
        if(AllowFiring)
        {
            //Looks for capsule component
            GetWorld()->GetTimerManager().SetTimer(PatrolWaitDelay, this, &AAiCharacter::AIFire, 0.65, true);
        }    
        SetFocus(Player);
        MoveToActor(Player, Radius, false);
        MoveToLocation(ActorLocation, 5, false);
        AllowPatrol = true;
        AllowFiring = false;

    }
    //used to activate timer so player can head into the patrol state.
    else if(!LineOfSightTo(Player) && AllowPatrol == true)
    {
        GetWorld()->GetTimerManager().SetTimer(PatrolWaitDelay, this, &AAiCharacter::Patrol, 4.5, false);
        AllowPatrol = false;
        AllowFiring = true;
       
    }
    else
    {   
        //Starts the patrol getting the AI to head forward, sideways and then return home.
        
        ClearFocus(EAIFocusPriority::Gameplay);
        if(PatrolForward)
        {
            FVector Forward = (GetPawn()->GetActorLocation() + FVector(450, 0, 0));
            MoveToLocation(Forward, 0, false);
            PatrolForward = false;
        }
        else if(PatrolSideways)
        {
            int32 LeftOrRight = FMath::RandRange(0,1);
            if(LeftOrRight == 1)
            {
                FVector Left = (GetPawn()->GetActorLocation() + FVector(0, -450, 0));
                MoveToLocation(Left, 0, false);
            }
            else
            {
                FVector Right = (GetPawn()->GetActorLocation() + FVector(0, 450, 0));
                MoveToLocation(Right, 0, false);
                // will not shoot if they are in this return home state
            }
            
            PatrolSideways = false;

        }

        else if(GoBack)
        {
            MoveToLocation(StartLocation, 5, false);
            GoBack = false;
            
        }
    }
}

//AI Patrol
void AAiCharacter::Patrol()
{
    PatrolForward = true;
    GetWorld()->GetTimerManager().SetTimer(PatrolWaitDelay, this, &AAiCharacter::PatrolLeft, 1, false);
}
void AAiCharacter::PatrolLeft()
{   
    GetWorld()->GetTimerManager().SetTimer(PatrolWaitDelay, this, &AAiCharacter::HeadToStart, 3, false);
    PatrolSideways = true;
}
void AAiCharacter::HeadToStart()
{
    GoBack = true;
}

void AAiCharacter::AIFire()
{
    AMyCharacter* PlayerCharacter = Cast<AMyCharacter>(this->GetPawn());
    if(PlayerCharacter != nullptr)
    {
        PlayerCharacter->Fire();
    }  
    
}