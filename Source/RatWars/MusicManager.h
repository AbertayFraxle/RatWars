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

	//timer for game time
	float timer;

	//store time of beat
	float beatTime;

	//store how long a beat is
	float beatLength;

	//volume values for different music tracks
	int drumValue;
	int vocalValue;
	int realVocalValue;

	//store music segment number
	int musicSegment;

	//store thresholds for increasing segment
	TMap<int, int> pointThresholds;

	//logic for moving on
	bool shouldIncrease;
	bool locked;
	
	//for passing segment to WWise state
	FString prefix;
	FString ZeroFill(int number);

	//function to trigger on callback
	UFUNCTION()
	void CallbackFunction(EAkCallbackType callbackType, UAkCallbackInfo* callbackInfo);

	//different functions for CallbackFunction to call
	void UnlockCallback();

	void BeatCallback();

	void VocalMuteCallback();
	void VocalUnmuteCallback();

	//blueprint function for ending the game
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GameEnd();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//increase the Music Segment
	void IncreaseSegment();

	//determine if shot is on beat
	UFUNCTION(BlueprintCallable)
	bool IsOnBeat();

	//set real vocal value
	void SetVocalValue(int nVocalValue);

};
