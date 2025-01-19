// Fill out your copyright notice in the Description page of Project Settings.


#include "ExtendedMovementComp/ExtendedCharacter.h"
#include "ExtendedMovementComp/ExtendedCharacterMovementComp.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "CombatComponent/CombatComponent.h"
#include "RecoilAnimationComponent.h"

// Sets default values
AExtendedCharacter::AExtendedCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UExtendedCharacterMovementComp>
		(ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	ExtendedCharacterMovementComponent = Cast<UExtendedCharacterMovementComp>(GetCharacterMovement());
	ExtendedCharacterMovementComponent->SetIsReplicated(true);

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->GroundFriction = 2.f;
	GetCharacterMovement()->MaxWalkSpeed = 550.f;
	GetCharacterMovement()->MaxAcceleration = 1660.f;

	GetCharacterMovement()->BrakingFrictionFactor = 0.15f;
	GetCharacterMovement()->bUseSeparateBrakingFriction = true;
	GetCharacterMovement()->BrakingFriction = 0.15f;
	GetCharacterMovement()->Mass = 200.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;

	GetCharacterMovement()->MaxWalkSpeedCrouched = 200.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1024.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 300.f;

	// important to set of 60.f for correct sliding
	GetCharacterMovement()->CrouchedHalfHeight = 60.f;

	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->AirControlBoostMultiplier = 2.f;
	GetCharacterMovement()->FallingLateralFriction = 500.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh(), "head");
	CameraBoom->TargetArmLength = 0.0f;
	CameraBoom->bUsePawnControlRotation = false;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	RecoilAnimationComponent = CreateDefaultSubobject<URecoilAnimationComponent>(TEXT("RecoilAnimationComponent"));
}

void AExtendedCharacter::Jump()
{
	Super::Jump();

	bPressedExtendedJump = true;
	bPressedJump = false;

	if (bIsCrouched)
	{
		UnCrouch();
	}
	StopSprinting();
}

void AExtendedCharacter::StopJumping()
{
	Super::StopJumping();

	bPressedExtendedJump = false;
}

bool AExtendedCharacter::CanJumpInternal_Implementation() const
{
	if (Super::CanJumpInternal_Implementation())
	{
		return true;
	}
	else
	{
		return ExtendedCharacterMovementComponent->IsCrouching() &&
			!ExtendedCharacterMovementComponent->IsFalling();
	}
}

void AExtendedCharacter::StartAiming()
{
	StopSprinting();
	ExtendedCharacterMovementComponent->Safe_bWantsToWalk = true;
	CombatComponent->StartADS();
}

void AExtendedCharacter::StopAiming()
{
	ExtendedCharacterMovementComponent->Safe_bWantsToWalk = false;
	CombatComponent->StopADS();
}

void AExtendedCharacter::StartSprinting()
{
	ExtendedCharacterMovementComponent->SprintPressed();

	GetWorld()->GetTimerManager().SetTimer(
		SprintCheckTimerHandle, 
		this, 
		&AExtendedCharacter::StopSprintingOnMinimalSpeed, 
		0.1f, 
		true);
	//SprintStopDelegate.Broadcast();
}

void AExtendedCharacter::StopSprinting()
{
	ExtendedCharacterMovementComponent->SprintReleased();

	GetWorld()->GetTimerManager().ClearTimer(SprintCheckTimerHandle);
}

void AExtendedCharacter::StopSprintingOnMinimalSpeed()
{
	if (GetVelocity().Length() <= MinimalSpeedToStopSprinting)
	{
		StopSprinting();
	}
}

void AExtendedCharacter::StartProne()
{
	if (ExtendedCharacterMovementComponent->IsSprinting())
	{
		StopSprinting();
		ExtendedCharacterMovementComponent->DolphinDivePressed();
	}
	else
	{
		ExtendedCharacterMovementComponent->PronePressed();
	}
}

void AExtendedCharacter::StopProne()
{
	ExtendedCharacterMovementComponent->ProneReleased();
}

// Called when the game starts or when spawned
void AExtendedCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitializedCharacterInput();

	//SprintStopDelegate.AddDynamic(this, &AExtendedCharacter::StopSprintingOnMinimalSpeed);
}

void AExtendedCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (ExtendedCharacterMovementComponent->IsDolphinDive())
	{
		// apply custom params
		ExtendedCharacterMovementComponent->GravityScale = 1.f;
		ExtendedCharacterMovementComponent->BrakingDecelerationFalling = 300.f;
		ExtendedCharacterMovementComponent->FallingLateralFriction = 500.f;

		ExtendedCharacterMovementComponent->Safe_bWantsToDolphinDive = false;
		ExtendedCharacterMovementComponent->PronePressed();
	}
}

// Called every frame
void AExtendedCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator AimRotation = GetControlRotation();

	FRotator Delta = AimRotation - GetActorRotation();
	Delta.Normalize();
	LookRotation = Delta;

	auto AimRot = GetBaseAimRotation();
	auto ActorRot = GetActorRotation();
	float PitchDelta = FRotator::NormalizeAxis(AimRot.Pitch - ActorRot.Pitch);

	PivotOffsetPitch = FMath::GetMappedRangeValueClamped(
		FVector2D(MinWeaponPitch, MaxWeaponPitch),
		FVector2D(MinWeaponPivot, MaxWeaponPivot),
		PitchDelta);
}

// Called to bind functionality to input
void AExtendedCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AExtendedCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AExtendedCharacter::Look);

		// Aiming
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AExtendedCharacter::StartAiming);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AExtendedCharacter::StopAiming);

		// Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AExtendedCharacter::StartSprinting);

		// Proning
		EnhancedInputComponent->BindAction(ProneAction, ETriggerEvent::Started, this, &AExtendedCharacter::StartProne);
	}
}

void AExtendedCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		MoveX = MovementVector.X;

		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AExtendedCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		LookX = LookAxisVector.X;
		LookY = LookAxisVector.Y;

		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}

	if (CombatComponent && CombatComponent->bIsRecoilRecoveryActive)
	{
		FRotator currentRotation = GetControlRotation();
		FRotator checkpointRotation = CombatComponent->RecoilCheckpoint;

		FRotator deltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(currentRotation, checkpointRotation);

		if (LookAxisVector.Y < 0.f)
		{
			CombatComponent->bIsRecoilRecoveryActive = false;
			CombatComponent->bIsRecoilNeutral = true;
			return;
		}

		if (deltaRotation.Pitch < 0.f)
		{
			CombatComponent->bUpdateRecoilPitchCheckpointInNextShot = true;
		}

		if (LookAxisVector.X != 0.f)
		{
			if (CombatComponent->bIsRecoilYawRecoveryActive)
			{
				CombatComponent->bIsRecoilYawRecoveryActive = false;
			}

			CombatComponent->bUpdateRecoilYawCheckpointInNextShot = true;
		}
	}
}

void AExtendedCharacter::OnAimStarted_Implementation(){}
void AExtendedCharacter::OnAimEnded_Implementation(){}

void AExtendedCharacter::InitializedCharacterInput()
{
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

FCollisionQueryParams AExtendedCharacter::GetIgnoreCharacterParams() const
{
	FCollisionQueryParams Params;
	TArray<AActor*> CharacterChildren;
	GetAllChildActors(CharacterChildren);

	Params.AddIgnoredActors(CharacterChildren);
	Params.AddIgnoredActor(this);

	return Params;
}


