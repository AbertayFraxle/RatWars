// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

#include "Components/CapsuleComponent.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	health = 100;

	bloodEmitter = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BloodEmitter"));
	bloodEmitter->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	//post event to play squeaking sound
	FOnAkPostEventCallback nullCallback;
	UAkGameplayStatics::PostEvent(squeakEvent, this, int32(0), nullCallback);
}

void AEnemy::Kill()
{
	//post event to play death sound
	FOnAkPostEventCallback nullCallback;
	UAkGameplayStatics::PostEvent(deathEvent, this, int32(0), nullCallback);

	//activate blood explosion, hide mesh, disable collider and set death bool to true
	bloodEmitter->Activate(true);
	GetComponentByClass<USkeletalMeshComponent>()->SetHiddenInGame(true);
	GetComponentByClass<UCapsuleComponent>()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	shouldDie = true;

	//post event to stop rat squeaking
	UAkGameplayStatics::PostEvent(stopSqueakEvent, this, int32(0), nullCallback);	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if death bool is true count up to 1 second, then destroy
	if (shouldDie)
	{
		killTimer+=DeltaTime;
		if (killTimer	>= 1.f)
		{
			Destroy();
		}
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//function to reduce enemy health, returns true if enemy is killed
bool AEnemy::ReduceHealth(int damage)
{
	//reduce health by damage amount, call kill function if health less than or zero
	health -=damage;
	
	if (health <= 0)
	{
		Kill();
		return true;
	}

	
	return false;
}

