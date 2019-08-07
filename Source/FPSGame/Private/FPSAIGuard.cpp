// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSAIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "FPSGameMode.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	GuardState = EAIState::Idle;

	UE_LOG(LogTemp, Log, TEXT("Finished in constructor for AIGuard"));
}

void AFPSAIGuard::PostInitializeComponents()
{
    Super::PostInitializeComponents();
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnPawnHeard);
	UE_LOG(LogTemp, Log, TEXT("Finished in PostInit for AIGuard"));
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();
	OriginalRotation = GetActorRotation();

}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFPSAIGuard::OnPawnSeen(APawn* SeenPawn)
{
	if (SeenPawn == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("NULL"));
		return;
	}

	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Red, false, 10);

	AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->CompleteMission(SeenPawn, false);
	}

	SetGuardState(EAIState::Alterted);
}

void AFPSAIGuard::OnPawnHeard(APawn *OtherActor, const FVector &Location, float Volume)
{
	if (GuardState == EAIState::Alterted)
	{
		return;
	}

	DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Green, false, 10);

	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();
	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	NewLookAt.Pitch = 0.0f;
	NewLookAt.Roll = 0.0f;
	SetActorRotation(NewLookAt);

	
	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);

	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAIGuard::ResetOrientation, 3.0f);

	if (GuardState != EAIState::Alterted)
	{
		SetGuardState(EAIState::Suspicious);
	}
}

void AFPSAIGuard::ResetOrientation()
{
	if (GuardState == EAIState::Alterted)
	{
		return;
	}

	SetActorRotation(OriginalRotation);

	SetGuardState(EAIState::Idle);
}

//This will only be called on the client
void AFPSAIGuard::OnRep_GuardState()
{
	OnStateChanged(GuardState);
}

void AFPSAIGuard::SetGuardState(EAIState NewState)
{
	if (GuardState == NewState)
	{
		return;
	}

	GuardState = NewState;
	OnRep_GuardState();
}

//set the rules for the replication
void AFPSAIGuard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//GuardState is replicated to all clients
	DOREPLIFETIME(AFPSAIGuard, GuardState);
}
