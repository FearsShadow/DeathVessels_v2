// Copyright Jonathan Bejarano, DeathVessels C++


#include "InteractionComponent.h"

UInteractionComponent::UInteractionComponent()
{
    SetComponentTickEnabled(false);

    InteractionTime = 1;
    InteractionDistance = 1000;
    InteractableNameText = FText::FromString("Interactable Object");
    InteractableActionText = FText::FromString("Interact");
    bAllowMultipleInteractors = true;

    Space = EWidgetSpace::Screen;
    DrawSize = FIntPoint(400, 100);
    bDrawAtDesiredSize = true;

    SetActive(true);
    SetHiddenInGame(true);
}

void UInteractionComponent::BeginFocus(class AMyCharacter * Character)
{
	if(!IsActive() || !GetOwner() || !Character)
	{
		return;
	}
	SetHiddenInGame(false);
    UE_LOG(LogTemp, Warning, TEXT("bruh"))
	OnBeginFocus.Broadcast(Character);

	if(!GetOwner()->HasAuthority())
	{
		for(auto& VisualComp : GetOwner()->GetComponents())
		{
			if(UPrimitiveComponent * Prim = Cast<UPrimitiveComponent>(VisualComp))
			{
				Prim->SetRenderCustomDepth(true);
			}
		}
	}
}

void UInteractionComponent::EndFocus(class AMyCharacter * Character)
{
}

void UInteractionComponent::BeginInteract(class AMyCharacter * Character)
{
}

void UInteractionComponent::EndInteract(class AMyCharacter * Character)
{
}

void UInteractionComponent::Interact(class AMyCharacter * Character)
{
    OnInteract.Broadcast(Character);
}