// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FPSGameState.generated.h"

/**
 * 
 */
UCLASS()
class FPSGAME_API AFPSGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	UFUNCTION(NetMulticast, Reliable) //If this function gets called on server, it will broadast to ALL clients
	void MulticastOnMissionComplete(APawn* InstigatorPawn, bool bMissionSuccess);
};
