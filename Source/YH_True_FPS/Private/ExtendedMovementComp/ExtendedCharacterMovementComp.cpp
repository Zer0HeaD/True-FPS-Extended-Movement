// Fill out your copyright notice in the Description page of Project Settings.


#include "ExtendedMovementComp/ExtendedCharacterMovementComp.h"
#include "GameFramework/Character.h"
#include "ExtendedMovementComp/ExtendedCharacter.h"
#include "Components/CapsuleComponent.h"
#include "CombatComponent/CombatComponent.h"
#include "Engine/OverlapResult.h"

#include <Net/UnrealNetwork.h>
#include "DrawDebugHelpers.h"


// Helper Macros
#if 1
float MacroDuration = 5.f;
#define SLOG(x) GEngine->AddOnScreenDebugMessage(-1, MacroDuration ? MacroDuration : -1.f, FColor::Yellow, x);
#define POINT(x, c) DrawDebugPoint(GetWorld(), x, 10, c, !MacroDuration, MacroDuration);
#define LINE(x1, x2, c) DrawDebugLine(GetWorld(), x1, x2, c, !MacroDuration, MacroDuration);
#define CAPSULE(x, c) DrawDebugCapsule(GetWorld(), x, CapHH(), CapR(), FQuat::Identity, c, !MacroDuration, MacroDuration);
#else
#define SLOG(x)
#define POINT(x, c)
#define LINE(x1, x2, c)
#define CAPSULE(x, c)
#endif

bool UExtendedCharacterMovementComp::FSavedMove_ExtendedMoveCharacter::CanCombineWith(
	const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	FSavedMove_ExtendedMoveCharacter* NewExtendedMove = static_cast<FSavedMove_ExtendedMoveCharacter*>(NewMove.Get());

	if (Saved_bWantsToSprint != NewExtendedMove->Saved_bWantsToSprint)
	{
		return false;
	}

	if (Saved_bWantsToWalk != NewExtendedMove->Saved_bWantsToWalk)
	{
		return false;
	}

	if (Saved_bWantsToDash != NewExtendedMove->Saved_bWantsToDash)
	{
		return false;
	}

	if (Saved_bWallrunIsRight != NewExtendedMove->Saved_bWallrunIsRight)
	{
		return false;
	}


	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UExtendedCharacterMovementComp::FSavedMove_ExtendedMoveCharacter::Clear()
{
	FSavedMove_Character::Clear();

	Saved_bWantsToSprint = 0;
	Saved_bWantsToWalk = 0;
	Saved_bWantsToDash = 0;

	Saved_bWantsToProne = 0;
	Saved_bPrevWantsToCrouch = 0;

	Saved_bPressedExtendedJump = 0;
	Saved_bHadAnimRootMotion = 0;
	Saved_bTransitionFinished = 0;

	Saved_bWallrunIsRight = 0;

}

uint8 UExtendedCharacterMovementComp::FSavedMove_ExtendedMoveCharacter::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (Saved_bWantsToSprint) Result |= FLAG_Sprint;
	if (Saved_bWantsToWalk) Result |= FLAG_Walk;
	if (Saved_bWantsToDash) Result |= FLAG_Dash;
	if (Saved_bPressedExtendedJump) Result |= FLAG_JumpPressed;

	return Result;
}

void UExtendedCharacterMovementComp::FSavedMove_ExtendedMoveCharacter::SetMoveFor(
	ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	UExtendedCharacterMovementComp* CharacterMovement = Cast<UExtendedCharacterMovementComp>(C->GetCharacterMovement());

	Saved_bWantsToSprint = CharacterMovement->Safe_bWantsToSprint;
	Saved_bWantsToWalk = CharacterMovement->Safe_bWantsToWalk;
	Saved_bPrevWantsToCrouch = CharacterMovement->Safe_bPrevWantsToCrouch;

	Saved_bWantsToProne = CharacterMovement->Safe_bWantsToProne;
	Saved_bWantsToDash = CharacterMovement->Safe_bWantsToDash;

	Saved_bPressedExtendedJump = CharacterMovement->ExtendedCharacterOwner->bPressedExtendedJump;

	Saved_bHadAnimRootMotion = CharacterMovement->Safe_bHadAnimRootMotion;
	Saved_bTransitionFinished = CharacterMovement->Safe_bTransitionFinished;

	Saved_bWallrunIsRight = CharacterMovement->Safe_bWallrunIsRight;

}

void UExtendedCharacterMovementComp::FSavedMove_ExtendedMoveCharacter::PrepMoveFor(ACharacter* C)
{
	FSavedMove_Character::PrepMoveFor(C);

	UExtendedCharacterMovementComp* CharacterMovement = Cast<UExtendedCharacterMovementComp>(C->GetCharacterMovement());

	CharacterMovement->Safe_bWantsToSprint = Saved_bWantsToSprint;
	CharacterMovement->Safe_bWantsToWalk = Saved_bWantsToWalk;
	CharacterMovement->Safe_bPrevWantsToCrouch = Saved_bPrevWantsToCrouch;

	CharacterMovement->Safe_bWantsToProne = Saved_bWantsToProne;
	CharacterMovement->Safe_bWantsToDash = Saved_bWantsToDash;

	CharacterMovement->ExtendedCharacterOwner->bPressedExtendedJump = Saved_bPressedExtendedJump;

	CharacterMovement->Safe_bHadAnimRootMotion = Saved_bHadAnimRootMotion;
	CharacterMovement->Safe_bTransitionFinished = Saved_bTransitionFinished;

	CharacterMovement->Safe_bWallrunIsRight = Saved_bWallrunIsRight;

}

UExtendedCharacterMovementComp::FNetworkPredictionData_Client_ExtendedCharacter::
FNetworkPredictionData_Client_ExtendedCharacter(const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement)
{

}

FSavedMovePtr UExtendedCharacterMovementComp::FNetworkPredictionData_Client_ExtendedCharacter::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_ExtendedMoveCharacter());
}

FNetworkPredictionData_Client* UExtendedCharacterMovementComp::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr)

		if (ClientPredictionData == nullptr)
		{
			UExtendedCharacterMovementComp* MutableThis = const_cast<UExtendedCharacterMovementComp*>(this);

			MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_ExtendedCharacter(*this);
			MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
			MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
		}
	return ClientPredictionData;
}

void UExtendedCharacterMovementComp::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	Safe_bWantsToSprint = (Flags & FSavedMove_ExtendedMoveCharacter::FLAG_Sprint) != 0;
	Safe_bWantsToWalk = (Flags & FSavedMove_ExtendedMoveCharacter::FLAG_Walk) != 0;
	Safe_bWantsToDash = (Flags & FSavedMove_ExtendedMoveCharacter::FLAG_Dash) != 0;
}

void UExtendedCharacterMovementComp::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	Safe_bPrevWantsToCrouch = bWantsToCrouch;
}

bool UExtendedCharacterMovementComp::IsMovingOnGround() const
{
	return Super::IsMovingOnGround() || IsCustomMovementMode(CMOVE_Slide) || IsCustomMovementMode(CMOVE_Prone);
}

bool UExtendedCharacterMovementComp::CanCrouchInCurrentState() const
{
	return Super::CanCrouchInCurrentState() && IsMovingOnGround();
}

float UExtendedCharacterMovementComp::GetMaxSpeed() const
{
	Super::GetMaxSpeed();
	if (MovementMode != MOVE_Custom)
	{
		if (IsMovementMode(MOVE_Walking) && Safe_bWantsToSprint && !IsCrouching())
		{
			return MaxSprintSpeed;
		}
		else if (IsMovementMode(MOVE_Walking) && Safe_bWantsToWalk && !IsCrouching())
		{
			return MaxWalkingSpeed;
		}
		else if (IsCrouching())
		{
			return MaxWalkSpeedCrouched;
		}
		else
		{
			return MaxWalkSpeed;
		}
	}

	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		return Slide_MaxSpeed;
		break;
	case CMOVE_Prone:
		return ProneMaxSpeed;
		break;
	case CMOVE_Wallrun:
		return MaxWallrunSpeed;
		break;
	case CMOVE_Hang:
		return 0.f;
		break;
	case CMOVE_Climb:
		return MaxClimbSpeed;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"))
			return -1.f;
	}
}

float UExtendedCharacterMovementComp::GetMaxBrakingDeceleration() const
{
	if (MovementMode != MOVE_Custom) return Super::GetMaxBrakingDeceleration();

	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		return BrakingDecelerationSliding;
		break;
	case CMOVE_Prone:
		return BrakingDecelerationProning;
		break;
	case CMOVE_Wallrun:
		return 0.f;
		break;
	case CMOVE_Hang:
		return 0.f;
		break;
	case CMOVE_Climb:
		return BrakingDecelerationClimbing;
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"))
			return -1.f;
	}
}

bool UExtendedCharacterMovementComp::CanAttemptJump() const
{
	return Super::CanAttemptJump() || IsWallrunning() || IsHanging() || IsClimbing();
}

bool UExtendedCharacterMovementComp::DoJump(bool bReplayingMoves, float DeltaTime)
{
	bool bWasWallrunning = IsWallrunning();
	bool bWasOnWall = IsHanging() || IsClimbing();
	bool bWasSliding = IsSliding();
	if (Super::DoJump(bReplayingMoves, DeltaTime))
	{
		FVector WorldForwardVector = CharacterOwner->GetActorRotation().RotateVector(FVector::ForwardVector);

		if (bWasWallrunning)
		{
			FVector Start = UpdatedComponent->GetComponentLocation();
			FVector CastDelta = UpdatedComponent->GetRightVector() * CapR() * 2;
			FVector End = Safe_bWallrunIsRight ? Start + CastDelta : Start - CastDelta;
			auto Params = ExtendedCharacterOwner->GetIgnoreCharacterParams();
			CharacterOwner->JumpCurrentCount = 0;

			FHitResult WallHit;
			GetWorld()->LineTraceSingleByProfile(WallHit, Start, End, "BlockAll", Params);
			Velocity += WallHit.Normal * WallJumpOffForce;
		}
		else if (bWasSliding)
		{
			if (!bReplayingMoves)
			{
				Velocity += FVector::UpVector * SlideJumpForce * SlideJumpUPModifier;
				Velocity += WorldForwardVector * SlideJumpForce * SlideJumpFORWARDModifier;
				Velocity += Acceleration.GetSafeNormal2D() * SlideJumpForce * SlideJumpACCELERATIONModifier;
			}
		}
		else if (bWasOnWall)
		{
			if (!bReplayingMoves)
			{
				//CharacterOwner->PlayAnimMontage(WallJumpMontage);
			}
			Velocity += FVector::UpVector * WallJumpForce * 0.5f;
			Velocity += Acceleration.GetSafeNormal2D() * WallJumpForce * 0.5f;
		}
		else	// Modifying Jump to make it forward
		{
			if (!bReplayingMoves)
			{
				Velocity += WorldForwardVector * JumpForwardForce * JumpForwardModifier;
				Velocity += Acceleration.GetSafeNormal2D() * JumpForwardForce * JumpForwardModifier;
			}
		}
		return true;
	}
	return false;
}

void UExtendedCharacterMovementComp::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	//Slide
	if (MovementMode == MOVE_Walking && !bWantsToCrouch && Safe_bPrevWantsToCrouch)
	{
		if (CanSlide())
		{
			SetMovementMode(MOVE_Custom, CMOVE_Slide);
		}
	}
	else if (IsCustomMovementMode(CMOVE_Slide) && !bWantsToCrouch)
	{
		SetMovementMode(MOVE_Walking);
	}
	/*else if (IsFalling() && bWantsToCrouch)
	{
		if (TryClimb()) bWantsToCrouch = false;
	}*/
	else if ((IsClimbing() || IsHanging()) && bWantsToCrouch)
	{
		SetMovementMode(MOVE_Falling);
		bWantsToCrouch = false;
	}


	//Prone
	if (Safe_bWantsToProne)
	{
		if (CanProne())
		{
			SetMovementMode(MOVE_Custom, CMOVE_Prone);
			if (!CharacterOwner->HasAuthority())
			{
				SLOG("START PRONE")
				Server_EnterProne();
			}
		}
		Safe_bWantsToProne = false;
	}
	if (IsCustomMovementMode(CMOVE_Prone) && !bWantsToCrouch && !IsFalling())
	{
		SetMovementMode(MOVE_Walking);
	}


	//Dash
	bool bAuthProxy = CharacterOwner->HasAuthority() && !CharacterOwner->IsLocallyControlled();
	if (Safe_bWantsToDash && CanDash())
	{
		if (!bAuthProxy || GetWorld()->GetTimeSeconds() - DashStartTime > AuthDashCooldownDuration)
		{
			PerformDash(false);
			Safe_bWantsToDash = false;
			Proxy_bDashStart = !Proxy_bDashStart;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Client tried to cheat!"))
		}
	}


	//Try Mantle / Climb
	if (ExtendedCharacterOwner->bPressedExtendedJump)
	{
		if (TryMantle())
		{
			SLOG("START MANTLE")
			ExtendedCharacterOwner->StopJumping();
		}
		/*else if (TryClimb())
		{
			SLOG("START CLIMB")
			ExtendedCharacterOwner->StopJumping();
		}*/
		else
		{
			SLOG("No mantle/climb - regular jump!")
			ExtendedCharacterOwner->bPressedExtendedJump = false;
			CharacterOwner->bPressedJump = true;
			CharacterOwner->CheckJumpInput(DeltaSeconds);
		}
	}

	//Transition Mantle
	if (Safe_bTransitionFinished)
	{
		SLOG("Transition finished")
		UE_LOG(LogTemp, Warning, TEXT("FINISHED ROOT MOTION"))

		if (IsValid(TransitionQueuedMontage))
		{
			SetMovementMode(MOVE_Flying);
			CharacterOwner->PlayAnimMontage(TransitionQueuedMontage, TransitionQueuedMontageSpeed);
			TransitionQueuedMontageSpeed = 0.f;
			TransitionQueuedMontage = nullptr;
		}
		else
		{
			SetMovementMode(MOVE_Walking);
		}
		Safe_bTransitionFinished = false;
	}

	// LAST UPDATED REALIZATION (FOR SOME REASONS NEVER GOING TO FINISHING MANTLE) TODO: FIX IT!!!
	/*if (Safe_bTransitionFinished)
	{
		SLOG("Transition Finished")
			UE_LOG(LogTemp, Warning, TEXT("FINISHED RM"))

			if (TransitionName == "Mantle")
			{
				if (IsValid(TransitionQueuedMontage))
				{
					SetMovementMode(MOVE_Flying);
					CharacterOwner->PlayAnimMontage(TransitionQueuedMontage, TransitionQueuedMontageSpeed);
					TransitionQueuedMontageSpeed = 0.f;
					TransitionQueuedMontage = nullptr;
				}
				else
				{
					SetMovementMode(MOVE_Walking);
				}
			}
			else if (TransitionName == "Hang")
			{
				SetMovementMode(MOVE_Custom, CMOVE_Hang);
				Velocity = FVector::ZeroVector;
			}

		TransitionName = "";
		Safe_bTransitionFinished = false;
	}*/

	// Wallrun
	if (IsFalling())
	{
		TryWallrun();
	}

	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UExtendedCharacterMovementComp::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateAfterMovement(DeltaSeconds);

	if (!HasAnimRootMotion() && Safe_bHadAnimRootMotion && IsMovementMode(MOVE_Flying))
	{
		SLOG("Ending Anim Root Motion")
			SetMovementMode(MOVE_Walking);
	}

	if (GetRootMotionSourceByID(TransitionRMS_ID) &&
		GetRootMotionSourceByID(TransitionRMS_ID)->Status.HasFlag(ERootMotionSourceStatusFlags::Finished))
	{
		RemoveRootMotionSourceByID(TransitionRMS_ID);
		Safe_bTransitionFinished = true;
	}

	Safe_bHadAnimRootMotion = HasAnimRootMotion();
}

void UExtendedCharacterMovementComp::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		PhysSlide(deltaTime, Iterations);
		break;
	case CMOVE_Prone:
		PhysProne(deltaTime, Iterations);
		break;
	case CMOVE_Wallrun:
		PhysWallrun(deltaTime, Iterations);
		break;
	case CMOVE_Hang:
		break;
	case CMOVE_Climb:
		return PhysClimb(deltaTime, Iterations);
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"))
	}
}

void UExtendedCharacterMovementComp::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == CMOVE_Slide) ExitSlide();
	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == CMOVE_Prone) ExitProne();

	if (IsCustomMovementMode(CMOVE_Slide)) EnterSlide(PreviousMovementMode, (ECustomMovementMode)PreviousCustomMode);
	if (IsCustomMovementMode(CMOVE_Prone)) EnterProne(PreviousMovementMode, (ECustomMovementMode)PreviousCustomMode);

	if (IsWallrunning() && GetOwnerRole() == ROLE_SimulatedProxy)
	{
		FVector Start = UpdatedComponent->GetComponentLocation();
		FVector End = Start + UpdatedComponent->GetRightVector();
		auto Params = ExtendedCharacterOwner->GetIgnoreCharacterParams();
		FHitResult WallHit;
		Safe_bWallrunIsRight = GetWorld()->LineTraceSingleByProfile(WallHit, Start, End, "BlockAll", Params);
	}
}






//CONSTRUCTOR
UExtendedCharacterMovementComp::UExtendedCharacterMovementComp()
{
	NavAgentProps.bCanCrouch = true;
	bOrientRotationToMovement = false;
}

void UExtendedCharacterMovementComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UExtendedCharacterMovementComp, Proxy_bDashStart, COND_SkipOwner);

	DOREPLIFETIME_CONDITION(UExtendedCharacterMovementComp, Proxy_bShortMantle, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(UExtendedCharacterMovementComp, Proxy_bTallMantle, COND_SkipOwner);
}

void UExtendedCharacterMovementComp::InitializeComponent()
{
	Super::InitializeComponent();

	ExtendedCharacterOwner = Cast<AExtendedCharacter>(GetOwner());
}

void UExtendedCharacterMovementComp::Server_EnterProne_Implementation()
{
	Safe_bWantsToProne = true;
	//CharacterOwner->bUseControllerRotationYaw = false;
}

void UExtendedCharacterMovementComp::EnterProne(EMovementMode PrevMode, ECustomMovementMode PrevCustomMode)
{
	bWantsToCrouch = true;

	if (PrevMode == MOVE_Custom && PrevCustomMode == CMOVE_Slide)
	{
		Velocity += Velocity.GetSafeNormal2D() * ProneSlideEnterImpulse;
	}
	
	FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, true, NULL);
}
void UExtendedCharacterMovementComp::ExitProne()
{
	bWantsToCrouch = false;
	//bOrientRotationToMovement = false;
	//CharacterOwner->bUseControllerRotationYaw = true;
}
bool UExtendedCharacterMovementComp::CanProne() const
{
	return	IsCustomMovementMode(CMOVE_Prone) || 
			(IsMovementMode(MOVE_Walking) && IsCrouching());
}
void UExtendedCharacterMovementComp::PhysProne(float deltaTime, int32 Iteration)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	if (!CharacterOwner || (!CharacterOwner->Controller &&
		!bRunPhysicsWithNoController &&
		!HasAnimRootMotion() &&
		!CurrentRootMotion.HasOverrideVelocity() &&
		(CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}


	bJustTeleported = false;
	bool bCheckedFall = false;
	bool bTriedLedgeMove = false;
	float remainingTime = deltaTime;

	//Perform move
	while ((remainingTime >= MIN_TICK_TIME) &&
		(Iteration < MaxSimulationIterations) &&
		CharacterOwner &&
		(CharacterOwner->Controller ||
			bRunPhysicsWithNoController ||
			(CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)))
	{
		Iteration++;
		bJustTeleported = false;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iteration);
		remainingTime -= timeTick;

		//Save current values
		UPrimitiveComponent* const OldBase = GetMovementBase();
		const FVector PreviousBaseLocation = (OldBase != NULL) ? OldBase->GetComponentLocation() : FVector::ZeroVector;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FFindFloorResult OldFloor = CurrentFloor;

		//Ensure velocity is horizontal
		MaintainHorizontalGroundVelocity();
		const FVector OldVelocity = Velocity;
		Acceleration.Z = 0.f;


		//Apply acceleration
		CalcVelocity(timeTick, GroundFriction, false, GetMaxBrakingDeceleration());


		//Compute move parameters
		const FVector MoveVelocity = Velocity;
		const FVector Delta = timeTick * MoveVelocity;
		const bool bZeroDelta = Delta.IsNearlyZero();
		FStepDownResult StepDownResult;


		if (bZeroDelta)
		{
			remainingTime = 0.f;
		}
		else
		{
			//Actual move capsule
			MoveAlongFloor(MoveVelocity, timeTick, &StepDownResult);

			if (IsFalling())
			{
				//Pawn decide to jump up
				const float DesiredDist = Delta.Size();
				if (DesiredDist > KINDA_SMALL_NUMBER)
				{
					const float ActualDist = (UpdatedComponent->GetComponentLocation() - OldLocation).Size2D();
					remainingTime += timeTick * (1.f - FMath::Min(1.f, ActualDist / DesiredDist));
				}
				StartNewPhysics(remainingTime, Iteration);
				return;
			}
			else if (IsSwimming()) // just entered water
			{
				StartSwimming(OldLocation, OldVelocity, timeTick, remainingTime, Iteration);
				return;
			}
			//added more statements by adding them the project for checks
		}


		//Update floor
		//StepUp might have already done it for us
		if (StepDownResult.bComputedFloor)
		{
			CurrentFloor = StepDownResult.FloorResult;
		}
		else
		{
			FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, bZeroDelta, NULL);
		}


		//check for ledges here
		const bool bCheckLedges = !CanWalkOffLedges();
		if (bCheckLedges && !CurrentFloor.IsWalkableFloor())
		{
			//calculate possible alternate movement
			const FVector GravDir = FVector(0.f, 0.f, -1.f);
			const FVector NewDelta = bTriedLedgeMove ? FVector::ZeroVector : GetLedgeMove(OldLocation, Delta, OldFloor);
			if (!NewDelta.IsZero())
			{
				//first revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, false);

				//avoid repeated ledge move if the first one fails
				bTriedLedgeMove = true;


				//Try new movement direction
				Velocity = NewDelta / timeTick;
				remainingTime += timeTick;
				continue;
			}
			else
			{
				//See if it is OK to jump
				// @todo collision : only thing that can be problem is that OldBase has world collision on
				bool bMustJump = bZeroDelta || (OldBase == NULL ||
					(!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));

				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iteration, bMustJump))
				{
					return;
				}
				bCheckedFall = true;

				//revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, true);
				remainingTime = 0.f;
				break;
			}
		}
		else
		{
			//Validate the floor check
			if (CurrentFloor.IsWalkableFloor())
			{
				AdjustFloorHeight();
				SetBase(CurrentFloor.HitResult.Component.Get(), CurrentFloor.HitResult.BoneName);
			}
			else if (CurrentFloor.HitResult.bStartPenetrating && remainingTime <= 0.f)
			{
				//The floor check failed because it started in penetration
				//We do not want to try to move downward because the downward sweep failed, 
				// rather we'd like to try to pop out of the floor
				FHitResult Hit(CurrentFloor.HitResult);
				Hit.TraceEnd = Hit.TraceStart + FVector(0.f, 0.f, MAX_FLOOR_DIST);
				const FVector RequestedAdjustment = GetPenetrationAdjustment(Hit);

				ResolvePenetration(RequestedAdjustment, Hit, UpdatedComponent->GetComponentQuat());
				bForceNextFloorCheck = true;
			}


			//check if just entered water
			if (IsSwimming())
			{
				StartSwimming(OldLocation, Velocity, timeTick, remainingTime, Iteration);
				return;
			}

			//See if we need to start falling
			if (!CurrentFloor.IsWalkableFloor() && !CurrentFloor.HitResult.bStartPenetrating)
			{
				const bool bMustJump = bJustTeleported ||
					bZeroDelta ||
					(OldBase == NULL ||
						(!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iteration, bMustJump))
				{
					return;
				}
				bCheckedFall = true;
			}
		}


		//Allow overlap events and such to change physics state and velocity
		if (IsMovingOnGround())
		{
			//Make velocity reflected actual move
			if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && timeTick >= MIN_TICK_TIME)
			{
				// TODO root motionSource: allow to happen during partial override Velocity, but only set allowed axes?
				Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick;
				MaintainHorizontalGroundVelocity();
			}
		}

		// if we didn't move at all this iteration then abort (since future iterations will also be stuck)
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			remainingTime = 0.f;
			break;
		}


		if (IsMovingOnGround())
		{
			MaintainHorizontalGroundVelocity();
		}
	}
}



void UExtendedCharacterMovementComp::EnterSlide(EMovementMode PrevMode, ECustomMovementMode PrevCustomMode)
{
	bWantsToCrouch = true;
	if (IsCustomMovementMode(CMOVE_Slide))
	{
		Velocity += Velocity.GetSafeNormal2D();
	}
	else
	{
		Velocity += Velocity.GetSafeNormal2D() * Slide_EnterImpulse;
	}

	FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, true, NULL);
	SetMovementMode(MOVE_Custom, CMOVE_Slide);
}
void UExtendedCharacterMovementComp::ExitSlide()
{
	bWantsToCrouch = false;
}
bool UExtendedCharacterMovementComp::CanSlide() const
{
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = Start + CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.5f * FVector::DownVector;
	FName ProfileName = TEXT("BlockAll");
	bool bValidSurface = GetWorld()->LineTraceTestByProfile(Start, End, ProfileName, ExtendedCharacterOwner->GetIgnoreCharacterParams());
	bool bEnoughSpeed = Velocity.SizeSquared() > pow(Slide_MinSpeed, 2);

	return bValidSurface &&
		bEnoughSpeed &&
		IsWalking() &&
		!IsFalling() &&
		!IsCustomMovementMode(CMOVE_Prone);
}
void UExtendedCharacterMovementComp::PhysSlide(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	if (!CanSlide())
	{
		SLOG(FString::Printf(TEXT("EXITING SLIDE!")))
		SetMovementMode(MOVE_Walking);
		StartNewPhysics(deltaTime, Iterations);
		return;
	}

	bJustTeleported = false;
	bool bCheckedFall = false;
	bool bTriedLedgeMove = false;
	float remainingTime = deltaTime;

	// Perform the move
	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)))
	{
		Iterations++;
		bJustTeleported = false;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		// Save current values
		UPrimitiveComponent* const OldBase = GetMovementBase();
		const FVector PreviousBaseLocation = (OldBase != NULL) ? OldBase->GetComponentLocation() : FVector::ZeroVector;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FFindFloorResult OldFloor = CurrentFloor;

		// Ensure velocity is horizontal.
		MaintainHorizontalGroundVelocity();
		const FVector OldVelocity = Velocity;

		FVector SlopeForce = CurrentFloor.HitResult.Normal;
		SlopeForce.Z = 0.f;
		Velocity += SlopeForce * Slide_GravityForce * deltaTime;

		Acceleration = Acceleration.ProjectOnTo(UpdatedComponent->GetForwardVector().GetSafeNormal2D());

		// Apply acceleration
		CalcVelocity(timeTick, GroundFriction * SlideFrictionFactor, false, GetMaxBrakingDeceleration());

		// Compute move parameters
		const FVector MoveVelocity = Velocity + CharacterOwner->GetActorForwardVector();
		const FVector Delta = timeTick * MoveVelocity;
		const bool bZeroDelta = Delta.IsNearlyZero();
		FStepDownResult StepDownResult;
		bool bFloorWalkable = CurrentFloor.IsWalkableFloor();

		if (bZeroDelta)
		{
			remainingTime = 0.f;
		}
		else
		{
			// try to move forward
			MoveAlongFloor(MoveVelocity, timeTick, &StepDownResult);

			if (IsFalling())
			{
				// pawn decided to jump up
				const float DesiredDist = Delta.Size();
				if (DesiredDist > KINDA_SMALL_NUMBER)
				{
					const float ActualDist = (UpdatedComponent->GetComponentLocation() - OldLocation).Size2D();
					remainingTime += timeTick * (1.f - FMath::Min(1.f, ActualDist / DesiredDist));
				}
				StartNewPhysics(remainingTime, Iterations);
				return;
			}
			else if (IsSwimming()) //just entered water
			{
				StartSwimming(OldLocation, OldVelocity, timeTick, remainingTime, Iterations);
				return;
			}
		}

		// Update floor.
		// StepUp might have already done it for us.
		if (StepDownResult.bComputedFloor)
		{
			CurrentFloor = StepDownResult.FloorResult;
		}
		else
		{
			FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, bZeroDelta, NULL);
		}

		// Handle rotation explicitly during slide
		if (CharacterOwner)
		{
			// Lock the character's rotation to the direction of movement
			FRotator SlideRotation = Velocity.Rotation();
			SlideRotation.Pitch = 0.f; // Neutralize pitch
			SlideRotation.Roll = 0.f;  // Neutralize roll
			CharacterOwner->SetActorRotation(SlideRotation);
		}

		// check for ledges here
		const bool bCheckLedges = !CanWalkOffLedges();
		if (bCheckLedges && !CurrentFloor.IsWalkableFloor())
		{
			// calculate possible alternate movement
			const FVector GravDir = FVector(0.f, 0.f, -1.f);
			const FVector NewDelta = bTriedLedgeMove ? FVector::ZeroVector : GetLedgeMove(OldLocation, Delta, OldFloor);
			if (!NewDelta.IsZero())
			{
				// first revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, false);

				// avoid repeated ledge moves if the first one fails
				bTriedLedgeMove = true;

				// Try new movement direction
				Velocity = NewDelta / timeTick;
				remainingTime += timeTick;
				continue;
			}
			else
			{
				// see if it is OK to jump
				// @todo collision : only thing that can be problem is that oldbase has world collision on
				bool bMustJump = bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump))
				{
					return;
				}
				bCheckedFall = true;

				// revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, true);
				remainingTime = 0.f;
				break;
			}
		}
		else
		{
			// Validate the floor check
			if (CurrentFloor.IsWalkableFloor())
			{
				if (ShouldCatchAir(OldFloor, CurrentFloor))
				{
					HandleWalkingOffLedge(OldFloor.HitResult.ImpactNormal, OldFloor.HitResult.Normal, OldLocation, timeTick);
					if (IsMovingOnGround())
					{
						// If still walking, then fall. If not, assume the user set a different mode they want to keep.
						StartFalling(Iterations, remainingTime, timeTick, Delta, OldLocation);
					}
					return;
				}

				AdjustFloorHeight();
				SetBase(CurrentFloor.HitResult.Component.Get(), CurrentFloor.HitResult.BoneName);
			}
			else if (CurrentFloor.HitResult.bStartPenetrating && remainingTime <= 0.f)
			{
				// The floor check failed because it started in penetration
				// We do not want to try to move downward because the downward sweep failed, rather we'd like to try to pop out of the floor.
				FHitResult Hit(CurrentFloor.HitResult);
				Hit.TraceEnd = Hit.TraceStart + FVector(0.f, 0.f, MAX_FLOOR_DIST);
				const FVector RequestedAdjustment = GetPenetrationAdjustment(Hit);
				ResolvePenetration(RequestedAdjustment, Hit, UpdatedComponent->GetComponentQuat());
				bForceNextFloorCheck = true;
			}

			// check if just entered water
			if (IsSwimming())
			{
				StartSwimming(OldLocation, Velocity, timeTick, remainingTime, Iterations);
				return;
			}

			// See if we need to start falling.
			if (!CurrentFloor.IsWalkableFloor() && !CurrentFloor.HitResult.bStartPenetrating)
			{
				const bool bMustJump = bJustTeleported || bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump))
				{
					return;
				}
				bCheckedFall = true;
			}
		}

		// Allow overlap events and such to change physics state and velocity
		if (IsMovingOnGround() && bFloorWalkable)
		{
			// Make velocity reflect actual move
			if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && timeTick >= MIN_TICK_TIME)
			{
				// TODO-RootMotionSource: Allow this to happen during partial override Velocity, but only set allowed axes?
				Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick;
				MaintainHorizontalGroundVelocity();
			}
		}

		// If we didn't move at all this iteration then abort (since future iterations will also be stuck).
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			remainingTime = 0.f;
			break;
		}
	}


	FHitResult Hit;

	FRotator RotationFromPlayer = FRotator(0.0, CharacterOwner->GetControlRotation().Yaw, 0.0);

	//FQuat NewRotation =	FRotationMatrix::MakeFromXZ(Velocity.GetSafeNormal2D(), FVector::UpVector).ToQuat();
	SafeMoveUpdatedComponent(FVector::ZeroVector, RotationFromPlayer, false, Hit);
}
bool UExtendedCharacterMovementComp::GetSlideSurface(FHitResult& Hit) const
{
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = Start + CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.f * FVector::DownVector;

	FName ProfileName = TEXT("BlockAll");
	return GetWorld()->LineTraceSingleByProfile(Hit, Start, End, ProfileName, ExtendedCharacterOwner->GetIgnoreCharacterParams());
}


void UExtendedCharacterMovementComp::OnDashCooldownFinished()
{
	Safe_bWantsToDash = true;
}
bool UExtendedCharacterMovementComp::CanDash() const
{
	//IsWalking() &&
	return IsWalking() && !IsCrouching() || IsFalling();
}
void UExtendedCharacterMovementComp::PerformDash(bool bDolphinDive)
{
	DashStartTime = GetWorld()->GetTimeSeconds();

	FVector DashDirection = (Acceleration.IsNearlyZero() ? UpdatedComponent->GetForwardVector() : Acceleration).GetSafeNormal2D();
	
	// modify Z axis to use in dolphin dive
	FVector upModifier = bDolphinDive ? (FVector::UpVector * DolphinDiveZModifier) : (FVector::UpVector * 0.1f);
	float impulse = bDolphinDive ? DolphinDiveImpulse : DashImpulse;
	Velocity = impulse * (DashDirection + upModifier);

	FQuat NewRotation = FRotationMatrix::MakeFromXZ(DashDirection, FVector::UpVector).ToQuat();
	FHitResult Hit;
	SafeMoveUpdatedComponent(FVector::ZeroVector, NewRotation, false, Hit);

	SetMovementMode(MOVE_Falling);

	DashStartDelegate.Broadcast();
}
void UExtendedCharacterMovementComp::OnRep_DashStart()
{
	DashStartDelegate.Broadcast();
}

void UExtendedCharacterMovementComp::OnRep_ShortMantle()
{
	CharacterOwner->PlayAnimMontage(ProxyShortMantleMontage);
}
void UExtendedCharacterMovementComp::OnRep_TallMantle()
{
	CharacterOwner->PlayAnimMontage(ProxyTallMantleMontage);
}
bool UExtendedCharacterMovementComp::TryMantle()
{
	//bWantsToCrouch = false;

	if (!IsMovementMode(MOVE_Walking) && !IsCrouching() && !IsMovementMode(MOVE_Falling) && !IsCustomMovementMode(CMOVE_Climb)) return false;
	// Helper Variables
	FVector BaseLoc = UpdatedComponent->GetComponentLocation() + FVector::DownVector * CapHH();
	FVector Fwd = UpdatedComponent->GetForwardVector().GetSafeNormal2D();
	auto Params = ExtendedCharacterOwner->GetIgnoreCharacterParams();
	float MaxHeight = CapHH() * 2 + MantleReachHeight;
	float CosMMWSA = FMath::Cos(FMath::DegreesToRadians(MantleMinWallSteepnessAngle));
	float CosMMSA = FMath::Cos(FMath::DegreesToRadians(MantleMaxSurfaceAngle));
	float CosMMAA = FMath::Cos(FMath::DegreesToRadians(MantleMaxAlignmentAngle));

	//Check front face
	FHitResult FrontHit;
	float CheckDistance = FMath::Clamp(Velocity | Fwd, CapR() + 30, MantleMaxDistance);
	FVector FrontStart = BaseLoc + FVector::UpVector * (MaxStepHeight - 1);

	// Increment nums of iterations if needs, if i < 6 - then FrontStart / 5.
	for (int i = 0; i < 6; i++)
	{
		LINE(FrontStart, FrontStart + Fwd * CheckDistance, FColor::Red);

		if (GetWorld()->LineTraceSingleByProfile(FrontHit, FrontStart, FrontStart + Fwd * CheckDistance, "BlockAll", Params)) break;
		FrontStart += FVector::UpVector * (2.f * CapHH() - (MaxStepHeight - 1)) / 5;
	}
	if (!FrontHit.IsValidBlockingHit()) return false;
	//Technically "FrontHit.Normal | FVector::UpVector"  returns dotProduct
	float CosWallSteepnessAngle = FrontHit.Normal | FVector::UpVector;
	if (FMath::Abs(CosWallSteepnessAngle) > CosMMWSA || (Fwd | -FrontHit.Normal) < CosMMAA) return false;
	POINT(FrontHit.Location, FColor::Red);


	// Check Height
	TArray<FHitResult> HeightHits;
	FHitResult SurfaceHit;
	FVector WallUp = FVector::VectorPlaneProject(FVector::UpVector, FrontHit.Normal).GetSafeNormal();
	float WallCos = FVector::UpVector | FrontHit.Normal;
	float WallSin = FMath::Sqrt(1 - WallCos * WallCos);
	FVector TraceStart = FrontHit.Location + Fwd + WallUp * (MaxHeight - (MaxStepHeight - 1)) / WallSin;

	LINE(TraceStart, FrontHit.Location + Fwd, FColor::Orange)


	if (!GetWorld()->LineTraceMultiByProfile(HeightHits, TraceStart, FrontHit.Location + Fwd, "BlockAll", Params)) return false;
	for (const FHitResult& Hit : HeightHits)
	{
		if (Hit.IsValidBlockingHit())
		{
			SurfaceHit = Hit;
			break;
		}
	}
	if (!SurfaceHit.IsValidBlockingHit() || (SurfaceHit.Normal | FVector::UpVector) < CosMMSA) return false;
	float Height = (SurfaceHit.Location - BaseLoc) | FVector::UpVector;

	SLOG(FString::Printf(TEXT("Height: %f"), Height))
	POINT(SurfaceHit.Location, FColor::Blue);


	if (Height > MaxHeight) return false;


	// Check Clearance
	float SurfaceCos = FVector::UpVector | SurfaceHit.Normal;
	float SurfaceSin = FMath::Sqrt(1 - SurfaceCos * SurfaceCos);
	FVector ClearCapLoc = SurfaceHit.Location + Fwd * CapR() + FVector::UpVector * (CapHH() + 1 + CapR() * 2 * SurfaceSin);
	FCollisionShape CapShape = FCollisionShape::MakeCapsule(CapR(), CapHH());
	if (GetWorld()->OverlapAnyTestByProfile(ClearCapLoc, FQuat::Identity, "BlockAll", CapShape, Params))
	{
		CAPSULE(ClearCapLoc, FColor::Red)
		return false;
	}
	else
	{
		CAPSULE(ClearCapLoc, FColor::Green)
	}
	SLOG("Can Mantle")


	// Mantle Selection
	FVector ShortMantleTarget = GetMantleStartLocation(FrontHit, SurfaceHit, false);
	FVector TallMantleTarget = GetMantleStartLocation(FrontHit, SurfaceHit, true);

	bool bTallMantle = false;
	if (IsMovementMode(MOVE_Walking) && Height > CapHH() * 2)
	{
		bTallMantle = true;
	}
	else if (IsMovementMode(MOVE_Falling) && (Velocity | FVector::UpVector) < 0)
	{
		if (!GetWorld()->OverlapAnyTestByProfile(TallMantleTarget, FQuat::Identity, "BlockAll", CapShape, Params))
			bTallMantle = true;
	}
	FVector TransitionTarget = bTallMantle ? TallMantleTarget : ShortMantleTarget;
	CAPSULE(TransitionTarget, FColor::Yellow)

	// Perform Transition to Mantle
	CAPSULE(UpdatedComponent->GetComponentLocation(), FColor::Red)


	float UpSpeed = Velocity | FVector::UpVector;
	float TransDistance = FVector::Dist(TransitionTarget, UpdatedComponent->GetComponentLocation());

	TransitionQueuedMontageSpeed = FMath::GetMappedRangeValueClamped(FVector2D(-500, 750), FVector2D(.9f, 1.2f), UpSpeed);
	TransitionRMS.Reset();
	TransitionRMS = MakeShared<FRootMotionSource_MoveToForce>();
	TransitionRMS->AccumulateMode = ERootMotionAccumulateMode::Override;

	TransitionRMS->Duration = FMath::Clamp(TransDistance / 500.f, .1f, .25f);
	SLOG(FString::Printf(TEXT("Duration: %f"), TransitionRMS->Duration))
	TransitionRMS->StartLocation = UpdatedComponent->GetComponentLocation();
	TransitionRMS->TargetLocation = TransitionTarget;

	// Apply Transition Root Motion Source
	Velocity = FVector::ZeroVector;
	SetMovementMode(MOVE_Flying);
	TransitionRMS_ID = ApplyRootMotionSource(TransitionRMS);
	//TransitionName = "Mantle";

	// Animations
	if (bTallMantle)
	{
		TransitionQueuedMontage = TallMantleMontage;

		CharacterOwner->PlayAnimMontage(TransitionTallMantleMontage, 1 / TransitionRMS->Duration);
		if (IsServer()) Proxy_bTallMantle = !Proxy_bTallMantle;
	}
	else
	{
		TransitionQueuedMontage = ShortMantleMontage;

		CharacterOwner->PlayAnimMontage(TransitionShortMantleMontage, 1 / TransitionRMS->Duration);
		if (IsServer()) Proxy_bShortMantle = !Proxy_bShortMantle;
	}

	return true;
}
FVector UExtendedCharacterMovementComp::GetMantleStartLocation(FHitResult FrontHit, FHitResult SurfaceHit, bool bTallMantle) const
{
	float CosWallSteepnessAngle = FrontHit.Normal | FVector::UpVector;
	float DownDistance = bTallMantle ? CapHH() * 2.f : MaxStepHeight - 1;
	FVector EdgeTangent = FVector::CrossProduct(SurfaceHit.Normal, FrontHit.Normal).GetSafeNormal();

	FVector MantleStart = SurfaceHit.Location;
	MantleStart += FrontHit.Normal.GetSafeNormal2D() * (2.f + CapR());
	MantleStart += UpdatedComponent->GetForwardVector().GetSafeNormal2D().ProjectOnTo(EdgeTangent) * CapR() * .3f;
	MantleStart += FVector::UpVector * CapHH();
	MantleStart += FVector::DownVector * DownDistance;
	MantleStart += FrontHit.Normal.GetSafeNormal2D() * CosWallSteepnessAngle * DownDistance;

	return MantleStart;
}
bool UExtendedCharacterMovementComp::IsServer() const
{
	return CharacterOwner->HasAuthority();
}
float UExtendedCharacterMovementComp::CapR() const
{
	return CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();
}
float UExtendedCharacterMovementComp::CapHH() const
{
	return CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}


bool UExtendedCharacterMovementComp::TryWallrun()
{
	if (!IsFalling()) return false;

	// similar check will be in the end of func, 
	// but we want to avoid unnecesary line trace checks if not eligible for wallrun
	if (Velocity.SizeSquared2D() < pow(MinWallrunSpeed, 2)) return false;

	/// if we fall to fast we don't want to wallrun. I disabled that.
	if (Velocity.Z < -MaxVerticalWallrunSpeed) return false;

	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector LeftEnd = Start - UpdatedComponent->GetRightVector() * CapR() * 2;
	FVector RightEnd = Start + UpdatedComponent->GetRightVector() * CapR() * 2;

	auto Params = ExtendedCharacterOwner->GetIgnoreCharacterParams();
	FHitResult FloorHit, WallHit;

	// Check Player Height
	if (GetWorld()->LineTraceSingleByProfile(
		FloorHit, Start, Start + FVector::DownVector * (CapHH() + MinWallrunHeight), "BlockAll", Params))
	{
		return false;
	}

	// Left Cast
	GetWorld()->LineTraceSingleByProfile(WallHit, Start, LeftEnd, "BlockAll", Params);
	if (WallHit.IsValidBlockingHit() && (Velocity | WallHit.Normal) < 0)
	{
		Safe_bWallrunIsRight = false;
	}

	// Right Cast
	else
	{
		GetWorld()->LineTraceSingleByProfile(WallHit, Start, RightEnd, "BlockAll", Params);
		if (WallHit.IsValidBlockingHit() && (Velocity | WallHit.Normal) < 0)
		{
			Safe_bWallrunIsRight = true;
		}
		else
		{
			return false;
		}
	}

	FVector ProjectedVelocity = FVector::VectorPlaneProject(Velocity, WallHit.Normal);
	if (ProjectedVelocity.SizeSquared2D() < pow(MinWallrunSpeed, 2)) return false;

	// Passed all conditions
	Velocity = ProjectedVelocity;
	Velocity.Z = FMath::Clamp(Velocity.Z, 0.f, MaxVerticalWallrunSpeed);
	SetMovementMode(MOVE_Custom, CMOVE_Wallrun);

	// start a timer to apply arch movement to wallrun
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_ArchWallrun,
		this,
		&UExtendedCharacterMovementComp::OnWallrunArchApplied,
		ArchFactorAppliedDelay);
	SLOG("Starting WallRun")
	return true;
}
void UExtendedCharacterMovementComp::PhysWallrun(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}
	if (!CharacterOwner || (!CharacterOwner->Controller &&
		!bRunPhysicsWithNoController &&
		!HasAnimRootMotion() &&
		!CurrentRootMotion.HasOverrideVelocity() &&
		(CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}

	bJustTeleported = false;
	float remainingTime = deltaTime;
	// Perform the move
	while ((remainingTime >= MIN_TICK_TIME) &&
		(Iterations < MaxSimulationIterations) &&
		CharacterOwner &&
		(CharacterOwner->Controller ||
			bRunPhysicsWithNoController ||
			(CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)))
	{
		Iterations++;
		bJustTeleported = false;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();

		FVector Start = UpdatedComponent->GetComponentLocation();
		FVector CastDelta = UpdatedComponent->GetRightVector() * CapR() * 2;
		FVector End = Safe_bWallrunIsRight ? Start + CastDelta : Start - CastDelta;
		auto Params = ExtendedCharacterOwner->GetIgnoreCharacterParams();

		float SinPullAwayAngle = FMath::Sin(FMath::DegreesToRadians(WallrunPullAwayAngle));
		FHitResult WallHit;
		GetWorld()->LineTraceSingleByProfile(WallHit, Start, End, "BlockAll", Params);
		bool bWantsToPullAway = WallHit.IsValidBlockingHit() &&
			!Acceleration.IsNearlyZero() && (Acceleration.GetSafeNormal() | WallHit.Normal) > SinPullAwayAngle;

		if (!WallHit.IsValidBlockingHit() || bWantsToPullAway)
		{
			SetMovementMode(MOVE_Falling);
			ResetWallrunArch();
			StartNewPhysics(remainingTime, Iterations);
			return;
		}


		// Clamp Acceleration
		Acceleration = FVector::VectorPlaneProject(Acceleration, WallHit.Normal);
		Acceleration.Z = 0.f;

		// Apply acceleration
		CalcVelocity(timeTick, 0.f, false, GetMaxBrakingDeceleration());
		Velocity = FVector::VectorPlaneProject(Velocity, WallHit.Normal);
		float TangentAccel = Acceleration.GetSafeNormal() | Velocity.GetSafeNormal2D();

		bool bVelUp = Velocity.Z > 0.f;
		Velocity.Z += GetGravityZ() * WallrunGravityScaleCurve->GetFloatValue(bVelUp ? 0.f : TangentAccel) * timeTick;

		if (bStartWallrunArch)
		{
			Velocity.Z -= ArchFactor * FMath::Abs(GetGravityZ()) * timeTick;
		}

		// Compute move parameters
		const FVector Delta = timeTick * Velocity; // dx = v * dt
		const bool bZeroDelta = Delta.IsNearlyZero();
		if (bZeroDelta)
		{
			remainingTime = 0.f;
		}
		else
		{
			FHitResult Hit;
			SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);
			FVector WallAttractionDelta = -WallHit.Normal * WallrunAttractionForce * timeTick;
			SafeMoveUpdatedComponent(WallAttractionDelta, UpdatedComponent->GetComponentQuat(), true, Hit);
		}

		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			remainingTime = 0.f;
			break;
		}
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick; // v = dx / dt
	}

	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector CastDelta = UpdatedComponent->GetRightVector() * CapR() * 2;
	FVector End = Safe_bWallrunIsRight ? Start + CastDelta : Start - CastDelta;
	auto Params = ExtendedCharacterOwner->GetIgnoreCharacterParams();
	FHitResult FloorHit, WallHit;
	GetWorld()->LineTraceSingleByProfile(WallHit, Start, End, "BlockAll", Params);
	GetWorld()->
		LineTraceSingleByProfile(FloorHit, Start, Start + FVector::DownVector * (CapHH() + MinWallrunHeight * .5f), "BlockAll", Params);
	if (FloorHit.IsValidBlockingHit() || 
		!WallHit.IsValidBlockingHit() || 
		Velocity.SizeSquared2D() < pow(MinWallrunSpeed, 2))
	{
		SetMovementMode(MOVE_Falling);
		ResetWallrunArch();
	}
}

void UExtendedCharacterMovementComp::OnWallrunArchApplied()
{
	bStartWallrunArch = true;
}

void UExtendedCharacterMovementComp::ResetWallrunArch()
{
	bStartWallrunArch = false;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ArchWallrun);
}

bool UExtendedCharacterMovementComp::TryHang()
{
	if (!IsMovementMode(MOVE_Falling)) return false;


	FHitResult WallHit;
	if (!GetWorld()->LineTraceSingleByProfile(
		WallHit, UpdatedComponent->GetComponentLocation(),
		UpdatedComponent->GetComponentLocation() +
		UpdatedComponent->GetForwardVector() * 300, "BlockAll", ExtendedCharacterOwner->GetIgnoreCharacterParams()))
		return false;

	TArray<FOverlapResult> OverlapResults;

	FVector ColLoc = UpdatedComponent->GetComponentLocation() +
		FVector::UpVector * CapHH() + UpdatedComponent->GetForwardVector() * CapR() * 3;

	auto ColBox = FCollisionShape::MakeBox(FVector(100, 100, 50));
	FQuat ColRot = FRotationMatrix::MakeFromXZ(WallHit.Normal, FVector::UpVector).ToQuat();

	if (!GetWorld()->OverlapMultiByChannel(
		OverlapResults, ColLoc, ColRot, ECC_WorldStatic, ColBox, ExtendedCharacterOwner->GetIgnoreCharacterParams()))
		return false;

	AActor* ClimbPoint = nullptr;

	float MaxHeight = -1e20;
	for (FOverlapResult Result : OverlapResults)
	{
		if (Result.GetActor()->ActorHasTag("Climb Point"))
		{
			float Height = Result.GetActor()->GetActorLocation().Z;
			if (Height > MaxHeight)
			{
				MaxHeight = Height;
				ClimbPoint = Result.GetActor();
			}
		}
	}
	if (!IsValid(ClimbPoint)) return false;

	FVector TargetLocation = ClimbPoint->GetActorLocation() + WallHit.Normal * CapR() * 1.01f + FVector::DownVector * CapHH();
	FQuat TargetRotation = FRotationMatrix::MakeFromXZ(-WallHit.Normal, FVector::UpVector).ToQuat();


	// Test if character can reach goal
	FTransform CurrentTransform = UpdatedComponent->GetComponentTransform();
	FHitResult Hit, ReturnHit;
	SafeMoveUpdatedComponent(TargetLocation - UpdatedComponent->GetComponentLocation(), UpdatedComponent->GetComponentQuat(), true, Hit);
	FVector ResultLocation = UpdatedComponent->GetComponentLocation();
	SafeMoveUpdatedComponent(CurrentTransform.GetLocation() - ResultLocation, TargetRotation, false, ReturnHit);
	if (!ResultLocation.Equals(TargetLocation)) return false;

	// Passed all conditions

	//bOrientRotationToMovement = false;

	// Perform Transition to Climb Point
	float UpSpeed = Velocity | FVector::UpVector;
	float TransDistance = FVector::Dist(TargetLocation, UpdatedComponent->GetComponentLocation());

	TransitionQueuedMontageSpeed = FMath::GetMappedRangeValueClamped(FVector2D(-500, 750), FVector2D(.9f, 1.2f), UpSpeed);
	TransitionRMS.Reset();
	TransitionRMS = MakeShared<FRootMotionSource_MoveToForce>();
	TransitionRMS->AccumulateMode = ERootMotionAccumulateMode::Override;

	TransitionRMS->Duration = FMath::Clamp(TransDistance / 500.f, .1f, .25f);
	TransitionRMS->StartLocation = UpdatedComponent->GetComponentLocation();
	TransitionRMS->TargetLocation = TargetLocation;

	// Apply Transition Root Motion Source
	Velocity = FVector::ZeroVector;
	SetMovementMode(MOVE_Flying);
	TransitionRMS_ID = ApplyRootMotionSource(TransitionRMS);

	// Animations
	TransitionQueuedMontage = nullptr;
	TransitionName = "Hang";
	CharacterOwner->PlayAnimMontage(TransitionHangMontage, 1 / TransitionRMS->Duration);

	return true;
}

bool UExtendedCharacterMovementComp::TryClimb()
{
	// WE WANT TO BE ABLE TO START CLIMB EVEN IF CHARACTER IS ON THE GROUND
	//if (!IsFalling()) return false;

	FHitResult SurfHit;
	FVector CapLoc = UpdatedComponent->GetComponentLocation();
	GetWorld()->LineTraceSingleByProfile(SurfHit, CapLoc, CapLoc + UpdatedComponent->GetForwardVector() * ClimbReachDistance, "BlockAll", ExtendedCharacterOwner->GetIgnoreCharacterParams());

	LINE(CapLoc, CapLoc + UpdatedComponent->GetForwardVector() * ClimbReachDistance, FColor::Red);

	if (!SurfHit.IsValidBlockingHit()) return false;

	FQuat NewRotation = FRotationMatrix::MakeFromXZ(-SurfHit.Normal, FVector::UpVector).ToQuat();
	SafeMoveUpdatedComponent(FVector::ZeroVector, NewRotation, false, SurfHit);

	SetMovementMode(MOVE_Custom, CMOVE_Climb);

	bOrientRotationToMovement = false;

	return true;
}
void UExtendedCharacterMovementComp::PhysClimb(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}
	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}

	// Perform the move
	bJustTeleported = false;
	Iterations++;
	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
	FHitResult SurfHit, FloorHit;
	GetWorld()->LineTraceSingleByProfile(SurfHit, OldLocation, OldLocation + UpdatedComponent->GetForwardVector() * ClimbReachDistance, "BlockAll", ExtendedCharacterOwner->GetIgnoreCharacterParams());
	GetWorld()->LineTraceSingleByProfile(FloorHit, OldLocation, OldLocation + FVector::DownVector * CapHH() * 1.2f, "BlockAll", ExtendedCharacterOwner->GetIgnoreCharacterParams());
	if (!SurfHit.IsValidBlockingHit() || GetLastInputVector() == FVector::ZeroVector) //modifying prevend character stay in climb idle		|| Velocity.Z == 0		FloorHit.IsValidBlockingHit()
	{
		SLOG("INSIDE TRYING TO LEAVE A CLIMB!")
			SetMovementMode(MOVE_Falling);
		StartNewPhysics(deltaTime, Iterations);
		return;
	}

	// Transform Acceleration
	Acceleration.Z = 0.f;
	Acceleration = Acceleration.RotateAngleAxis(90.f, -UpdatedComponent->GetRightVector());

	// Apply acceleration
	CalcVelocity(deltaTime, 0.f, false, GetMaxBrakingDeceleration());
	Velocity = FVector::VectorPlaneProject(Velocity, SurfHit.Normal);

	// Compute move parameters
	const FVector Delta = deltaTime * Velocity; // dx = v * dt
	if (!Delta.IsNearlyZero())
	{
		FHitResult Hit;
		SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);
		FVector WallAttractionDelta = -SurfHit.Normal * WallrunAttractionForce * deltaTime;
		SafeMoveUpdatedComponent(WallAttractionDelta, UpdatedComponent->GetComponentQuat(), true, Hit);
	}

	Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime; // v = dx / dt
	TryMantle();
}



bool UExtendedCharacterMovementComp::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}
bool UExtendedCharacterMovementComp::IsMovementMode(EMovementMode InMovementMode) const
{
	return MovementMode == InMovementMode;
}

void UExtendedCharacterMovementComp::SprintPressed()
{
	Safe_bWantsToSprint = true;
}
void UExtendedCharacterMovementComp::SprintReleased()
{
	Safe_bWantsToSprint = false;
}

void UExtendedCharacterMovementComp::WalkPressed()
{
	Safe_bWantsToWalk = !Safe_bWantsToWalk;
}

void UExtendedCharacterMovementComp::WalkReleased()
{
	//
}

void UExtendedCharacterMovementComp::CrouchPressed()
{
	bWantsToCrouch = ~bWantsToCrouch;
}
void UExtendedCharacterMovementComp::CrouchReleased()
{
	//GetWorld()->GetTimerManager().ClearTimer(TimerHandle_EnterProne);
}

void UExtendedCharacterMovementComp::DashPressed()
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - DashStartTime >= DashCoolDownDuration)
	{
		Safe_bWantsToDash = true;
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle_DashCooldown,
			this,
			&UExtendedCharacterMovementComp::OnDashCooldownFinished,
			DashCoolDownDuration);
	}
}
void UExtendedCharacterMovementComp::DashReleased()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_DashCooldown);
	Safe_bWantsToDash = false;
}

void UExtendedCharacterMovementComp::ClimbPressed()
{
	if (ExtendedCharacterOwner->CombatComponent->PlayerState == EPlayerStates::Unoccupied)
	{
		TryClimb();
	}
	//if (IsFalling() || IsClimbing() || IsHanging()) bWantsToCrouch = true;
}

void UExtendedCharacterMovementComp::ClimbReleased()
{
	bWantsToCrouch = false;
}

void UExtendedCharacterMovementComp::PronePressed()
{
	bWantsToCrouch = ~bWantsToCrouch;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_EnterProne,
		this,
		&UExtendedCharacterMovementComp::TryEnterProne,
		Prone_EnterHoldDuration);
}

void UExtendedCharacterMovementComp::ProneReleased()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_EnterProne);
}

void UExtendedCharacterMovementComp::DolphinDivePressed()
{
	// apply custom params
	GravityScale = 2.f;
	BrakingDecelerationFalling = 25.f;
	FallingLateralFriction = 0.f;

	Safe_bWantsToDolphinDive = true;
	PerformDash(true);
}

