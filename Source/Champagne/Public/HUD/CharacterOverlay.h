// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

class UProgressBar;
class UTextBlock;
class UHitMarker;

/**
 * 
 */
UCLASS()
class CHAMPAGNE_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthPoint;	

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentArrows;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MaxArrows;

};
