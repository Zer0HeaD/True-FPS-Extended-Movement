// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ExtendedCharacter.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSprintStopDelegate);

class UExtendedCharacterMovementComp;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class URecoilAnimationComponent;
class UCombatComponent;

UCLASS()
class AExtendedCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	URecoilAnimationComponent* RecoilAnimationComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UCombatComponent* CombatComponent;

protected:

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;


	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Aim Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ProneAction;

public:
	// Sets default values for this character's properties
	AExtendedCharacter(const FObjectInitializer& ObjectInitializer);

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	virtual void Jump() override;
	virtual void StopJumping() override;
	virtual bool CanJumpInternal_Implementation() const override;

	void StartAiming();
	void StopAiming();

	void StartSprinting();
	UFUNCTION(BlueprintCallable) void StopSprinting();
	void StopSprintingOnMinimalSpeed();

	void StartProne();
	void StopProne();

	bool bPressedExtendedJump;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) bool bFirstPerson;


	//DELEGATES
	//UPROPERTY(BlueprintAssignable) FSprintStopDelegate SprintStopDelegate;

	// For Animations
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Controls, meta = (AllowPrivateAccess = "true"))
	FRotator LookRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Controls, meta = (AllowPrivateAccess = "true"))
	float PivotOffsetPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controls, meta = (AllowPrivateAccess = "true"))
	float MinimalSpeedToStopSprinting = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controls, meta = (AllowPrivateAccess = "true"))
	float MaxWeaponPitch = -1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controls, meta = (AllowPrivateAccess = "true"))
	float MinWeaponPitch = 1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controls, meta = (AllowPrivateAccess = "true"))
	float MaxWeaponPivot = -1000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controls, meta = (AllowPrivateAccess = "true"))
	float MinWeaponPivot = 1000.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) float MoveX = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) float LookX = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) float LookY = 0.f;

#pragma region Blueprint Ipmlementable

	// BLUEPRINT IMPLEMETATION
	UFUNCTION(BlueprintImplementableEvent, Category = "CustomEvents") void UpdateWeaponBarCurrentAmmo();
	UFUNCTION(BlueprintImplementableEvent, Category = "CustomEvents") void UpdateWeaponBarMaxAmmo();

#pragma endregion

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement)
	UExtendedCharacterMovementComp* ExtendedCharacterMovementComponent;

	virtual void Landed(const FHitResult& Hit) override;

private:
	
	FTimerHandle SprintCheckTimerHandle;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ZZZ")
	void OnAimStarted();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ZZZ")
	void OnAimEnded();

	UFUNCTION(BlueprintCallable) void InitializedCharacterInput();

	FCollisionQueryParams GetIgnoreCharacterParams() const;

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE UExtendedCharacterMovementComp* GetExtendedCharacterMovement()
		const {
		return ExtendedCharacterMovementComponent;
	}

};
