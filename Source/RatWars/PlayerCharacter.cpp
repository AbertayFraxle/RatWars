// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//// Create a CameraComponent	
	camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	camera->SetupAttachment(RootComponent);
	camera->SetRelativeLocation(FVector(0.f, 0.f, 50.f)); // Position the camera
	camera->bUsePawnControlRotation = true;
	camera->SetupAttachment(RootComponent);

	//create gun actor
	gunActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("GunActor"));
	gunActor->SetupAttachment(camera);

	//create component for position of gun tip
	gunTip = CreateDefaultSubobject<USceneComponent>(TEXT("GunTip"));
	gunTip->SetupAttachment(gunActor);

	//create system for muzzle flash particles
	muzzleFlash = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MuzzleFlash"));
	muzzleFlash->SetupAttachment(gunTip);

	//store reference to movement component
	movementComponent = GetCharacterMovement();

	//initialise multipliers
	beatMultiplier = 1;
	hitMultiplier = 1;

	//initialise health
	health = HEALTH_MAX;

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//store reference to gun actor class
	gun = Cast<AGun>(gunActor->GetChildActor());
	
	//Bind Input Mapping Context to Character
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(inputMappingContext, 0);
		}
	}

	//find music manager and keep the reference
	AActor * foundActor = UGameplayStatics::GetActorOfClass(GetWorld(),AMusicManager::StaticClass());
	musicManager = Cast<AMusicManager>(foundActor);
}

void APlayerCharacter::CalcVocalVolume()
{

	//multiply total multiplier by 20 to determine vocal volume, clamp to max of 100, then pass to music manager
	int nVocalValue = ((hitMultiplier+beatMultiplier)*20);
	if (nVocalValue > 100)
	{
		nVocalValue = 100;
	}
	musicManager->SetVocalValue(nVocalValue);
}

void APlayerCharacter::RegenPlayerHealth(float DeltaTime)
{
	if (regenTimer > 0) {
		//if the cooldown isn't over tick it down
		regenTimer -= DeltaTime;
	}
	else if (health <HEALTH_MAX) {
		//if the cooldown is over, and player under 100 health
		//heal player by 5 health every 2 seconds
		if (secondRegenTimer < 0) {
			health += HEALTH_REGEN_AMOUNT;
			secondRegenTimer = HEALTH_REGEN_INTERVAL;
		}
		else {
			secondRegenTimer -= DeltaTime;
		}
				
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//pass score to music manager
	musicManager->score = score;
	
	//determine vocal volume and pass to music manager
	CalcVocalVolume();	
	
	//heal the player over time
	RegenPlayerHealth(DeltaTime);
	
	//if thers a change in player falling state, and player stops falling, play landing event
	if (!isFalling)
	{
		if (movementComponent->IsFalling())
		{
			isFalling = true;
		}
	}else
	{
		if (!movementComponent->IsFalling())
		{
			isFalling = false;
			UAkGameplayStatics::PostEvent(landEvent, this, int32(0), nullCallback);
		}
	}



	//if the player is moving on the ground, tick down the stepTimer, and when its less than 0, post event to play a footstep noise
	if (FVector(movementComponent->Velocity.X,movementComponent->Velocity.Y,movementComponent->Velocity.Z).Length() > 0)
	{
		if (movementComponent->IsMovingOnGround())
		{
			if (stepTimer <= 0)
			{
				UAkGameplayStatics::PostEvent(footstepEvent, this, int32(0), nullCallback);
				stepTimer = FOOT_STEP_INTERVAL;
			}else
			{
				stepTimer -= DeltaTime;
			}
		}
	}


	//line cast straight down to get player's position on the ground
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this);
	FHitResult hit;
	GetWorld()->LineTraceSingleByChannel(hit,GetActorLocation(),GetActorLocation()+-20000*GetActorUpVector(),ECC_Camera,queryParams);

	if (hit.bBlockingHit)
	{
		groundLocation = hit.ImpactPoint;
	}

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(moveIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(lookIA, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EnhancedInputComponent->BindAction(jumpIA, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(shootIA, ETriggerEvent::Started, this,&APlayerCharacter::Shoot);
	}
	
}

//move the player
void APlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D moveVector = Value.Get<FVector2D>();

	AddMovementInput(GetActorForwardVector(),moveVector.Y);
	AddMovementInput(GetActorRightVector(), moveVector.X);
}

//look around
void APlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D lookVector = Value.Get<FVector2D>();
	AddControllerYawInput(lookVector.X);
	AddControllerPitchInput(lookVector.Y);
}

void APlayerCharacter::Shoot(const FInputActionValue& Value)
{

	//check if the gun can shoot
	if (gun->CanShootGun())
	{
		//check to see if the shot was on the beat via music manager
		bool hasHit = musicManager->IsOnBeat();

		//if on the beat
		if (hasHit)
		{
			//increase beat multiplier to a maximum
			if (beatMultiplier< MULTIPLIER_MAX)
			{
				beatMultiplier+=1;
			}
		}else
		{
			//reset beat multiplier to 1
			beatMultiplier = 1;
		}

		//post WWise event for gunshot
		UAkGameplayStatics::PostEvent(gunshotEvent, this, int32(0), nullCallback);

		//activate muzzleflash particles
		muzzleFlash->Activate(true);

		//add recoil to gun model
		gun->AddRecoil();

		//create vfx for gunshot
		UNiagaraComponent * gunBeam = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),gunBeamTemplate,gunTip->GetComponentLocation(),FRotator::ZeroRotator,FVector(1),true,true);
		gunBeam->SetNiagaraVariableVec3(FString("endPos"),GetActorLocation()+camera->GetForwardVector()*gunRange);


		//line trace for gun shot and see if hit target
		FCollisionQueryParams queryParams;
		queryParams.AddIgnoredActor(this);
		FHitResult hit;
		GetWorld()->LineTraceSingleByChannel(hit,gunTip->GetComponentLocation(),GetActorLocation()+camera->GetForwardVector()*gunRange,ECC_Camera,queryParams);

		//if target hit
		if (hit.bBlockingHit && hit.GetActor())
		{
			//if target is an enemy
			if (Cast<AEnemy>(hit.GetActor()))
			{
				//increase hit multiplier to maximum
				if (hitMultiplier < MULTIPLIER_MAX)
				{
					hitMultiplier += 1;
				}

				//reduce enemy health, and if kills, increase score
				if (Cast<AEnemy>(hit.GetActor())->ReduceHealth(gunDamage))
				{
					score +=baseScoreAmount * (beatMultiplier+hitMultiplier);
				};
			}else
			{

				//if miss reset hit multiplier
				hitMultiplier = 1;
			}
		}
	}
}

void APlayerCharacter::ReduceHealth(int damage)
{
	//decrease health and reset regen timer
	regenTimer = REGEN_COOLDOWN;
	health -= damage;

	//play damaged sound
	UAkGameplayStatics::PostEvent(damagedEvent, this, int32(0), nullCallback);
}
