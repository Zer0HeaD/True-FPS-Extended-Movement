// Fill out your copyright notice in the Description page of Project Settings.


#include "ExtendedMovementComp/ProneAnimInstance.h"
#include "ExtendedMovementComp/ExtendedCharacter.h"
#include "ExtendedMovementComp/ExtendedCharacterMovementComp.h"
#include "Kismet/KismetSystemLibrary.h"
#include "KismetAnimationLibrary.h"

UProneAnimInstance::UProneAnimInstance()
{

}

void UProneAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!Player)
	{
		Player = Cast<AExtendedCharacter>(TryGetPawnOwner());
	}

	if (!MovementComponent)
	{
		if (Player)
		{
			MovementComponent = Cast<UExtendedCharacterMovementComp>(Player->GetMovementComponent());
		}
	}

	if (Player) 
	{
		IsProne = MovementComponent->IsProning();

		if (/*!IsProne && */!FMath::IsNearlyZero(Player->GetVelocity().Size()))
		{
			LastLocomotionDirectionSaved =
				UKismetAnimationLibrary::CalculateDirection(Player->GetVelocity(), Player->GetActorRotation());
		}

		TurnBodyYaw(UpperYaw, UpperProneYaw);

		FRotator PitchAngle = (Player->GetControlRotation() - Player->GetActorRotation()).GetNormalized();

		UpperPitch = FMath::ClampAngle(PitchAngle.Pitch, -90.0f, 90.0f);

		PlayerSpeed = Player->GetVelocity().Size();

		if (IsProne)
		{
			// Root
			FVector RootDir = ProneRootIK("pelvis", ProneIK_Pelvis_Rot, bDebugEnable);


			FVector SpineDir = ProneMiddleIK("ProneSpineLoc", ProneIK_Upper_Rot, RootDir, bDebugEnable);

			FVector Calf_Rgiht_Dir = ProneMiddleIK("ProneCalfRightLoc", ProneIK_Right_Knee_Rot, RootDir, bDebugEnable);
			FVector Calf_Left_Dir = ProneMiddleIK("ProneCalfLeftLoc", ProneIK_Left_Knee_Rot, RootDir, bDebugEnable);


			ProneEndIK("ProneFootRightLoc", ProneIK_Right_Foot_Rot, Calf_Rgiht_Dir, bDebugEnable);
			ProneEndIK("ProneFootLeftLoc", ProneIK_Left_Foot_Rot, Calf_Left_Dir, bDebugEnable);

			ProneEndIK("ProneArmRightLoc", ProneIK_Right_Hand_Rot, SpineDir, bDebugEnable);
			ProneEndIK("ProneArmLeftLoc", ProneIK_Left_Hand_Rot, SpineDir, bDebugEnable);
		}
	}
}




void UProneAnimInstance::TurnBodyYaw(float& Yaw, float& ProneYaw)
{
	float YawEnd = 0.0f;

	// Check if the player is proning
	if (MovementComponent->IsProning())
	{
		if (FMath::IsNearlyZero(Player->GetVelocity().Size())) // Player is idle
		{
			if (!IsTurn)
			{
				IsTurn = true;
				TurnDirEnd = Player->GetActorRotation();
			}

			// Adjust for backward prone if yaw is outside valid range
			IsProneBack = !(ProneYaw <= 110.0f && ProneYaw >= -110.0f);
		}
		else // Player is moving
		{
			TurnDirEnd = Player->GetActorRotation();

			// Adjust for backward prone if necessary
			if (!(ProneYaw <= 110.0f && ProneYaw >= -110.0f))
			{
				TurnDirEnd = FRotator(TurnDirEnd.Pitch, TurnDirEnd.Yaw + 180.0f, TurnDirEnd.Roll);
				IsProneBack = true;
			}
			else
			{
				IsProneBack = false;
			}
		}

		ProneRot = GetProneRotBlend(TurnDirEnd.Vector(), Player->GetActorRotation());
	}
	else
	{
		TurnDirEnd = Player->GetActorRotation(); // Use actor rotation
	}

	// Smoothly interpolate between current and target rotation
	TurnDir = FMath::RInterpTo(TurnDir, TurnDirEnd, GetWorld()->GetDeltaSeconds(), 5.0f);
	//TurnDir = TurnDirEnd;

	// Calculate the yaw difference
	FRotator InterpToAngle = (Player->GetActorRotation() - TurnDir).GetNormalized();

	// Update ProneYaw using InterpToAngle and LastLocomotionDirectionSaved
	if (FMath::IsNearlyZero(Player->GetVelocity().Size()) && bCompensateProneRotation) // Player is idle
	{
		ProneYaw = InterpToAngle.Yaw - LastLocomotionDirectionSaved;
	}
	else // Player is moving
	{
		bCompensateProneRotation = false;
		LastLocomotionDirectionSaved = 0.f;
		ProneYaw = InterpToAngle.Yaw;
	}

	// Clamp the yaw value between -90 and 90 for body turning limits
	YawEnd = FMath::ClampAngle(InterpToAngle.Yaw, -90.0f, 90.0f);
	Yaw = -YawEnd; // Update the output Yaw value


	//UE_LOG(LogTemp, Warning, TEXT("Player Rotation: %f"), Player->GetActorRotation().Yaw);
	//UE_LOG(LogTemp, Warning, TEXT("TurnDir: %f"), TurnDir.Yaw);
	//UE_LOG(LogTemp, Warning, TEXT("InterpToAngle: %f"), InterpToAngle.Yaw);
	//UE_LOG(LogTemp, Warning, TEXT("ProneYaw after calculations: %f, YawEnd: %f"), ProneYaw, YawEnd);
}


float UProneAnimInstance::RemapValue(float Value, float InMin, float InMax, float OutMin, float OutMax)
{
	return FMath::Clamp((Value - InMin) / (InMax - InMin), 0.0f, 1.0f) * (OutMax - OutMin) + OutMin;
}


/*void UProneAnimInstance::TurnBodyYaw(float& Yaw, float& ProneYaw)
{
	float YawEnd = 0.0f;

	// Check if the player is proning
	if (MovementComponent->IsProning())
	{
		// Player is moving
		if (Player->GetVelocity().Size() > 3.0f)
		{
			TurnDirEnd = Player->GetActorRotation();

			// Check if prone yaw is outside valid range
			if (!(ProneYaw <= 110.0f && ProneYaw >= -110.0f))
			{
				// Adjust rotation for backward prone
				TurnDirEnd = FRotator(TurnDirEnd.Pitch, TurnDirEnd.Yaw + 180.0f, TurnDirEnd.Roll);
				IsProneBack = true;
			}
			else
			{
				IsProneBack = false;
			}
		}
		// Player is not moving
		else
		{
			if (!IsTurn)
			{
				IsTurn = true;
				TurnDirEnd = Player->GetActorRotation();
			}

			// Adjust for backward prone if yaw is outside valid range
			IsProneBack = !(ProneYaw <= 110.0f && ProneYaw >= -110.0f);
		}
		
		ProneRot = GetProneRotBlend(TurnDirEnd.Vector(), Player->GetActorRotation());
	}
	// Player is not proning
	else
	{
		if (Player->GetVelocity().Size() > 0.0f)
		{
			// Update direction when the player is moving
			TurnDir = Player->GetActorRotation();
			TurnDirEnd = Player->GetActorRotation();
			IsTurn = false;
		}
		else
		{
			// Handle idle turning logic
			if (!IsTurn)
			{
				IsTurn = true;
				TurnDirEnd = Player->GetActorRotation();
			}
			else
			{
				// Reset rotation if yaw exceeds range
				if (Yaw >= 70.0f || Yaw <= -70.0f)
				{
					TurnDirEnd = Player->GetActorRotation();
				}
			}
		}
		TurnDirEnd = FRotator(0.f, LastLocomotionDirectionSaved, 0.f);
	}

	// Smoothly interpolate between current and target rotation
	TurnDir = FMath::RInterpTo(TurnDir, TurnDirEnd, GetWorld()->GetDeltaSeconds(), 5.0f);

	// Calculate the yaw difference between player rotation and target direction
	FRotator InterpToAngle = (Player->GetActorRotation() - TurnDir).GetNormalized();
	ProneYaw = InterpToAngle.Yaw;

	// Clamp the yaw value between -90 and 90 for body turning limits
	YawEnd = FMath::ClampAngle(InterpToAngle.Yaw, -90.0f, 90.0f);
	Yaw = -YawEnd; // Update the output Yaw value
}*/



FVector UProneAnimInstance::ProneRootIK(FName BoneName, FRotator& Rot, bool IsShow)
{
	FVector SocketLoc = Player->GetMesh()->GetSocketLocation(BoneName);
	FVector StartTracer = SocketLoc + FVector(0.0f, 0.0f, 20.0f);
	FVector EndTracer = SocketLoc - FVector(0.0f, 0.0f, 60.0f);
	TArray<AActor*> ActorsToIgnore;
	FHitResult OutHit;

	bool hitis = false;
	if (IsShow)
	{
		hitis = UKismetSystemLibrary::SphereTraceSingle(this, StartTracer, EndTracer, 3.0f,
			ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore,
			EDrawDebugTrace::ForOneFrame, OutHit, true);
	}
	else
	{
		hitis = UKismetSystemLibrary::SphereTraceSingle(this, StartTracer, EndTracer, 3.0f,
			ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore,
			EDrawDebugTrace::None, OutHit, true);
	}

	if (hitis)
	{
		FVector ImpactNomal = OutHit.ImpactNormal;
		/*UKismetSystemLibrary::SphereTraceSingle(this, ImpactNomal * 40.0f + SocketLoc, ImpactNomal * -40.0f + SocketLoc, 3.0f,
			ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore,
			EDrawDebugTrace::ForOneFrame, OutHit, true);*/
			//UE_LOG(LogTemp, Warning, TEXT("ImpactNomal: %f, %f, %f"), ImpactNomal.X, ImpactNomal.Y, ImpactNomal.Z);
		float ProneRotationRoll = FMath::RadiansToDegrees(FMath::Atan2(ImpactNomal.Y, ImpactNomal.Z));
		float ProneRotationPitch = FMath::RadiansToDegrees(FMath::Atan2(ImpactNomal.X, ImpactNomal.Z) * -1.0f);
		Rot = FMath::RInterpTo(Rot, FRotator(ProneRotationPitch, 0.0f, ProneRotationRoll), GetWorld()->GetDeltaSeconds(), 15.0f);
		return ImpactNomal;
	}

	return FVector::ZeroVector;
}

FVector UProneAnimInstance::ProneMiddleIK(FName BoneName, FRotator& Rot, FVector Dir, bool IsShow)
{
	FVector SocketLoc = Player->GetMesh()->GetSocketLocation(BoneName);
	FVector StartTracer = SocketLoc + Dir * 20.0f;
	FVector EndTracer = SocketLoc - Dir * 60.0f;
	TArray<AActor*> ActorsToIgnore;
	FHitResult OutHit;

	bool hitis = false;
	if (IsShow)
	{
		hitis = UKismetSystemLibrary::SphereTraceSingle(this, StartTracer, EndTracer, 3.0f,
			ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore,
			EDrawDebugTrace::ForOneFrame, OutHit, true);
	}
	else
	{
		hitis = UKismetSystemLibrary::SphereTraceSingle(this, StartTracer, EndTracer, 3.0f,
			ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore,
			EDrawDebugTrace::None, OutHit, true);
	}

	if (hitis)
	{
		FVector ImpactNomal = OutHit.ImpactNormal;
		float ProneRotationRoll = ProneDegrees(ImpactNomal.Y, ImpactNomal.Z, Dir.Y, Dir.Z, 1.0f);
		float ProneRotationPitch = ProneDegrees(ImpactNomal.X, ImpactNomal.Z, Dir.X, Dir.Z, -1.0f);
		Rot = FMath::RInterpTo(Rot, FRotator(ProneRotationPitch, 0.0f, ProneRotationRoll), GetWorld()->GetDeltaSeconds(), 15.0f);
		return ImpactNomal;
	}

	return FVector::ZeroVector;
}

void UProneAnimInstance::ProneEndIK(FName BoneName, FRotator& Rot, FVector Dir, bool IsShow)
{
	FVector SocketLoc = Player->GetMesh()->GetSocketLocation(BoneName);
	FVector StartTracer = SocketLoc + Dir * 20.0f;
	FVector EndTracer = SocketLoc - Dir * 60.0f;

	FRotator Test = FRotator(0.0f, 0.0f, 5.0f);

	TArray<AActor*> ActorsToIgnore;
	FHitResult OutHit;

	bool hitis = false;
	if (IsShow)
	{
		hitis = UKismetSystemLibrary::SphereTraceSingle(this, StartTracer, EndTracer, 3.0f,
			ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore,
			EDrawDebugTrace::ForOneFrame, OutHit, true);
	}
	else
	{
		hitis = UKismetSystemLibrary::SphereTraceSingle(this, StartTracer, EndTracer, 3.0f,
			ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore,
			EDrawDebugTrace::None, OutHit, true);
	}

	if (hitis)
	{
		FVector ImpactNomal = OutHit.ImpactNormal;
		/*UKismetSystemLibrary::SphereTraceSingle(this, ImpactNomal * 60.0f + SocketLoc, ImpactNomal * -60.0f + SocketLoc, 3.0f,
			ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore,
			EDrawDebugTrace::ForOneFrame, OutHit, true);*/
		float ProneRotationRoll = ProneDegrees(ImpactNomal.Y, ImpactNomal.Z, Dir.Y, Dir.Z, 1.0f);
		float ProneRotationPitch = ProneDegrees(ImpactNomal.X, ImpactNomal.Z, Dir.X, Dir.Z, -1.0f);
		Rot = FMath::RInterpTo(Rot, FRotator(ProneRotationPitch, 0.0f, ProneRotationRoll), GetWorld()->GetDeltaSeconds(), 15.0f);
	}
}

float UProneAnimInstance::ProneDegrees(float ImpactNomalXY, float ImpactNomalZ, float DirXY, float DirZ, float XY)
{
	float Degrees = 0.0f;
	if (DirZ > ImpactNomalZ)
	{
		Degrees = FMath::RadiansToDegrees(FMath::Atan2(ImpactNomalXY, DirZ) * XY);
	}
	else if (DirZ < ImpactNomalZ)
	{
		Degrees = FMath::RadiansToDegrees(FMath::Atan2(DirXY, ImpactNomalZ) * -1.0f * XY);
	}

	return Degrees;
}

FRotBlend UProneAnimInstance::GetProneRotBlend(FVector PlayerForwardLoc, FRotator PlayerRot)
{
	PlayerForwardLoc.Normalize();
	FMatrix RotMatrix = FRotationMatrix(PlayerRot);
	FVector ForwardVector = RotMatrix.GetScaledAxis(EAxis::X);
	FVector RightVector = RotMatrix.GetScaledAxis(EAxis::Y);
	FVector NormalizedVel = PlayerForwardLoc.GetSafeNormal2D();

	float DirForward = FVector::DotProduct(ForwardVector, NormalizedVel);
	float DirRight = FVector::DotProduct(RightVector, NormalizedVel);

	//UE_LOG(LogTemp, Warning, TEXT("DirForward: %f"), DirForward);
	//UE_LOG(LogTemp, Warning, TEXT("DirRight: %f"), DirRight);
	FRotBlend SetRot;
	SetRot.Front = FMath::Clamp(DirForward, 0.0f, 1.0f);
	SetRot.Back = FMath::Abs(FMath::Clamp(DirForward, -1.0f, 0.0f));
	SetRot.Left = FMath::Abs(FMath::Clamp(DirRight, -1.0f, 0.0f));
	SetRot.Right = FMath::Clamp(DirRight, 0.0f, 1.0f);

	return SetRot;
}
