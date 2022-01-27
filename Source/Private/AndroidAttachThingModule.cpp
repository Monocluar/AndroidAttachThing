
#include "IAndroidAttachThingModule.h"
#include "Setting/AndroidAttachThingSettings.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "AndroidAttachThing"

class FAndroidAttachThingModule : public IAndroidAttachThingModule
{

public:
	virtual void StartupModule() override;


	virtual void ShutdownModule() override;

private:

	UAndroidAttachThingSettings* AndroidAttachThingSettings;
};

IMPLEMENT_MODULE(FAndroidAttachThingModule, AndroidAttachThing)

void FAndroidAttachThingModule::StartupModule()
{
	FString PluginName = "AndroidAttachThing";

	AndroidAttachThingSettings = NewObject<UAndroidAttachThingSettings>(GetTransientPackage(), "AndroidAttachThingSettings", RF_Standalone);
	AndroidAttachThingSettings->AddToRoot();

	// Register settings
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "Android Attach Thing",
			LOCTEXT("RuntimeSettingsName", "Android Attach Thing"),
			LOCTEXT("RuntimeSettingsDescription", "Configure Android Attach Thing"),
			AndroidAttachThingSettings);
	}
}

void FAndroidAttachThingModule::ShutdownModule()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "Android Attach Thing");
	}

	if (!GExitPurge)
	{
		// If we're in exit purge, this object has already been destroyed
		AndroidAttachThingSettings->RemoveFromRoot();
	}
	else
	{
		AndroidAttachThingSettings = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE

