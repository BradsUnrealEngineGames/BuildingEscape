// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDoor.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include <sstream>

#define OUT
// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	if (!PressurePlate) {
		UE_LOG(LogTemp, Error, TEXT("%s missing attached pressure plate!"), *(GetOwner()->GetName()))
	}
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Poll the trigger volume
	if (GetTotalMassOfActorsOnPlate() > TriggerMass) {
		OnOpenRequest.Broadcast();
		LastDoorOpenTime = GetWorld()->GetTimeSeconds();
	}

	/* Casted float to FString for debugging message

	std::stringstream ss;
	ss << GetWorld()->GetTimeSeconds() - LastDoorOpenTime;
	FString HappyString(ss.str().c_str());
	UE_LOG(LogTemp, Warning, TEXT("Time since last opening %s"), *HappyString);
	*/

	if (GetWorld()->GetTimeSeconds() - LastDoorOpenTime > DoorCloseDelay) {
		
		OnCloseRequest.Broadcast();
	}
}

float UOpenDoor::GetTotalMassOfActorsOnPlate() {
	float TotalMass = 0.f;
	TArray<AActor*> OverlappingActors;
	/// Find all overlapping actors
	if (!PressurePlate) {
		return 1;
	}
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);
	/// Iterate through overlapping actors
	for (AActor* Actor : OverlappingActors) {
		UE_LOG(LogTemp, Warning, TEXT("%s overlapping pressure plate"), *(Actor->GetName()))
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}
	return TotalMass;
}

