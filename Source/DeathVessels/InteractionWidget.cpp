// Copyright Jonathan Bejarano, DeathVessels C++


#include "InteractionWidget.h"
#include "InteractionComponent.h"

void UInteractionWidget::UpdateInteractionWidget(class UInteractionComponent* InteractionComponent)
{
    OwningInteractionComponent = InteractionComponent;
    OnUpdateInteractionWidget();
}