// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "curlHttpHandler.generated.h"

UCLASS()
class CURLHTTP_API AcurlHttpHandler : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AcurlHttpHandler();
	~AcurlHttpHandler();

	UFUNCTION(BlueprintCallable, Category = "curlHttp")
	void PostRequest(const FString& URL, const FString& Parameters);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDataReceivedDelegate, FString, ReceivedData, int64, MsgSize);

	UPROPERTY(BlueprintAssignable, Category = "curlHttp")
	FDataReceivedDelegate onDataReceived;


	

	UFUNCTION(BlueprintCallable, Category = "curlHttp")
	void PostFileRequest(const FString& URL, const FString& Parameter, const FString& FilePath);
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPostFileReceivedDelegate, FString, PostFileReceivedData);
	
	UPROPERTY(BlueprintAssignable, Category = "curlHttp")
	FPostFileReceivedDelegate onPostFileReceived;


private:
	static size_t StreamCallback(void* contents, size_t size, size_t numItems, void* userData);
	static size_t PostFileCallback(void* contents, size_t size, size_t numItems, void* userData);
};
