
#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/Struct/AATCalendar.h"
#include "AATCalendarBP.generated.h"

UCLASS()
class ANDROIDATTACHTHING_API UAATCalendarBP : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// 设置一个日程
	UFUNCTION(BlueprintCallable, Category = "AndroidGoodies")
		static bool SetCalendarData(FAATCalendar AATCalendar);

	// 删除一个日程
	UFUNCTION(BlueprintCallable, Category = "AndroidGoodies")
		static bool RemoveCalendarData(FString Title, FDateTime StrTime);

	// 获取所有日程
	UFUNCTION(BlueprintCallable, Category = "AndroidGoodies")
		static TArray<FAATCalendar> GetAllCalendarsArr();
};