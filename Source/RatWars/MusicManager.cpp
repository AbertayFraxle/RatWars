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

	//set drum volume to 0
	drumValue = 0;

	//set bpm
	trackBPM = 116;

	//set point thresholds to increase MusicSegment
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

	//define callback function for music event
	FOnAkPostEventCallback functionCallback;
	static FName callBackName("CallbackFunction");
	functionCallback.BindUFunction(this, callBackName);


	//set state to the first one, and start the music
		
	FName newStateValue = FName(prefix + ZeroFill(musicSegment));

	//update musicSegment state in WWise
	UAkGameplayStatics::SetState(NULL, FName(FString("MusicSegment")), newStateValue);

	//call music event in WWise
	UAkGameplayStatics::PostEvent(musicEvent, GetOwner(), int32(AkCallbackType::AK_MusicSyncUserCue| AkCallbackType::AK_MusicSyncBeat),functionCallback);

	//set initial values for music track volumes
	UAkGameplayStatics::SetRTPCValue(drumVolume, 10, 0, NULL, FName(FString("DrumVolume")));
	UAkGameplayStatics::SetRTPCValue(synthVolume, 100, 0, NULL, FName(FString("SynthVolume")));
	UAkGameplayStatics::SetRTPCValue(vocalVolume, 100, 0, NULL, FName(FString("VocalVolume")));
	UAkGameplayStatics::SetRTPCValue(effectsVolume, 50, 0, NULL, FName(FString("EffectsVolume")));

	//determine the length of a beat via bpm
	beatLength = 60/trackBPM;
}

// Called every frame
void AMusicManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//increase game timer
	timer += DeltaTime;

	//increase the segment if it should increase
	if (shouldIncrease) 
	{
		IncreaseSegment();
	}
	else {

		//if the score is greater than the current threshold set that should increase
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

	//if callback is triggered by user defined cue in WWise
	if (callbackType == EAkCallbackType::MusicSyncUserCue) {

		UAkMusicSyncCallbackInfo* musicSyncInfo = static_cast<UAkMusicSyncCallbackInfo*>(callbackInfo); 
		

		//define callbacks for different cues
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

		//if at the end of the music, call blueprint function to end game (level loading in C++ is a nightmare lol)
		if (musicSyncInfo->UserCueName ==FString("EndGame") )
		{
			GameEnd();
		}
	}
	if (callbackType == EAkCallbackType::MusicSyncBeat) {
		//if callback is triggered by beat
		BeatCallback();
	}

}

void AMusicManager::UnlockCallback()
{
	locked = false;
}

void AMusicManager::BeatCallback()
{
	//on beat, reduce the drum volume by 5 and update RTPC in WWise
	beatTime = timer;
	if (drumValue > 10)
	{
		drumValue -=5;
	}
	UAkGameplayStatics::SetRTPCValue(drumVolume, drumValue, 0, NULL, FName(FString("DrumVolume")));

	//if vocal shouldnt be muted, set it to the real value, and update RTPC in WWise
	if (vocalValue !=0)
	{
		vocalValue = realVocalValue;
		UAkGameplayStatics::SetRTPCValue(vocalVolume, vocalValue, beatLength, NULL, FName(FString("VocalVolume")));
	}
}

void AMusicManager::VocalMuteCallback()
{
	//mute the vocal track if current segment is meant to loop
	if (!locked)
	{
		vocalValue = 0;
		UAkGameplayStatics::SetRTPCValue(vocalVolume, vocalValue, 0, NULL, FName(FString("VocalVolume")));
	}
}

void AMusicManager::VocalUnmuteCallback()
{
	//unmute the vocals
	vocalValue = realVocalValue;
	UAkGameplayStatics::SetRTPCValue(vocalVolume, vocalValue, 0, NULL, FName(FString("VocalVolume")));
}

void AMusicManager::GameEnd_Implementation()
{
}


bool AMusicManager::IsOnBeat() {
	
	bool returnValue;

	//if the difference between beatTime and currentTime is within a quarter of the beatLength
	if (abs(timer - beatTime) < (beatLength/4) || abs(timer - (beatTime+beatLength)) < (beatLength/4))
	{
		//increase drum volume by 20
		if (drumValue < 100) {
			drumValue += 20;
		}
		returnValue = true;
	}
	else {

		//reduce drumValue by 5
		if (drumValue > 10) {
			drumValue -= 5;
		}

		returnValue = false;
	}

	//update drum track volume by updating RTPC in WWise
	UAkGameplayStatics::SetRTPCValue(drumVolume, drumValue, 0, NULL, FName(FString("DrumVolume")));

	return returnValue;

}

void AMusicManager::SetVocalValue(int nVocalValue)
{
	//set vocal value to new vocal value
	realVocalValue = nVocalValue;
}

