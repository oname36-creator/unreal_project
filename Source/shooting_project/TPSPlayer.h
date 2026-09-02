// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPSPlayer.generated.h"


UCLASS()
class SHOOTING_PROJECT_API ATPSPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATPSPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
public:
	UPROPERTY(VisibleAnywhere, Category=Camera)
	class USpringArmComponent* springArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* tpsCamComp;

public:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputMappingContext* imc_TPS;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<class UInputAction> ia_LookUp;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<class UInputAction> ia_Move;

	void Input_Move(const struct FInputActionValue& inputValue);

	void Input_Look(const struct FInputActionValue& inputValue);

	UPROPERTY(EditAnywhere, Category = "Input")
	float MeshVisibleDistance =100;
	
	UPROPERTY(VisibleAnywhere, Category = "GumMesh")
	TObjectPtr<class USkeletalMeshComponent> gunMeshComp;
	
	UPROPERTY(EditDefaultsOnly, Category=BulletFactory)
	TSubclassOf<class ABullet> bulletFactory;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<class UInputAction> ia_Fire;
	void InputFire(const struct FInputActionValue& inputValue);
	
	UPROPERTY(VisibleAnywhere, Category = GumMesh)
	TObjectPtr<class USkeletalMeshComponent> sniperGunComp;
	
	UPROPERTY(VisibleAnywhere, Category = GunMesh)
	TObjectPtr<class USkeletalMeshComponent> gunComp;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<class UInputAction> ia_WPchange;
	
	bool bUsingGrenade = false;
	
	void WPChange(const struct FInputActionValue& inputValue);
	void ChangeToGrenadeGun(const struct FInputActionValue& inputValue);
	void ChangeToSniperGun(const struct FInputActionValue& inputValue);
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<class UInputAction> ia_sniper;
	
	void SniperAim(const struct FInputActionValue& inputValue);
	
	bool bSniperAim = false;
	
	UPROPERTY(EditDefaultsOnly, Category = SniperUI)
	TSubclassOf<class UUserWidget> sniperUIFactory;
	
	UPROPERTY()
	TObjectPtr<class UUserWidget> _sniperUI;
	
	UPROPERTY(EditAnywhere, Category = BulletEffect)
	TObjectPtr<class UNiagaraSystem> bulletEffectFactory;
	
	UPROPERTY(EditAnywhere, Category = SniperUI)
	TSubclassOf<class UUserWidget> crosshairUIFactory;
	
	UPROPERTY()
	TObjectPtr<class UUserWidget> _crosshairUI;

};
