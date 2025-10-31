#include "Environment/EnvBlueprintLibrary.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Components/LightComponent.h"

void UEnvBlueprintLibrary::ApplySunAngle(ADirectionalLight* Sun, float HourOfDay)
{
    if (!Sun) return;
    // Map 06:00 -> low horizon sunrise, 12:00 -> zenith, 18:00 -> sunset, 00:00 -> night
    const float DayFrac = FMath::Fmod(FMath::Max(0.f, HourOfDay), 24.f) / 24.f;
    const float Azimuth = DayFrac * 360.f;      // rotate around Yaw
    const float Elev    = FMath::Cos(DayFrac * 2.f * PI) * -45.f + 45.f; // simple elevation curve
    FRotator R( Elev, Azimuth, 0.f );
    Sun->SetActorRotation(R);
    if (ULightComponent* L = Sun->GetLightComponent())
    {
        static TMap<TWeakObjectPtr<ULightComponent>, float> BaseIntensity;
        const TWeakObjectPtr<ULightComponent> Key(L);
        float* Found = BaseIntensity.Find(Key);
        if (!Found)
        {
            Found = &BaseIntensity.Add(Key, L->Intensity);
        }
        const float DefaultIntensity = *Found;
        const bool bNight = (HourOfDay < 6.f || HourOfDay >= 22.f);
        L->SetIntensity(bNight ? 0.05f * DefaultIntensity : DefaultIntensity);
    }
}

void UEnvBlueprintLibrary::RefreshSkyLight(ASkyLight* Sky)
{
    if (!Sky) return;
    Sky->RecaptureSky();
}
