// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h" 
#include "RecoilAnimationComponent.h"
#include "Camera/CameraShakeBase.h"
#include "CombatComponent.generated.h"

class AWeaponBase;
class AExtendedCharacter;
class USoundBase;
class UAnimMontage;
class UCurveFloat;
class UParticleSystem;
class UMaterialInstance;

UENUM(BlueprintType)
enum class EPlayerStates : uint8
{
	Unoccupied				UMETA(DisplayName = "Unoccupied"),
	Shooting				UMETA(DisplayName = "Shooting"),
	Reloading				UMETA(DisplayName = "Reloading"),
	SwitchingWeapon			UMETA(DisplayName = "SwitchingWeapon"),
	MeleeAtacking			UMETA(DisplayName = "MeleeAtacking"),
	Climbing				UMETA(DisplayName = "Climbing")
};

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	Ammo_AR		UMETA(DisplayName = "Assault Rifle Ammo"),
	Ammo_SMG	UMETA(DisplayName = "Submachine Gun Ammo"),
	Ammo_Pistol UMETA(DisplayName = "Pistol Ammo"),
	// Add more ammo types as needed
};

USTRUCT(BlueprintType)
struct FAmmoData
{
	GENERATED_BODY()

public:
	// Current ammo count
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo Equipment")
	int32 CurrentAmmo;

	// Maximum ammo count
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo Equipment")
	int32 MaxAmmo;

	/*FAmmoData()
		: CurrentAmmo(0), MaxAmmo(0) {}

	FAmmoData(int32 InCurrentAmmo, int32 InMaxAmmo)
		: CurrentAmmo(InCurrentAmmo), MaxAmmo(InMaxAmmo) {}*/
};

// DEFAULT SETUP OF RECOIL SYSTEM
//float RecoilStat = 85.f;
//float BaseRecoilPitchForce = 5.f;
//float BaseRecoilYawForce = 8.f;
//float BloomStep = 0.5f;
//float ADSBloomModifier = 0.2f;
//float BloomRecoveryInterpSpeed = 20.f;
//float MaxBloom = 10.f;
//float MaxADSHeat = 10.f;
//float ADSheatModifierMax = 0.6f;
USTRUCT(BlueprintType)
struct FWeaponSettings : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 MagazineCapacity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 CurrentMagazineAmmo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EAmmoType AmmoType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool IsPumpOrBoltAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "IsPumpOrBoltAction", EditConditionHides))
	UAnimMontage* ReloadInsertAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "IsPumpOrBoltAction", EditConditionHides))
	UAnimMontage* ReloadEndAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "IsPumpOrBoltAction", EditConditionHides))
	USoundBase* ReloadInsertSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "IsPumpOrBoltAction", EditConditionHides))
	USoundBase* ReloadEndSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "IsPumpOrBoltAction", EditConditionHides))
	UAnimMontage* WeaponReloadInsertAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "IsPumpOrBoltAction", EditConditionHides))
	UAnimMontage* WeaponReloadEndAnimation;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite) float PumpOrBoltReloadTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float FireRateTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float FireRateAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TEnumAsByte<EFireMode_PRAS> FireMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Burst;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 NumberOfBulletsPerShot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RecoilStat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BaseRecoilPitchForce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BaseRecoilYawForce;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BloomStep;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ADSBloomModifier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float BloomRecoveryInterpSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxBloom;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxADSHeat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ADSheatModifierMax;


	UPROPERTY(EditAnywhere, BlueprintReadWrite) URecoilData* RecoilAnimData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<UCameraShakeBase> RecoilCameraShake;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<AActor> BulletProjectile;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<AActor> BulletShellEject;


	UPROPERTY(EditAnywhere, BlueprintReadWrite) UParticleSystem* WeaponMuzzleParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRotator WeaponMuzzleParticleRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UParticleSystem* HitEffectParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UMaterialInterface* HitImpact;


	UPROPERTY(EditAnywhere, BlueprintReadWrite) USoundBase* UnholsterSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) USoundBase* HolsterSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) USoundBase* UnholsterQuickSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) USoundBase* HolsterQuickSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) USoundBase* AimInSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) USoundBase* AimOutSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) USoundBase* FireSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) USoundBase* FireSuppressorSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) USoundBase* FireEmptySound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) USoundBase* ReloadSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) USoundBase* ReloadEmptySound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) USoundBase* InspectSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) USoundBase* GrenadeThrowSound;


	UPROPERTY(EditAnywhere, BlueprintReadWrite) UAnimMontage* UnholsterAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UAnimMontage* HolsterAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) UAnimMontage* UnholsterQuickAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UAnimMontage* HolsterQuickAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) UAnimMontage* AimInAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UAnimMontage* AimOutAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) UAnimMontage* FireAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UAnimMontage* FireAimAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) UAnimMontage* EmptyFireAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UAnimMontage* EmptyFireAimAnimation;

	// For Pump And Bold action weapon - we using this animations as Reload Start and not come back to main reload function

	UPROPERTY(EditAnywhere, BlueprintReadWrite) UAnimMontage* ReloadAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UAnimMontage* ReloadEmptyAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) UAnimMontage* ReloadAimAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UAnimMontage* ReloadAimEmptyAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) UAnimMontage* MeleeAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UAnimMontage* InspectAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UAnimMontage* GrenadeThrowAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<UAnimInstance> WeaponAnimationBlueprint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UAnimMontage* WeaponFireAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UAnimMontage* WeaponReloadAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UAnimMontage* WeaponEmptyReloadAnimation;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

#pragma region FiringAWeapon


	UFUNCTION(BlueprintCallable) void InitializeCurrentWeaponVariables();

	UFUNCTION(BlueprintCallable) void StartFiring();
	UFUNCTION(BlueprintCallable) void StopFiring();

	void OnFire();
	bool CanFire();

	void UnlockFire();

	void ShootAProjectile(const UWorld* World, 
		FVector MuzzleSocketLocation, 
		FHitResult result, 
		FActorSpawnParameters ActorSpawnParams, 
		FVector LaunchDirection);

public:
	void StartADS();
	void StopADS();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) EPlayerStates PlayerState = EPlayerStates::Unoccupied;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) bool bInADS = false;
protected:

	bool bFiringAWeapon = false;
	bool bUnlockFire = true;
	FTimerHandle AutomaticFireHandle;
	FTimerHandle LockingFireHandle;
	FTimerHandle BurstFireHandle;

	int BurstCurrentShots = 0;
	float BurstLockingTimer = 1.f;

#pragma endregion

#pragma region PRAS

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	URecoilAnimationComponent* PlayerRecoilAnimComponent;

#pragma endregion

#pragma region RecoilSystem

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CombatSystem, meta = (AllowPrivateAccess = "true"))
	AWeaponBase* CurrentWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CombatSystem, meta = (AllowPrivateAccess = "true"))
	FWeaponSettings CurrentWeaponSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CombatSystem, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* RecoilDirectionCurve;

public:

	float InitialRecoilPitchForce;
	float RecoilPitchDamping;
	float RecoilPitchVelocity;

	float InitialRecoilYawForce;
	float RecoilYawDamping;
	float RecoilYawVelocity;

	void StartRecoilRecovery();
	bool bIsRecoilRecoveryActive;
	bool bIsRecoilNeutral = true;
	FRotator RecoilCheckpoint;

	bool bUpdateRecoilPitchCheckpointInNextShot = false;

	bool bIsRecoilYawRecoveryActive;
	bool bUpdateRecoilYawCheckpointInNextShot = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) float CurrentBloom = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float ADSAlpha = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float CurrentADSHeat = 0.f;


protected:

	UPROPERTY() AExtendedCharacter* PlayerOwner;
	UPROPERTY() APlayerController* PlayerController;
	UPROPERTY() UAnimInstance* AnimInstance;
	UPROPERTY() UAnimInstance* CurrentWeaponAnimInstance;

	UFUNCTION(BlueprintCallable) void StartRecoil();
	UFUNCTION(BlueprintCallable) void EndRecoil();

	bool bIsRecoilActive = false;

#pragma endregion

#pragma region Weapon Ammo
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CombatSystem, meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, FAmmoData> AmmoMap;

	UFUNCTION(BlueprintCallable) void AddAmmo(EAmmoType AmmoType, int32 AmountToAdd);
	UFUNCTION(BlueprintCallable) bool ConsumeAmmo(EAmmoType AmmoType, int32 AmountConsume); // CURRENTLY NOT IN USE!
	UFUNCTION(BlueprintCallable) void ReloadWeapon(EAmmoType AmmoType);

	void HandleReload(EAmmoType AmmoType);
	void HandlePumpOrBoltWeaponReload(EAmmoType AmmoType);
	void UnlockReload();
	bool CanReload();
	void UnlockBoltOrPumpWeaponReload();
	FTimerHandle ReloadTimerHandle;
	FTimerHandle PumpOrBoltReloadTimerHandle;
	bool bCanReload = true;

private:
	// HELPER FUNCTIONS

	void PlayAnimationWithDelay(UAnimMontage* InMontage);
	void PlayWeaponAnimation(UAnimMontage* InMontage);
	void PlaySoundAtOwner(USoundBase* InSound);
	void HandleEndReload();

#pragma endregion

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
