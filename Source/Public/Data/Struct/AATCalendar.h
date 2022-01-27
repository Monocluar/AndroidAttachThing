
#pragma once

#if PLATFORM_ANDROID
#include "Android/AndroidJava.h"
#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#endif

#include "AATCalendar.generated.h"

USTRUCT(BlueprintType)
struct ANDROIDATTACHTHING_API FAATCalendar
{
	GENERATED_BODY()

public:

	// 标题
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AndroidAttachThing|Contacts")
		FString Title;

	// 附加信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AndroidAttachThing|Contacts")
		FString AddInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AndroidAttachThing|Contacts")
		FString Place;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AndroidAttachThing|Contacts")
		int32 EventStatus;

	// 起始时间
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AndroidAttachThing|Contacts")
		FDateTime StartDatetTime;

	// 是否需要邮件服务
	bool bNeedMailService;
};

