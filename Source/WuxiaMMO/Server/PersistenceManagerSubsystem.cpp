#include "Server/PersistenceManagerSubsystem.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/ScopeLock.h"

bool UPersistenceManagerSubsystem::InitFromConfig()
{
    GConfig->GetString(TEXT("Wuxia.Server"), TEXT("DbHost"), ConnHost, GGameIni);
    GConfig->GetInt   (TEXT("Wuxia.Server"), TEXT("DbPort"), ConnPort, GGameIni);
    GConfig->GetString(TEXT("Wuxia.Server"), TEXT("DbName"), ConnDb, GGameIni);
    GConfig->GetString(TEXT("Wuxia.Server"), TEXT("DbUser"), ConnUser, GGameIni);
    GConfig->GetString(TEXT("Wuxia.Server"), TEXT("DbPass"), ConnPass, GGameIni);
    return !ConnHost.IsEmpty() && !ConnDb.IsEmpty() && !ConnUser.IsEmpty();
}

// NOTE: ExecSQL is a stub. Replace with libpq or an embedded client. For now, return false to indicate no runtime DB until wired.
bool UPersistenceManagerSubsystem::ExecSQL(const FString& SQL, const TArray<FString>& Params, TArray<TArray<FString>>& OutRows)
{
    // TODO: integrate with libpq or db connector plugin. Stub returns false for now.
    return false;
}

bool UPersistenceManagerSubsystem::CreateAccount(const FAccountDTO& In, FString& OutAccountId)
{
    TArray<TArray<FString>> Rows;
    // Example parameterized upsert (pseudo). Replace with ExecSQL once connector is wired.
    // SQL: INSERT INTO account (id,email,pwd_hash,role) VALUES (uuid_generate_v4(),$1,$2,$3) RETURNING id;
    return false;
}

bool UPersistenceManagerSubsystem::CreateCharacter(const FCharacterDTO& In, FString& OutCharacterId)
{
    // Example insert + container init, etc. Stubbed for now.
    return false;
}

bool UPersistenceManagerSubsystem::IsAccountBanned(const FString& AccountId, bool& bOutBanned)
{
    // SELECT 1 FROM ban_list WHERE account_id=$1 AND (expires_at IS NULL OR expires_at>now()) LIMIT 1;
    bOutBanned = false;
    return true;
}

bool UPersistenceManagerSubsystem::LogAudit(const FString& ActorAccountId, const FString& Action, const FString& PayloadJson)
{
    // INSERT INTO audit_log (actor_account, action, payload) VALUES ($1,$2,$3);
    return true;
}
