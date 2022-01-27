
#pragma once

#include "Modules/ModuleManager.h"

class IAndroidAttachThingModule : public IModuleInterface
{
public:

	static inline IAndroidAttachThingModule& Get()
	{
		return FModuleManager::LoadModuleChecked< IAndroidAttachThingModule >("AndroidAttachThing");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("AndroidGoodies");
	}
};
