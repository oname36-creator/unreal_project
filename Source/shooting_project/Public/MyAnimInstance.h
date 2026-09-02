// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTING_PROJECT_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	void NativeInitializeAnimation();
	void NativeUpdateAnimation(float DeltaSeconds);
	void PlayAttackAnim();

public:

		
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim, meta=(AllowPrivateAccess=true))
	TObjectPtr<class ACharacter> Character;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim, meta= (AllowPrivateAccess=true))
	TObjectPtr<class UCharacterMovementComponent> MovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim, meta= (AllowPrivateAccess=true))
	FVector Velocity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim, meta= (AllowPrivateAccess=true))
	float GroundSpeed = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim, meta= (AllowPrivateAccess=true))
	float Direction = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim, meta= (AllowPrivateAccess=true))
	bool bShouldMove = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim, meta= (AllowPrivateAccess=true))
	bool bIsFalling = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerAnim)
	TObjectPtr<class UAnimMontage> AttackAnimMontage;
	

};
