// Fill out your copyright notice in the Description page of Project Settings.

#include "TestUDP.h"




// Sets default values
ATestUDP::ATestUDP()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	ListenSocket = NULL;
	PrimaryActorTick.bCanEverTick = true;
	//Socket = new FSocket(SOCKTYPE_Datagram);

}

// Called when the game starts or when spawned
void ATestUDP::BeginPlay()
{
	Super::BeginPlay();
	StartUDPReceiver("", "0.0.0.0", port);
}

// Called every frame
void ATestUDP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


bool ATestUDP::StartUDPReceiver(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort) {
	UE_LOG(LogTemp, Warning, TEXT("Starting UDP Receiver"))

		//~~~

		FIPv4Address Addr;
	FIPv4Address::Parse(TheIP, Addr);

	//Create Socket
	FIPv4Endpoint Endpoint(Addr, ThePort);

	//BUFFER SIZE
	int32 BufferSize = 2 * 1024 * 1024;

	ListenSocket = FUdpSocketBuilder(*YourChosenSocketName)
		.AsNonBlocking()
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.WithReceiveBufferSize(BufferSize);
	;

	check(ListenSocket != nullptr)

	FTimespan ThreadWaitTime = FTimespan::FromMilliseconds(100);
	UDPReceiver = new FUdpSocketReceiver(ListenSocket, ThreadWaitTime, TEXT("UDP Receiver"));
	UDPReceiver->OnDataReceived().BindUObject(this, &ATestUDP::Recv);
	UDPReceiver->Start();

	return true;
}

FString ATestUDP::BytesToStringFixed(const uint8 *In, int32 Count)
{
	FString Broken = BytesToString(In, Count);
	FString Fixed;

	for (int i = 0; i < Broken.Len(); i++)
	{
		const TCHAR c = Broken[i] - 1;
		Fixed.AppendChar(c);
	}

	return Fixed;
}


void ATestUDP::Recv(const FArrayReaderPtr& ArrayReaderPtr, const FIPv4Endpoint& EndPt)
{
	UE_LOG(LogTemp, Warning, TEXT("Received bytes"))
		//ScreenMsg("Received bytes", ArrayReaderPtr->Num());

	TArray<uint8> Data;
	Data.AddUninitialized(ArrayReaderPtr->TotalSize());
	ArrayReaderPtr->Serialize(Data.GetData(), ArrayReaderPtr->TotalSize());
	//*ArrayReaderPtr << Data;		//Now de-serializing! See AnyCustomData.h

	//BP Event
	OnDataReceived(Data, BytesToStringFixed(Data.GetData(), ArrayReaderPtr->TotalSize()));
}


void ATestUDP::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	//~~~~~~~~~~~~~~~~
	UDPReceiver->Stop();
	delete UDPReceiver;
	UDPReceiver = nullptr;

	//Clear all sockets!
	//		makes sure repeat plays in Editor dont hold on to old sockets!
	if (ListenSocket)
	{
		ListenSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
	}
}

