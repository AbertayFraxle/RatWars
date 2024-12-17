// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AkAudioEvent.h"
#include "AkComponent.h"
#include "../Plugins/Wwise/Source/AkAudio/Classes/AkGameplayStatics.h"
#include "NiagaraComponent.h"
#include "Enemy.generated.h"

UCLASS()
class RATWARS_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	//AK events to call
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAkAudioEvent* squeakEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAkAudioEvent* stopSqueakEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAkAudioEvent* deathEvent;

	//niagara vfx particle system
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* bloodEmitter;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//function to kill self
	void Kill();


	//	variables to handle death
	float killTimer;
	bool shouldDie;
	int health;

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool ReduceHealth(int damage);
};
