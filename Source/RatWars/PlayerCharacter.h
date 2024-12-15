// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define FOOT_STEP_INTERVAL 0.4

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "AkAudioEvent.h"
#include "AkComponent.h"
#include "../Plugins/Wwise/Source/AkAudio/Classes/AkGameplayStatics.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MusicManager.h"
#include "Gun.h"
#include "NiagaraComponent.h"
#include "Enemy.h"

#include "NiagaraFunctionLibrary.h"
#include "PlayerCharacter.generated.h"



UCLASS()
class RATWARS_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	//movement action components
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UInputAction * moveIA;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UInputAction * lookIA;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UInputAction * jumpIA;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UInputAction * shootIA;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UInputMappingContext * inputMappingContext;

	//player camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* camera;
	
	//gun variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Gun")
	float gunRange;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Gun")
	int gunDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UChildActorComponent* gunActor;

	AGun* gun;

	//location of player on ground
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector groundLocation;

	//the player's score
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int score;

	//reference to movement component
	UCharacterMovementComponent * movementComponent;

	//empty callback for events
	FOnAkPostEventCallback nullCallback;

	//wwise events
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAkAudioEvent* footstepEvent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAkAudioEvent* gunshotEvent;

	//VFX
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraComponent* muzzleFlash;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* gunBeamTemplate;

	//utility for end of gun
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* gunTip;

	//reference to music manager
	AMusicManager* musicManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float baseScoreAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int beatMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int hitMultiplier;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float stepTimer;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//movement functions
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	//fire gun function
	void Shoot(const FInputActionValue& Value);
};
