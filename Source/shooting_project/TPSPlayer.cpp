// Fill out your copyright notice in the Description page of Project Settings.


#include "TPSPlayer.h"
#include "Bullet.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
ATPSPlayer::ATPSPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Game/Assets/ThirdPersonTemplate/Characters/Mannequins/Meshes/SK_Mannequin.SK_Mannequin"));
	if (TempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(TempMesh.Object);

		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -90), FRotator(0, -90, 0));
	}

	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	springArmComp->SetupAttachment(RootComponent);
	//springArmComp->SetRelativeLocation(FVector(0,70,90));
	springArmComp->SocketOffset = FVector(0,70,90);
	springArmComp->TargetArmLength = 400;
	springArmComp->bUsePawnControlRotation = true;
	
	tpsCamComp = CreateDefaultSubobject<UCameraComponent>(TEXT("TpsCamComp"));
	tpsCamComp->SetupAttachment(springArmComp);
	tpsCamComp->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = true;
	
	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	gunMeshComp->SetupAttachment(GetMesh());
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("'SkeletalMesh'/Game/Assets/MilitaryWeapSilver/Weapons/Assault_Rifle_A.Assault_Rifle_A"));
	
	if (TempGunMesh.Succeeded())
	{
		gunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		gunMeshComp->SetRelativeLocation(FVector(-14, 11, 138));
	}
	
	sniperGunComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SniperGunComp"));
	
	sniperGunComp->SetupAttachment(GetMesh());
	
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempSniperMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Assets/MilitaryWeapSilver/Weapons/Sniper_Rifle_A.Sniper_Rifle_A'"));
	if (TempSniperMesh.Succeeded())
	{
		sniperGunComp->SetSkeletalMesh(TempSniperMesh.Object);
		
		sniperGunComp->SetRelativeLocation(FVector(-22, 31, 128));
	}
}

// Called when the game starts or when spawned
void ATPSPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	auto pc = Cast<APlayerController>(Controller);
	if (pc)
	{
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());
		if (subsystem)
		{
			subsystem->AddMappingContext(imc_TPS, 0);
		}
	}
	_sniperUI = CreateWidget(GetWorld(), sniperUIFactory);
	bUsingGrenade =false;
	ChangeToSniperGun(FInputActionValue());
}

// Called every frame
void ATPSPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//카메라 거리, 캐릭터 거리 차
	// 거리값 으로 메시 숨김처리
	//FVector DistVector = _tpsCamComp->GetComponentLocation() - GetActorLocation();
	//double Dist = DistVector.Size();
	
	//if (Dist > MeshVisibleDistance)
	//{
		//Get()->SetVisibility(false);
	//}
	//else
	//{
		//Get()->SetVisibility(true);
	//}
}

// Called to bind functionality to input
void ATPSPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto PlayerInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (PlayerInput)
	{
		PlayerInput->BindAction(ia_Move, ETriggerEvent::Triggered, this, &ATPSPlayer::Input_Move);
		PlayerInput->BindAction(ia_LookUp, ETriggerEvent::Triggered, this, &ATPSPlayer::Input_Look);
		PlayerInput->BindAction(ia_Fire, ETriggerEvent::Started, this, &ATPSPlayer::InputFire);
		PlayerInput->BindAction(ia_WPchange, ETriggerEvent::Started, this, &ATPSPlayer::WPChange);
		PlayerInput->BindAction(ia_sniper, ETriggerEvent::Started, this, &ATPSPlayer::SniperAim);
		PlayerInput->BindAction(ia_sniper, ETriggerEvent::Completed, this, &ATPSPlayer::SniperAim);
	
	}

}

void ATPSPlayer::InputFire(const struct FInputActionValue& inputValue)
{
	if (bUsingGrenade)
	{
		FTransform firePosition = gunMeshComp->GetSocketTransform(TEXT("FirePosition"));
		GetWorld()->SpawnActor<ABullet>(bulletFactory, firePosition);
	}
	else
	{
		FVector startPos = tpsCamComp->GetComponentLocation();
		FVector endPos = tpsCamComp->GetComponentLocation() + tpsCamComp->GetForwardVector() * 5000;
		FHitResult hitInfo;
		FCollisionQueryParams params;
		params.AddIgnoredActor(this);
		
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
		
		if (bHit)
		{
			FTransform bulletTrans;
			bulletTrans.SetLocation(hitInfo.ImpactPoint);
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,bulletEffectFactory, hitInfo.ImpactPoint);
			
			auto hitComp = hitInfo.GetComponent();
			
			if (hitComp && hitComp->IsSimulatingPhysics())
			{
				FVector dir = (endPos - startPos).GetSafeNormal();
				FVector force = dir * hitComp->GetMass() * 500000;
				
				hitComp->AddForceAtLocation(force, hitInfo.ImpactPoint);
			}
		}
	}
}

void ATPSPlayer::Input_Look(const FInputActionValue& inputValue)
{
	const FVector2D Value = inputValue.Get<FVector2D>();

	if (Value.X != 0.0F)
	{
		AddControllerYawInput(Value.X);
	}
	if (Value.Y != 0.0F)
	{
		AddControllerPitchInput(Value.Y);
	}
}

void ATPSPlayer::Input_Move(const FInputActionValue& inputValue)
{
	if (Controller)
	{
		const FVector2D Value = inputValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void ATPSPlayer::WPChange(const FInputActionValue& inputValue)
{
	if (bUsingGrenade)
	{
		ChangeToSniperGun(inputValue);
	}
	else
	{
		ChangeToGrenadeGun(inputValue);
	}
	bUsingGrenade = !bUsingGrenade;
}

void ATPSPlayer::ChangeToGrenadeGun(const FInputActionValue& inputValue)
{
	sniperGunComp->SetVisibility(false);
	gunMeshComp->SetVisibility(true);
}

void ATPSPlayer::ChangeToSniperGun(const FInputActionValue& inputValue)
{
	sniperGunComp->SetVisibility(true);
	gunMeshComp->SetVisibility(false);
}

void ATPSPlayer::SniperAim(const FInputActionValue& inputValue)
{
	if (bUsingGrenade)
	{
		return;
	}
	if (bSniperAim == false)
	{
		bSniperAim = true;
		
		_sniperUI->AddToViewport();
		tpsCamComp->SetFieldOfView(45.0f);
	}
	else
	{
		bSniperAim = false;
		_sniperUI->RemoveFromParent();
		tpsCamComp->SetFieldOfView(90.0f);
	}
}


