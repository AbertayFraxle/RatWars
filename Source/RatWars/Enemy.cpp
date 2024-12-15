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

	bloodEmitter->Activate();
	GetComponentByClass<USkeletalMeshComponent>()->SetHiddenInGame(true);

	GetComponentByClass<UCapsuleComponent>()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	shouldDie = true;
	
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

bool AEnemy::ReduceHealth(int damage)
{
	health -=damage;

	if (health <= 0)
	{
		Kill();
		return true;
	}
	return false;
}

