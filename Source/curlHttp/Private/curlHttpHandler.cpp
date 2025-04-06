#include "curlHttpHandler.h"

#include <iostream>
#include <thread>
#include "curl.h"
#include <string>
#include <mutex>

std::mutex curlMutex;

AcurlHttpHandler::AcurlHttpHandler()
{
	PrimaryActorTick.bCanEverTick = false;

	curl_global_init(CURL_GLOBAL_DEFAULT);
}

AcurlHttpHandler::~AcurlHttpHandler()
{
	curl_global_cleanup();
}

void AcurlHttpHandler::PostRequest(const FString& URL, const FString& Parameters)
{
	const FString callURL = URL;
	const FString callParameters = Parameters;
	const std::string callParam_std(TCHAR_TO_UTF8(*callParameters));
	CURL* curlHandle = curl_easy_init();
	curl_easy_setopt(curlHandle, CURLOPT_URL, TCHAR_TO_UTF8(*callURL));
	std::thread requestThread([curlHandle, callParam_std, this]
	{
		if (curlHandle)
		{
			curl_easy_setopt(curlHandle, CURLOPT_CUSTOMREQUEST, "POST");

			curl_easy_setopt(curlHandle, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curlHandle, CURLOPT_DEFAULT_PROTOCOL, "https");
			curl_slist* headers = NULL;
			headers = curl_slist_append(headers, "Content-Type: application/json; charset=UTF-8");
			curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, headers);

			const char* msg = callParam_std.c_str();
			curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, msg);
			curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, StreamCallback);
			curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, this);
			CURLcode result = curl_easy_perform(curlHandle);
			if (result != CURLE_OK)
			{
				UE_LOG(LogTemp, Warning, TEXT("----流式请求完成----"));
				curl_easy_cleanup(curlHandle);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("请求失败"));
			}
			curl_easy_cleanup(curlHandle);
		}
	});
	requestThread.detach();
}

size_t AcurlHttpHandler::StreamCallback(void* contents, size_t size, size_t numItems, void* userData)
{
	AcurlHttpHandler* obj = (AcurlHttpHandler*)userData;
	if (!obj)
	{
		return 0;
	}

	const size_t totalSize = size * numItems;
	if (totalSize > 0)
	{
		FString receivedData = FString(UTF8_TO_TCHAR((const char*)contents));
		obj->onDataReceived.Broadcast(receivedData, totalSize);
	}
	return totalSize;
}

void AcurlHttpHandler::PostFileRequest(const FString& URL, const FString& Parameter, const FString& FilePath)
{
	CURL* curl;
	CURLcode res;
	curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(curl, CURLOPT_URL, TCHAR_TO_UTF8(*URL));
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
		struct curl_slist* headers = NULL;
		headers = curl_slist_append(headers, "Content-Type: multipart/form-data");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_mime* mime;
		curl_mimepart* part;
		mime = curl_mime_init(curl);
		part = curl_mime_addpart(mime);
		curl_mime_name(part, TCHAR_TO_UTF8(*Parameter));
		curl_mime_filedata(part, TCHAR_TO_UTF8(*FilePath));
		//curl_mime_data(part, TCHAR_TO_UTF8(*FilePath), CURL_ZERO_TERMINATED);
		curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, PostFileCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			UE_LOG(LogTemp, Error, TEXT("cURL 请求失败: %s"), *FString(curl_easy_strerror(res)));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("cURL 请求成功"));
		}

		curl_mime_free(mime);
		curl_easy_cleanup(curl);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("无法初始化 cURL"));
	}
}

size_t AcurlHttpHandler::PostFileCallback(void* contents, size_t size, size_t numItems, void* userData)
{
	AcurlHttpHandler* obj = (AcurlHttpHandler*)userData;
	if (!obj)
	{
		return 0;
	}

	const size_t totalSize = size * numItems;

	// 将数据转换为 std::string，并添加空终止符
	std::string receivedString((const char*)contents, totalSize);
	FString receivedData = FString(UTF8_TO_TCHAR(receivedString.c_str()));

	UE_LOG(LogTemp, Warning, TEXT("Received Data: %s"), *receivedData);

	obj->onPostFileReceived.Broadcast(receivedData);

	return totalSize;
}

