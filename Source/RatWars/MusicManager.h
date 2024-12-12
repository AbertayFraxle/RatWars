// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AkAudioEvent.h"
#include "AkComponent.h"
#include "../Plugins/Wwise/Source/AkAudio/Classes/AkGameplayStatics.h"

#include "MusicManager.generated.h"

UCLASS()
class RATWARS_API AMusicManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMusicManager();

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	UAkAudioEvent* musicEvent;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int score;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAkRtpc* drumVolume;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAkRtpc* synthVolume;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAkRtpc* vocalVolume;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UAkRtpc* effectsVolume;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float trackBPM;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float timer;
	float beatTime;
	float beatLength;
	
	int drumValue;

	int musicSegment;
	TMap<int, int> pointThresholds;
	bool shouldIncrease;
	bool locked;
	
	
	FString prefix;

	FString ZeroFill(int number);

	UFUNCTION()
	void CallbackFunction(EAkCallbackType callbackType, UAkCallbackInfo* callbackInfo);

	void UnlockCallback();

	void BeatCallback();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void IncreaseSegment();

	UFUNCTION(BlueprintCallable)
	bool IsOnBeat();

};
