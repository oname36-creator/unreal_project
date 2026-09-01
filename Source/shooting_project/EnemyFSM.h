// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle,
	Move ,
	Attack ,
	Damage,
	Die ,
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOTING_PROJECT_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FSM)
	EEnemyState State = EEnemyState::Idle;
	
	void IdleState();
	
	void MoveState();
	
	void AttackState();
	
	void DamageState();
	
	void DieState();
	
	UPROPERTY(EditDefaultsOnly, Category = FSM)
	float idleDelay = 2;
	float currentTime = 0;
	
	UPROPERTY(VisibleAnywhere, Category = FSM)
	TObjectPtr <class ATPSPlayer> target;
	
	UPROPERTY()
	TObjectPtr<class AEnemy> me;
	
	UPROPERTY(EditAnywhere, Category = FSM)
	float attackRange = 150.0f;
	
	UPROPERTY(EditAnywhere, Category = FSM)
	float attackDelayTime = 2.0f;
	
	void OnDamageProcess();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FSM)
	int32 hp = 3;
	
	UPROPERTY(EditAnywhere, Category = FSM)
	float damageDelayTime = 2.0f;
	
	UPROPERTY(EditAnywhere, Category = FSM)
	float dieSpeed = 50.0f;
};
