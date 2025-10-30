#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PersistenceHttpClient.generated.h"

DECLARE_DELEGATE_TwoParams(FOnPersistenceResponse, bool /*bOK*/, const FString& /*PayloadOrError*/);

UCLASS()
class UPersistenceHttpClient : public UObject
{
    GENERATED_BODY()

public:
    void PostSave(const FString& Url, const FString& EncryptedBase64, FOnPersistenceResponse Callback);
    void PostLoad(const FString& Url, const FString& EncryptedBase64, FOnPersistenceResponse Callback);
};
