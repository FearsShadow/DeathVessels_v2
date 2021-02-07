// Copyright Jonathan Bejarano, DeathVessels C++

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "InteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginInteract, class AMyCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndInteract, class AMyCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginFocus, class AMyCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndFocus, class AMyCharacter*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, class AMyCharacter*, Character);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DEATHVESSELS_API UInteractionComponent : public UWidgetComponent
{
	GENERATED_BODY()
protected:

	virtual void Deactivate() override;

	bool CanInteract(class AMyCharacter* Character) const;

	UPROPERTY()
	TArray<class AMyCharacter*> Interactors;
public:
	UInteractionComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	float InteractionDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractableNameText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText InteractableActionText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bAllowMultipleInteractors;

	void RefreshWidget();

	void BeginFocus(class AMyCharacter * Character);
	void EndFocus(class AMyCharacter * Character);

	void BeginInteract(class AMyCharacter * Character);
	void EndInteract(class AMyCharacter * Character);

	void Interact(class AMyCharacter * Character);

	UFUNCTION(BlueprintPure, Category = "Info")
	float GetInteractionPercentage();

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnBeginInteract OnBeginInteract;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnEndInteract OnEndInteract;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnBeginFocus OnBeginFocus;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnBeginFocus OnEndFocus;

	UPROPERTY(EditDefaultsOnly, BlueprintAssignable)
	FOnInteract OnInteract;

	UFUNCTION(BlueprintCallable, Category = "Interactable")
	void SetInteractableNameText(const FText& NewNameText);

	UFUNCTION(BlueprintCallable, Category = "Interactable")
	void SetInteractableActionText(const FText& NewActionText);


};
