// Fill out your copyright notice in the Description page of Project Settings.


#include "PathObstacle.h"

// Sets default values
APathObstacle::APathObstacle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Tags.Add(FName("PathObstacle"));

}

// Called when the game starts or when spawned
void APathObstacle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
/*void APathObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}*/

