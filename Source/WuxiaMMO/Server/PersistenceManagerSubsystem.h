#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Server/SyncJobTypes.h"
#include "PersistenceManagerSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FAccountDTO
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Email;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString PasswordHash; // Argon2id string; stored directly
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Role;         // player/mod/gm/admin
};

USTRUCT(BlueprintType)
struct FCharacterDTO
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Id;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString AccountId;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString FirstName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString LastName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString ClassId;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString CultivationMajor;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 CultivationMinor = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FString StatsBaseJson; // encrypted or plaintext JSON
};

UCLASS()
class UPersistenceManagerSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category="DB") bool InitFromConfig();
    UFUNCTION(BlueprintCallable, Category="DB") bool CreateAccount(const FAccountDTO& In, FString& OutAccountId);
    UFUNCTION(BlueprintCallable, Category="DB") bool CreateCharacter(const FCharacterDTO& In, FString& OutCharacterId);
    UFUNCTION(BlueprintCallable, Category="DB") bool IsAccountBanned(const FString& AccountId, bool& bOutBanned);
    UFUNCTION(BlueprintCallable, Category="DB") bool LogAudit(const FString& ActorAccountId, const FString& Action, const FString& PayloadJson);

    UFUNCTION(BlueprintCallable, Category="DB") bool UpsertPlayerBatch(const TArray<FSyncJob>& Jobs);
    UFUNCTION(BlueprintCallable, Category="DB") bool UpsertWorldBatch(const TArray<FSyncJob>& Jobs);
    UFUNCTION(BlueprintCallable, Category="DB") bool UpsertServerBatch(const TArray<FSyncJob>& Jobs);

private:
    FString ConnHost, ConnDb, ConnUser, ConnPass; int32 ConnPort=5432;
    bool ExecSQL(const FString& SQL, const TArray<FString>& Params, TArray<TArray<FString>>& OutRows); // placeholder
};
