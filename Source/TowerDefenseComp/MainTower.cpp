// Fill out your copyright notice in the Description page of Project Settings.


#include "MainTower.h"

// Sets default values
AMainTower::AMainTower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMainTower::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMainTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

