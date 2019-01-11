// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Networking.h"
#include "SocketSubsystem.h"
#include "TestUDP.generated.h"

UCLASS()
class VIDAO_RA_UNR_RECEIVE_API ATestUDP : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATestUDP();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	FSocket* ListenSocket;
	FUdpSocketReceiver* UDPReceiver = nullptr;

	bool StartUDPReceiver(
		const FString& YourChosenSocketName,
		const FString& TheIP,
		const int32 ThePort
	);

	void Recv(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt);

	UFUNCTION(BlueprintImplementableEvent)
		void BPEvent_DataReceived(const FString& ReceivedData);


	/** Called whenever this actor is being removed from a level */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};
