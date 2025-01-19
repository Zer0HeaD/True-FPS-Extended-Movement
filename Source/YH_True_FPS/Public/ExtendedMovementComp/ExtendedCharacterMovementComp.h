// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ExtendedCharacterMovementComp.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDashStartDelegate);

class AExtendedCharacter;

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None       UMETA(Hidden),
	CMOVE_Slide      UMETA(DisplayName = "Slide"),
	CMOVE_Prone      UMETA(DisplayName = "Prone"),
	CMOVE_Wallrun    UMETA(DisplayName = "Wallrun"),
	CMOVE_Hang       UMETA(DisplayName = "Hang"),
	CMOVE_Climb      UMETA(DisplayName = "Climb"),
	CMOVE_MAX        UMETA(Hidden),
};

/**
 *
 */
UCLASS()
class UExtendedCharacterMovementComp : public UCharacterMovementComponent
{
	GENERATED_BODY()

	class FSavedMove_ExtendedMoveCharacter : public FSavedMove_Character
	{
		typedef FSavedMove_Character Super;

	public:
		enum CompressedFlags
		{
			FLAG_Sprint = 0x10,
			FLAG_Dash = 0x20,
			FLAG_Custom_2 = 0x40,
			FLAG_Custom_3 = 0x80,
			FLAG_Walk = 0x160,
		};

		//Flag
		uint8 Saved_bWantsToSprint : 1;
		uint8 Saved_bWantsToWalk : 1;
		uint8 Saved_bWantsToDash : 1;
		uint8 Saved_bPressedExtendedJump : 1;

		//Other Variables
		uint8 Saved_bPrevWantsToCrouch : 1;
		uint8 Saved_bWantsToProne : 1;
		uint8 Saved_bHadAnimRootMotion : 1;
		uint8 Saved_bTransitionFinished : 1;
		uint8 Saved_bWallrunIsRight : 1;


		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;
	};

	class FNetworkPredictionData_Client_ExtendedCharacter : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_Client_ExtendedCharacter(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		virtual FSavedMovePtr AllocateNewMove() override;
	};

public:

	//Params
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ground Movement")
	float MaxSprintSpeed = 750.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ground Movement")
	float MaxWalkingSpeed = 200.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ground Movement")
	float JumpForwardForce = 250.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ground Movement")
	float JumpForwardModifier = 1.f;

private:

	//Slide params
	UPROPERTY(EditDefaultsOnly, Category = "Slide") float Slide_MinSpeed = 350;       //Min required speed to start sliding
	UPROPERTY(EditDefaultsOnly, Category = "Slide") float Slide_MaxSpeed = 250;
	UPROPERTY(EditDefaultsOnly, Category = "Slide") float Slide_EnterImpulse = 850;   //Start impulse when entering side
	UPROPERTY(EditDefaultsOnly, Category = "Slide") float Slide_GravityForce = 2500;  //How many gravity force applied to slide + how fast velocity changes on slope
	UPROPERTY(EditDefaultsOnly, Category = "Slide") float SlideFrictionFactor = 0.06f;  //How fast char loose velocity on surface
	UPROPERTY(EditDefaultsOnly, Category = "Slide") float BrakingDecelerationSliding = 800.f;
	UPROPERTY(EditDefaultsOnly, Category = "Slide") float SlideJumpForce = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Slide") float SlideJumpUPModifier = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "Slide") float SlideJumpFORWARDModifier = 0.35f;
	UPROPERTY(EditDefaultsOnly, Category = "Slide") float SlideJumpACCELERATIONModifier = 0.35f;

	//Prone params
	UPROPERTY(EditDefaultsOnly, Category = "Prone") float Prone_EnterHoldDuration = 0.01f;       //How long to hold to entering the prone
	UPROPERTY(EditDefaultsOnly, Category = "Prone") float ProneSlideEnterImpulse = 300.f;       //Dive into prone after slide
	UPROPERTY(EditDefaultsOnly, Category = "Prone") float ProneMaxSpeed = 300.f;                //ProneMaxSpeed
	UPROPERTY(EditDefaultsOnly, Category = "Prone") float BrakingDecelerationProning = 2500.f;  //

	//Dash
	UPROPERTY(EditDefaultsOnly, Category = "Dash") float DashImpulse = 2000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Dash") float DolphinDiveImpulse = 1200.f;
	UPROPERTY(EditDefaultsOnly, Category = "Dash") float DolphinDiveZModifier = 0.65f;
	UPROPERTY(EditDefaultsOnly, Category = "Dash") float DashCoolDownDuration = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Dash") float AuthDashCooldownDuration = 0.9f;

	//Mantle
	UPROPERTY(EditDefaultsOnly, Category = "Mantle") float MantleMaxDistance = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "Mantle") float MantleReachHeight = 50.f;
	UPROPERTY(EditDefaultsOnly, Category = "Mantle") float MinMantleDepth = 30.f;
	UPROPERTY(EditDefaultsOnly, Category = "Mantle") float MantleMinWallSteepnessAngle = 75.f;
	UPROPERTY(EditDefaultsOnly, Category = "Mantle") float MantleMaxSurfaceAngle = 40.f;
	UPROPERTY(EditDefaultsOnly, Category = "Mantle") float MantleMaxAlignmentAngle = 45.f;
	UPROPERTY(EditDefaultsOnly, Category = "Mantle") UAnimMontage* TallMantleMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Mantle") UAnimMontage* TransitionTallMantleMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Mantle") UAnimMontage* ProxyTallMantleMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Mantle") UAnimMontage* ShortMantleMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Mantle") UAnimMontage* TransitionShortMantleMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Mantle") UAnimMontage* ProxyShortMantleMontage;



	//Wallrun
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun") float MinWallrunSpeed = 160.f;
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun") float MaxWallrunSpeed = 850.f;
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun") float MaxVerticalWallrunSpeed = 500.f;
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun") float WallrunPullAwayAngle = 120.f;
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun") float WallrunAttractionForce = 2500.f;
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun") float MinWallrunHeight = 25.f;
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun") UCurveFloat* WallrunGravityScaleCurve;
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun") float WallJumpOffForce = 800.f;
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun") float ArchFactor = 0.45f;
	UPROPERTY(EditDefaultsOnly, Category = "Wallrun") float ArchFactorAppliedDelay = 1.f;


	//Hang
	UPROPERTY(EditDefaultsOnly, Category = "Hang") UAnimMontage* TransitionHangMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Hang") UAnimMontage* WallJumpMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Hang") float WallJumpForce = 400.f;

	//Climb
	UPROPERTY(EditDefaultsOnly, Category = "Climb") float MaxClimbSpeed = 300.f;
	UPROPERTY(EditDefaultsOnly, Category = "Climb") float BrakingDecelerationClimbing = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Climb") float ClimbReachDistance = 200.f;


	//Transient
	UPROPERTY(Transient) AExtendedCharacter* ExtendedCharacterOwner;

	//FLAGS
	bool Safe_bPrevWantsToCrouch;
	bool Safe_bWantsToProne;
	bool Safe_bWantsToDash;

	bool Safe_bHadAnimRootMotion;
	float DashStartTime;

	bool bStartWallrunArch = false;

	FTimerHandle TimerHandle_EnterProne;
	FTimerHandle TimerHandle_DashCooldown;

	FTimerHandle TimerHandle_ArchWallrun;

	bool Safe_bTransitionFinished;
	TSharedPtr<FRootMotionSource_MoveToForce> TransitionRMS;
	FString TransitionName;

	UPROPERTY(Transient) UAnimMontage* TransitionQueuedMontage;
	int TransitionRMS_ID;

	bool Safe_bWallrunIsRight;

	//Replication
	UPROPERTY(ReplicatedUsing = OnRep_DashStart) bool Proxy_bDashStart;
	UPROPERTY(ReplicatedUsing = OnRep_ShortMantle) bool Proxy_bShortMantle;
	UPROPERTY(ReplicatedUsing = OnRep_TallMantle) bool Proxy_bTallMantle;

public:


	UPROPERTY(BlueprintReadOnly) float TransitionQueuedMontageSpeed;

	UPROPERTY(BlueprintReadOnly) bool Safe_bWantsToSprint;
	UPROPERTY(BlueprintReadOnly) bool Safe_bWantsToWalk;
	UPROPERTY(BlueprintReadOnly) bool Safe_bWantsToDolphinDive;

	//DELEGATES
	UPROPERTY(BlueprintAssignable) FDashStartDelegate DashStartDelegate;

	UExtendedCharacterMovementComp();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	virtual bool IsMovingOnGround() const override;
	virtual bool CanCrouchInCurrentState() const override;
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxBrakingDeceleration() const override;

	virtual bool CanAttemptJump() const override;
	virtual bool DoJump(bool bReplayingMoves, float DeltaTime) override;

protected:

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;

	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	virtual void InitializeComponent() override;

private:

	//Prone
	void TryEnterProne() { Safe_bWantsToProne = true; }

	UFUNCTION(Server, Reliable) void Server_EnterProne();

	void EnterProne(EMovementMode PrevMode, ECustomMovementMode PrevCustomMode);
	void ExitProne();
	bool CanProne() const;
	void PhysProne(float deltaTime, int32 Iteration);

	//Slide
	UFUNCTION(BlueprintCallable) void EnterSlide(EMovementMode PrevMode, ECustomMovementMode PrevCustomMode);

public:
	UFUNCTION(BlueprintCallable) void ExitSlide();

private:
	UFUNCTION(BlueprintCallable) bool CanSlide() const;

	void PhysSlide(float deltaTime, int32 Iterations);
	bool GetSlideSurface(FHitResult& Hit) const;


	//Dash
	void OnDashCooldownFinished();
	bool CanDash() const;

	//Mantle
	bool TryMantle();
	FVector GetMantleStartLocation(FHitResult FrontHit, FHitResult SurfaceHit, bool bTallMantle) const;

	//Wallrun
	bool TryWallrun();
	void PhysWallrun(float deltaTime, int32 Iterations);
	void OnWallrunArchApplied();
	void ResetWallrunArch();


	//Climb
	UFUNCTION(BlueprintCallable) bool TryHang();

	UFUNCTION(BlueprintCallable) bool TryClimb();
	void PhysClimb(float deltaTime, int32 Iterations);

	//Helpers
	bool IsServer() const;
	float CapR() const;
	float CapHH() const;

	//Replication
	UFUNCTION() void OnRep_DashStart();
	UFUNCTION() void OnRep_ShortMantle();
	UFUNCTION() void OnRep_TallMantle();

public:
	//Dash
	void PerformDash(bool bDolphinDive);

	UFUNCTION(BlueprintPure) bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;
	UFUNCTION(BlueprintPure) bool IsMovementMode(EMovementMode InMovementMode) const;

	UFUNCTION(BlueprintPure) bool IsSprinting() const { return Safe_bWantsToSprint; }
	UFUNCTION(BlueprintPure) bool IsSliding() const { return IsCustomMovementMode(CMOVE_Slide); }
	UFUNCTION(BlueprintPure) bool IsWallrunning() const { return IsCustomMovementMode(CMOVE_Wallrun); }
	UFUNCTION(BlueprintPure) bool WallrunningIsRight() const { return Safe_bWallrunIsRight; }
	UFUNCTION(BlueprintPure) bool IsHanging() const { return IsCustomMovementMode(CMOVE_Hang); }
	UFUNCTION(BlueprintPure) bool IsClimbing() const { return IsCustomMovementMode(CMOVE_Climb); }
	UFUNCTION(BlueprintPure) bool IsProning() const { return IsCustomMovementMode(CMOVE_Prone); }

	UFUNCTION(BlueprintPure) bool IsDolphinDive() const { return Safe_bWantsToDolphinDive; }

	UFUNCTION(BlueprintCallable) void SprintPressed();
	UFUNCTION(BlueprintCallable) void SprintReleased();

	UFUNCTION(BlueprintCallable) void WalkPressed();
	UFUNCTION(BlueprintCallable) void WalkReleased();

	UFUNCTION(BlueprintCallable) void CrouchPressed();
	UFUNCTION(BlueprintCallable) void CrouchReleased();

	UFUNCTION(BlueprintCallable) void DashPressed();
	UFUNCTION(BlueprintCallable) void DashReleased();

	UFUNCTION(BlueprintCallable) void ClimbPressed();
	UFUNCTION(BlueprintCallable) void ClimbReleased();

	UFUNCTION(BlueprintCallable) void PronePressed();
	UFUNCTION(BlueprintCallable) void ProneReleased();
	
	UFUNCTION(BlueprintCallable) void DolphinDivePressed();
};
