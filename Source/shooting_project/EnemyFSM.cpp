// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "TPSPlayer.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "shooting_project.h"
#include "Components/CapsuleComponent.h"
#include "Public/EnemyAnim.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "Splines/SplineMath.h"

// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), ATPSPlayer::StaticClass());
	
	target = Cast<ATPSPlayer>(actor);
	
	me = Cast<AEnemy>(GetOwner());
	// ...
	anim = Cast<UEnemyAnim>(me->GetMesh()->GetAnimInstance());
	
	ai = Cast<AAIController>(me->GetController());
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	FString logMsg = UEnum::GetValueAsString(State);
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Cyan, logMsg);

	switch (State)
	{
	case EEnemyState::Idle:
		IdleState();
		break;
	case EEnemyState::Move:
		MoveState();
		break;
	case EEnemyState::Attack:
		AttackState();
		break;
	case EEnemyState::Damage:
		DamageState();
		break;
	case EEnemyState::Die:
		DieState();
		break;
	}
	// .
}

void UEnemyFSM::IdleState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;
	
	if (currentTime > idleDelay)
	{
		State = EEnemyState::Move;
		currentTime = 0;
		
		anim->animState = State;
		
		GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
	}
}

void UEnemyFSM::MoveState()
{
	FVector destination = target->GetActorLocation();
	
	FVector dir = destination - me -> GetActorLocation();
	
	//me->AddMovementInput(dir.GetSafeNormal());
	
	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	
	FPathFindingQuery query;
	FAIMoveRequest req;
	
	req.SetAcceptanceRadius(3);
	req.SetGoalLocation(destination);
	
	ai->BuildPathfindingQuery(req, query);
	
	FPathFindingResult r= ns->FindPathSync(query);
	
	if (r.Result == ENavigationQueryResult::Success)
	{
		ai->MoveToLocation(destination);
	}
	else
	{
		auto result = ai->MoveToLocation(randomPos);
		
		if (result == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
		}
	}
	
	if (dir.Size() < attackRange)
	{
		ai->StopMovement();
		State = EEnemyState::Attack;
		
		anim->animState = State;
		anim->bAttackPlay = true;
		currentTime = attackDelayTime;
	}
}

void UEnemyFSM::AttackState()
{
	FVector dirToPlayer = target->GetActorLocation() - me->GetActorLocation();
	dirToPlayer = dirToPlayer.GetSafeNormal();
	float dot = FVector::DotProduct(dirToPlayer, me->GetActorForwardVector());
	
	if (currentTime > attackDelayTime && dot >= threshold)
	{
		
	
		PRINT_LOG(TEXT("Attack!!!"));
		currentTime = 0;
		
		anim->bAttackPlay = true;
	}
	else if ( anim->bAttackPlay == false)
	{
		currentTime += GetWorld()->DeltaTimeSeconds;
		FVector cross = FVector::CrossProduct(me->GetActorForwardVector(), dirToPlayer);
		float rotateDir = (cross.Z >= 0 ) ? 1.0F : -1.0F;
		
		FRotator currentRot = me->GetActorRotation();
		currentRot.Yaw += rotateDir * 1.0f;
		me->SetActorRotation(currentRot);
	}
	float distatnce = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());
	
	if (distatnce > attackRange && anim->bAttackPlay == false)
	{
		

		State = EEnemyState::Move;
		anim->animState = State;
		
		GetRandomPositionInNavMesh(me->GetActorLocation(), 500, randomPos);
	}
}
void UEnemyFSM::OnDamageProcess()
{
	hp--;
	if (hp > 0)
	{
		State = EEnemyState::Damage;
		
		currentTime =0;
		
		int32 index = FMath::RandRange(0, 1);
		FString sectionName = FString::Printf(TEXT("Damge%d"), index);
		anim->PlayDamageAnim(FName(*sectionName));
	}
	else
	{
		State = EEnemyState::Die;
		
		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		anim->PlayDamageAnim(TEXT("Die"));
	}
	anim->animState = State;
	ai->StopMovement();
}



void UEnemyFSM::DamageState()
{
	currentTime += GetWorld()->DeltaTimeSeconds;
	
	if (currentTime > damageDelayTime)
	{
		State= EEnemyState::Idle;
		currentTime = 0;
		
		anim->animState = State;
	}
}

void UEnemyFSM::DieState()
{
	if (anim->bDieDone == false)
	{
		return;
	}
	
	FVector P0 = me->GetActorLocation();
	FVector vt = FVector::DownVector * dieSpeed * GetWorld()->DeltaTimeSeconds;
	FVector P = P0 + vt;
	me->SetActorLocation(P);
	
	if (P.Z < -200.0f)
	{
		me->Destroy();
	}
}

bool UEnemyFSM::GetRandomPositionInNavMesh(FVector centerLocation, float radius, FVector& dest)
{
	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	FNavLocation loc;
	bool result = ns->GetRandomReachablePointInRadius(centerLocation, radius, loc);
	dest = loc.Location;
	return result;
}