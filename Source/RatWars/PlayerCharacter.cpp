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

	
	gunActor = CreateDefaultSubobject<UChildActorComponent>(TEXT("GunActor"));
	gunActor->SetupAttachment(camera);
	gunTip = CreateDefaultSubobject<USceneComponent>(TEXT("GunTip"));
	gunTip->SetupAttachment(gunActor);
	muzzleFlash = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MuzzleFlash"));
	muzzleFlash->SetupAttachment(gunTip);
	
	movementComponent = GetCharacterMovement();

	beatMultiplier = 1;
	hitMultiplier = 1;
	
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

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

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	musicManager->score = score;

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

	if (gun->CanShootGun())
	{
		bool hasHit = musicManager->IsOnBeat();

		if (hasHit)
		{
			if (beatMultiplier< 8)
			{
				beatMultiplier+=1;
			}
		}else
		{
			beatMultiplier = 1;
		}
		
		UAkGameplayStatics::PostEvent(gunshotEvent, this, int32(0), nullCallback);
		muzzleFlash->Activate(true);
		gun->AddRecoil();

		UNiagaraComponent * gunBeam = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),gunBeamTemplate,gunTip->GetComponentLocation(),FRotator::ZeroRotator,FVector(1),true,true);
		gunBeam->SetNiagaraVariableVec3(FString("endPos"),GetActorLocation()+camera->GetForwardVector()*gunRange);

		
		FCollisionQueryParams queryParams;
		queryParams.AddIgnoredActor(this);
		FHitResult hit;
		GetWorld()->LineTraceSingleByChannel(hit,gunTip->GetComponentLocation(),GetActorLocation()+camera->GetForwardVector()*gunRange,ECC_Camera,queryParams);

		if (hit.bBlockingHit && hit.GetActor())
		{
			if (Cast<AEnemy>(hit.GetActor()))
			{
				if (hitMultiplier < 8)
				{
					hitMultiplier += 1;
				}
				if (Cast<AEnemy>(hit.GetActor())->ReduceHealth(gunDamage))
				{
					score +=baseScoreAmount * (beatMultiplier+hitMultiplier);
				};
			}else
			{
				hitMultiplier = 1;
			}
		}
	}
}
