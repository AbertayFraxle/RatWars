// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	root= CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(root);
	
	gunModel= CreateDefaultSubobject<USceneComponent>(TEXT("GunModel"));
	gunModel->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (recoilTimer < RECOIL_TIMER_MAX)
	{
		recoilTimer +=DeltaTime;

		float rotateAmount = FMath::Lerp(30,0,recoilTimer/RECOIL_TIMER_MAX);
		gunModel->SetRelativeRotation(FRotator(gunModel->GetRelativeRotation().Roll,rotateAmount,gunModel->GetRelativeRotation().Yaw));
	}
}

bool AGun::CanShootGun()
{

	if (recoilTimer >= RECOIL_TIMER_MAX)
	{
		return true;
	}else
	{
		return false;
	}
}

void AGun::AddRecoil()
{
	recoilTimer = 0;
}
