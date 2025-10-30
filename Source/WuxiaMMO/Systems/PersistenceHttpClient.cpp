#include "Systems/PersistenceHttpClient.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

namespace
{
    void SendJson(const FString& Url, const FString& Payload, FOnPersistenceResponse Callback)
    {
        FHttpModule* Http = &FHttpModule::Get();
        if (!Http)
        {
            Callback.ExecuteIfBound(false, TEXT("HTTP module unavailable"));
            return;
        }

        TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
        Request->SetURL(Url);
        Request->SetVerb(TEXT("POST"));
        Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
        Request->SetContentAsString(Payload);
        Request->OnProcessRequestComplete().BindLambda([Callback](FHttpRequestPtr, FHttpResponsePtr Response, bool bSuccess)
        {
            if (!bSuccess || !Response.IsValid())
            {
                Callback.ExecuteIfBound(false, TEXT("No response"));
                return;
            }

            const bool bHttpOk = Response->GetResponseCode() >= 200 && Response->GetResponseCode() < 300;
            Callback.ExecuteIfBound(bHttpOk, Response->GetContentAsString());
        });
        Request->ProcessRequest();
    }
}

void UPersistenceHttpClient::PostSave(const FString& Url, const FString& EncryptedBase64, FOnPersistenceResponse Callback)
{
    const FString Body = FString::Printf(TEXT("{\"payload\":\"%s\"}"), *EncryptedBase64);
    SendJson(Url, Body, Callback);
}

void UPersistenceHttpClient::PostLoad(const FString& Url, const FString& EncryptedBase64, FOnPersistenceResponse Callback)
{
    const FString Body = FString::Printf(TEXT("{\"payload\":\"%s\"}"), *EncryptedBase64);
    SendJson(Url, Body, Callback);
}
