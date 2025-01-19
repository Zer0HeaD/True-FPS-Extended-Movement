// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent/WeaponBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	Scope = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponScope"));
	Scope->SetupAttachment(RootComponent);
	Scope->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Rail = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponRail"));
	Rail->SetupAttachment(RootComponent);
	Rail->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Mag = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMag"));
	Mag->SetupAttachment(RootComponent);
	Mag->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MechanicScope = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMechanicScope"));
	MechanicScope->SetupAttachment(RootComponent);
	MechanicScope->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Grip = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponGrip"));
	Grip->SetupAttachment(RootComponent);
	Grip->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Muzzle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMuzzle"));
	Muzzle->SetupAttachment(RootComponent);
	Muzzle->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	LaserFlash = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponLaserFlash"));
	LaserFlash->SetupAttachment(RootComponent);
	LaserFlash->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Bipod = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponBipod"));
	Bipod->SetupAttachment(RootComponent);
	Bipod->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/// LPAMG WEAPONS
	Barrel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BarrelDefault"));
	Barrel->SetupAttachment(RootComponent);
	Barrel->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BipodLPAMG = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BipodLPAMG"));
	BipodLPAMG->SetupAttachment(RootComponent);
	BipodLPAMG->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Forestock = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ForestockDefault"));
	Forestock->SetupAttachment(RootComponent);
	Forestock->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GripLPAMG = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GripAngled"));
	GripLPAMG->SetupAttachment(RootComponent);
	GripLPAMG->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MechanicalScope = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MechanicalScope"));
	MechanicalScope->SetupAttachment(RootComponent);
	MechanicalScope->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MechanicalScopeIronsight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MechanicalScopeIronsight"));
	MechanicalScopeIronsight->SetupAttachment(RootComponent);
	MechanicalScopeIronsight->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RailLPAMG = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RailLPAMG"));
	RailLPAMG->SetupAttachment(RootComponent);
	RailLPAMG->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ScopeLPAMG = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ScopeLPAMG"));
	ScopeLPAMG->SetupAttachment(RootComponent);
	ScopeLPAMG->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Silenser = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Silenser"));
	Silenser->SetupAttachment(RootComponent);
	Silenser->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Slide = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SlideDefault"));
	Slide->SetupAttachment(RootComponent);
	Slide->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

