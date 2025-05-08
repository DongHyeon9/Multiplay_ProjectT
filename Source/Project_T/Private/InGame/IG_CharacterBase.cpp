// Fill out your copyright notice in the Description page of Project Settings.


#include "InGame/IG_CharacterBase.h"


// Sets default values
AIG_CharacterBase::AIG_CharacterBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AIG_CharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AIG_CharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AIG_CharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

