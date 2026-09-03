// Fill out your copyright notice in the Description page of Project Settings.

#include "TPSPlayer.h"
#include "Public/MyAnimInstance.h"

#include "GameFramework/CharacterMovementComponent.h"


void UMyAnimInstance::NativeInitializeAnimation()
{
	
	Super::NativeInitializeAnimation();
	Character= Cast<ATPSPlayer>(TryGetPawnOwner());
	if (Character == nullptr)
	{
		return;
	}
	
	// 초기화 될때 캐싱해두기
	
	MovementComponent = Character->GetCharacterMovement();
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (Character== nullptr)
	{
		return;
	}
		
	
	if (Character && MovementComponent)
	{
		// 속도 계산
		Velocity = Character->GetVelocity();
		GroundSpeed = Velocity.Length();
		bShouldMove = (MovementComponent->GetCurrentAcceleration().Size() > 0) && (GroundSpeed >= 0.01f);
		bIsFalling = MovementComponent->IsFalling();
		
		// 방향 계산
		Direction = CalculateDirection(Velocity, Character->GetActorRotation());
		
		
	}
}

void UMyAnimInstance::PlayAttackAnim()
{
	bisFire = true; 
	Montage_Play(AttackAnimMontage);
	
	if (GetWorld()->GetTimerManager().IsTimerActive(OnFireTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(OnFireTimer);
	}
	
	GetWorld()->GetTimerManager().SetTimer(OnFireTimer,
		FTimerDelegate::CreateLambda([this]
		{
			bisFire = false;
		}), 
		FireMotionTimer,
		false
	);
}
