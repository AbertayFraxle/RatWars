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
	//initialise recoil timer
	recoilTimer = RECOIL_TIMER_MAX;
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//initialise rotate amount
	//if recoil timer is less than maximum
	if (recoilTimer < RECOIL_TIMER_MAX)
	{
		//count up recoilTimer
		recoilTimer +=DeltaTime;

		//interpolate the angles of rotation on gun based on recoil timer and apply to model
		float rotateAmount  = FMath::Lerp(30,0,recoilTimer/RECOIL_TIMER_MAX);
		gunModel->SetRelativeRotation(FRotator(gunModel->GetRelativeRotation().Roll,rotateAmount,gunModel->GetRelativeRotation().Yaw));
	}

	
}

bool AGun::CanShootGun()
{

	//if still recoiling return false, otherwise return true
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
	//reset recoil timer
	recoilTimer = 0;
}
