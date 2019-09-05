
#include "Grabber.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"

#define OUT
// Sets default values for this component's properties
UGrabber::UGrabber()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	/// Look for attached Physics Handle
	FindPhysicsHandleComponent();
	/// Set up (assumed) attached input component
	SetupInputComponent();
}

void UGrabber::FindPhysicsHandleComponent()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("%s missing physics handle component"), *(GetOwner()->GetName()))
	}
}

void UGrabber::SetupInputComponent()
{
	/// Look for attached input component
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s missing input component"), *(GetOwner()->GetName()))
	}
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// If the physics handle is attached
	if (PhysicsHandle->GrabbedComponent) {
		/// update location data and move the object that we're holding
		UpdateInfoForGrabber();
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}

	/// Log viewpoint every tick and create debug line (commented out)
	/*UE_LOG(LogTemp, Warning, TEXT("Current location: %s, Current rotation: %s"), *PlayerViewPointLocation.ToString(), *PlayerViewPointRotation.ToString())
	
	 Draw debugging reach line
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector()* GrabLength;
	DrawDebugLine(
		GetWorld(),
		PlayerViewPointLocation,
		LineTraceEnd,
		FColor(255, 0, 0),
		false,
		0.f,
		0,
		10.f
	);*/
}

void UGrabber::Grab()
{
	///Line trace and find first physics body actor
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();

	/// If we hit something then attach a physics handle
	if (ComponentToGrab) {
		PhysicsHandle->GrabComponent(ComponentToGrab, NAME_None /* root bone name */, ComponentToGrab->GetOwner()->GetActorLocation() /* grab location */, true /* allow rotation */);
	}
}

void UGrabber::Release()
{
		PhysicsHandle->ReleaseComponent();
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	UpdateInfoForGrabber();

	/// Set up query parameters
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	/// Ray cast out to reach distance
	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);

	/// See what we hit
	AActor* ObjectHit = Hit.GetActor();
	if (ObjectHit) {
		FString ActorName = ObjectHit->GetName();
		UE_LOG(LogTemp, Warning, TEXT("%s was hit"), *ActorName)
	}

	return Hit;
}

void UGrabber::UpdateInfoForGrabber()
{
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlayerViewPointLocation, OUT PlayerViewPointRotation);
	LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * GrabLength;
}
