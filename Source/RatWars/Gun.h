// Fill out your copyright notice in the Description page of Project Settings.
#define RECOIL_TIMER_MAX 0.2
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

UCLASS()
class RATWARS_API AGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGun();

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USceneComponent* root;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USceneComponent* gunModel;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float recoilTimer;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool CanShootGun();
	void AddRecoil();
};
