#include "Setting/AndroidAttachThingSettings.h"


UAndroidAttachThingSettings::UAndroidAttachThingSettings(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bWriteCalendarPermission = false;
	bReadCalendarPermission = false;
}