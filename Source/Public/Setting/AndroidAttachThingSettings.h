
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "AndroidAttachThingSettings.generated.h"

UCLASS(Config = Engine, DefaultConfig)
class ANDROIDATTACHTHING_API UAndroidAttachThingSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** 文件夹，文件应从中复制到 "./Android/res/drawable" 文件夹 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Android Attach Thing Settings | Drawables")
		FDirectoryPath AndroidDrawablesFolder;

	/** 将写日程权限添加到Android清单 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Android Attach Thing Settings | Permissions")
		bool bWriteCalendarPermission;

	/** 将读取日程权限添加到Android清单 */
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Android Attach Thing Settings | Permissions")
		bool bReadCalendarPermission;
};