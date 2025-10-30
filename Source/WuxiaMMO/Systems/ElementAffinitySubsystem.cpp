#include "Systems/ElementAffinitySubsystem.h"
#include "Kismet/GameplayStatics.h"

bool UElementAffinitySubsystem::CanChangeAffinity(const FString& CharacterID) const
{
    if (const double* LastChange = LastChangeTimestamps.Find(CharacterID))
    {
        double Current = FPlatformTime::Seconds();
        return (Current - *LastChange) > CooldownSeconds;
    }
    return true;
}

void UElementAffinitySubsystem::RequestAffinityChange(const FString& CharacterID, EElementType NewElement)
{
    if (!CanChangeAffinity(CharacterID))
        return;

    // TODO: Deduct Qi offering and trigger element change logic server-side.
    LastChangeTimestamps.Add(CharacterID, FPlatformTime::Seconds());
}

float UElementAffinitySubsystem::GetRemainingCooldown(const FString& CharacterID) const
{
    if (const double* LastChange = LastChangeTimestamps.Find(CharacterID))
    {
        double Current = FPlatformTime::Seconds();
        double Remaining = CooldownSeconds - (Current - *LastChange);
        return FMath::Max(0.0, (float)Remaining);
    }
    return 0.0f;
}
