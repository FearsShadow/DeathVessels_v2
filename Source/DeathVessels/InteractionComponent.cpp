// Copyright Jonathan Bejarano, DeathVessels C++


#include "InteractionComponent.h"
#include "MyCharacter.h"
#include "InteractionWidget.h"

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
	OnBeginFocus.Broadcast(Character);
	RefreshWidget();
}

void UInteractionComponent::EndFocus(class AMyCharacter * Character)
{
	OnEndFocus.Broadcast(Character);
	SetHiddenInGame(true);

}

void UInteractionComponent::BeginInteract(class AMyCharacter * Character)
{
	if(CanInteract(Character))
	{
		Interactors.AddUnique(Character);
		OnBeginInteract.Broadcast(Character);
	}
}

void UInteractionComponent::EndInteract(class AMyCharacter * Character)
{
	Interactors.RemoveSingle(Character);
	OnEndInteract.Broadcast(Character);
}

void UInteractionComponent::Interact(class AMyCharacter * Character)
{
	if(CanInteract(Character))
	{
		OnInteract.Broadcast(Character);
	}
    
}

void UInteractionComponent::SetInteractableNameText(const FText& NewNameText)
{
	InteractableNameText = NewNameText;
	RefreshWidget();
}
void UInteractionComponent::SetInteractableActionText(const FText& NewActionText)
{
	InteractableActionText = NewActionText;
	RefreshWidget();
}

void UInteractionComponent::Deactivate()
{
	Super::Deactivate();

	for(int32 i = Interactors.Num() - 1; i >= 0; --i)
	{
		if(AMyCharacter* Interactor = Interactors[i])
		{
			EndFocus(Interactor);
			EndInteract(Interactor);
		}
	}
	Interactors.Empty();
}

bool UInteractionComponent::CanInteract(class AMyCharacter * Character) const
{
	const bool bPlayerAlreadyInteracting = !bAllowMultipleInteractors && Interactors.Num() >= 1;
	return !bPlayerAlreadyInteracting && IsActive() && GetOwner() != nullptr && Character != nullptr;
}

float UInteractionComponent::GetInteractionPercentage()
{
	if(Interactors.IsValidIndex(0))
	{
		if(AMyCharacter* Interactor = Interactors[0])
		{
			if(Interactor && Interactor->IsInteracting())
			{
				return 1.f - FMath::Abs(Interactor->GetRemainingInteractTime() / InteractionTime);
			}
		}
	}
	return 0.f;
}

void UInteractionComponent::RefreshWidget()
{
	if(!bHiddenInGame && GetOwner()->GetNetMode() != NM_DedicatedServer)
	{
		if(UInteractionWidget* InteractionWidget = Cast<UInteractionWidget>(GetUserWidgetObject()))
		{
			InteractionWidget->UpdateInteractionWidget(this);
			
		}

	}
}