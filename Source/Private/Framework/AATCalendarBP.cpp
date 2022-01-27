#include "Framework/AATCalendarBP.h"

#include "AATMethodCallUtils.h"
#include "Utils/AndroidJniObjectConverter.h"

static const ANSICHAR* AATHardwareClassName = "com/Monocular/AndroidAttachThing/AATCalendarBP";

bool UAATCalendarBP::SetCalendarData(FAATCalendar AATCalendar)
{
#if PLATFORM_ANDROID
	TSharedPtr<FJavaClassObject> JavaContact = FAndroidJniObjectConverter::UStructToJniObject(AATCalendar);

	bool Value = AATMethodCallUtils::CallStaticBoolMethod(AATHardwareClassName, "PushAppointmentsToCalender", "(Landroid/app/Activity;Lcom/Monocular/AndroidAttachThing/AATCalendar;)Z",
		FJavaWrapper::GameActivityThis, JavaContact->GetJObject());
	
	JavaContact.Reset();
	return Value;

#endif
	return false;
}

bool UAATCalendarBP::RemoveCalendarData(FString Title, FDateTime StrTime)
{
#if PLATFORM_ANDROID

	bool Value = AATMethodCallUtils::CallStaticBoolMethod(AATHardwareClassName, "DeleteCalendarEvent", "(Landroid/app/Activity;Ljava/lang/String;J)Z",
		FJavaWrapper::GameActivityThis, FAndroidJniObjectConverter::GetJavaString(Title), FAndroidJniObjectConverter::FDataTimeToJinLong(StrTime));

	return Value;
#endif
	return false;
}

TArray<FAATCalendar> UAATCalendarBP::GetAllCalendarsArr()
{
	TArray<FAATCalendar> Result;

#if PLATFORM_ANDROID
	jobjectArray contactsArray = static_cast<jobjectArray>(AATMethodCallUtils::CallStaticObjectMethod(AATHardwareClassName, "GetAllCalendarEvent",
		"(Landroid/app/Activity;)[Lcom/Monocular/AndroidAttachThing/AATCalendar;", FJavaWrapper::GameActivityThis));

	Result = FAndroidJniObjectConverter::UStructArrayFromJavaArray<FAATCalendar>(contactsArray);
	return Result;
#endif

	return Result;
}

