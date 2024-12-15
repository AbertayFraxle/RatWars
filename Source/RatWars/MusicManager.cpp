// Fill out your copyright notice in the Description page of Project Settings.


#include "MusicManager.h"

// Sets default values
AMusicManager::AMusicManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	prefix = "segment_";
	musicSegment = 1;
	locked = true;
	shouldIncrease = false;

	drumValue = 0;

	trackBPM = 116;
	
	pointThresholds = {
		{1,1000},
		{2,2000},
		{3,3000},
		{4,4000},
		{5,5000},
		{6,6000},
		{7,7000},
		{8,8000},
		{9,9000},
		{10,10000}
	};
}

// Called when the game starts or when spawned
void AMusicManager::BeginPlay()
{
	Super::BeginPlay();

	FOnAkPostEventCallback functionCallback;
	static FName callBackName("CallbackFunction");
	functionCallback.BindUFunction(this, callBackName);


	//set state to the first one, and start the music
		
	FName newStateValue = FName(prefix + ZeroFill(musicSegment));

	//update musicSegment state in WWise
	UAkGameplayStatics::SetState(NULL, FName(FString("MusicSegment")), newStateValue);

	//call music event in WWise
	UAkGameplayStatics::PostEvent(musicEvent, GetOwner(), int32(AkCallbackType::AK_MusicSyncUserCue| AkCallbackType::AK_MusicSyncBeat),functionCallback);

	UAkGameplayStatics::SetRTPCValue(drumVolume, 10, 0, NULL, FName(FString("DrumVolume")));
	UAkGameplayStatics::SetRTPCValue(synthVolume, 100, 0, NULL, FName(FString("SynthVolume")));
	UAkGameplayStatics::SetRTPCValue(vocalVolume, 110, 0, NULL, FName(FString("VocalVolume")));
	UAkGameplayStatics::SetRTPCValue(effectsVolume, 50, 0, NULL, FName(FString("EffectsVolume")));

	beatLength = 60/trackBPM;
}

// Called every frame
void AMusicManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	timer += DeltaTime;

	//increase the segment if it should increase
	if (shouldIncrease) 
	{
		IncreaseSegment();
	}
	else {

		if (int32(score) >= pointThresholds.FindRef(musicSegment))
		{
			shouldIncrease = true;
		}
	}
}

void AMusicManager::IncreaseSegment()
{
	//if locked
	if (!locked) {

		//turn off the lock and increase amount
		shouldIncrease = false;
		locked = true;

		//increment music segment
		musicSegment++;

		//convert musicSegment to string and packto pass to WWise
		FName newStateValue = FName(prefix + ZeroFill(musicSegment));

		//update musicSegment state in WWise
		UAkGameplayStatics::SetState(NULL, FName(FString("MusicSegment")), newStateValue);
	}
}

FString AMusicManager::ZeroFill(int number) {

	//if the number is less than 10 pad with a zero at the front, else just convert to string
	if (number < 10) {
		return "0" + FString::FromInt(number);
	}
	else {
		return  FString::FromInt(number);
	}
}

void AMusicManager::CallbackFunction(EAkCallbackType callbackType ,UAkCallbackInfo* callbackInfo)
{
	if (callbackType == EAkCallbackType::MusicSyncUserCue) {

		UAkMusicSyncCallbackInfo* musicSyncInfo = static_cast<UAkMusicSyncCallbackInfo*>(callbackInfo); 
		

		
		if (musicSyncInfo->UserCueName==FString("LoopBegin") )
		{
			UnlockCallback();
		}
		if (musicSyncInfo->UserCueName==FString("VocalUnmute" ))
		{
			VocalUnmuteCallback();
		}
		if (musicSyncInfo->UserCueName ==FString("VocalMute" ))
		{
			VocalMuteCallback();
		}
		if (musicSyncInfo->UserCueName ==FString("OnLastVocalUnmute") )
		{
			OnLastVocalUnmuteCallback();
		}
		
	}
	if (callbackType == EAkCallbackType::MusicSyncBeat) {
		BeatCallback();
	}
}

void AMusicManager::UnlockCallback()
{
	locked = false;
}

void AMusicManager::BeatCallback()
{
	beatTime = timer;
	if (drumValue > 10)
	{
		drumValue -=5;
	}
	UAkGameplayStatics::SetRTPCValue(drumVolume, drumValue, 0, NULL, FName(FString("DrumVolume")));
}

void AMusicManager::VocalMuteCallback()
{
	vocalValue = 0;
	UAkGameplayStatics::SetRTPCValue(vocalVolume, vocalValue, 0, NULL, FName(FString("VocalVolume")));
}

void AMusicManager::VocalUnmuteCallback()
{
	vocalValue = 110;
	UAkGameplayStatics::SetRTPCValue(vocalVolume, vocalValue, 0, NULL, FName(FString("VocalVolume")));
}

void AMusicManager::OnLastVocalUnmuteCallback()
{
	vocalValue = 110;
	UAkGameplayStatics::SetRTPCValue(vocalVolume, vocalValue, 0, NULL, FName(FString("VocalVolume")));
}

bool AMusicManager::IsOnBeat() {

	bool returnValue;

	if (abs(timer - beatTime) < (beatLength/4) || abs(timer - (beatTime+beatLength)) < (beatLength/4))
	{
		if (drumValue < 100) {
			drumValue += 20;
		}
		returnValue = true;
	}
	else {
	
		if (drumValue > 10) {
			drumValue -= 5;
		}

		returnValue = false;
	}
	UAkGameplayStatics::SetRTPCValue(drumVolume, drumValue, 0, NULL, FName(FString("DrumVolume")));

	return returnValue;

}

