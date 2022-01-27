#pragma once

#include "CoreMinimal.h"
#include "UObject/Class.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJava.h"
#include "Android/AndroidApplication.h"
#include "Android/AndroidJNI.h"
#endif

#if PLATFORM_ANDROID
class ANDROIDATTACHTHING_API FAndroidJniObjectConverter
{
public:

	static jstring GetJavaString(const FString& InString);

	static FString FromJavaString(jstring JavaString);

public:

	/** FName案例的不敏感性会使Upperties的案例变得不可预测。尝试标准化输出. */
	static FString StandardizeCase(const FString& StringIn);

	/** FDataTime To JinLong */
	static int64 FDataTimeToJinLong(const FDateTime& InDataTime);

	static FDateTime FDataTimeFormJinLong(const int64& InDataTime);

public:

	static TSharedPtr<FJavaClassObject> UStructToJniObject(const UStruct* StructDefinition, const void* Struct, int64 CheckFlags = 0, int64 SkipFlags = 0);

	static void UPropertyToJniValue(JNIEnv* Env, jclass AGJniClass, jobject JniObject, FProperty* Property, void* Value, int64 CheckFlags, int64 SkipFlags);

	/**
	 * UstructOjsonObject的模板化版本，以尝试并利用大部分参数。也可用作示例用例
	 * 
	 * @param InStruct 要从中读取的UStruct实例
	 * @param ExportCb 用于覆盖导出行为的可选回调，如果返回null，则返回默认值
	 * @param CheckFlags 仅转换至少与其中一个标志匹配的属性。如果为0，请检查所有属性。
	 * @param SkipFlags 跳过与这些标志匹配的属性
	 * @return jobject pointer. 如果发生错误，则无效
	 */
	template<typename InStructType>
	static TSharedPtr<FJavaClassObject> UStructToJniObject(const InStructType& InStruct, int64 CheckFlags = 0, int64 SkipFlags = 0)
	{
		return UStructToJniObject(InStructType::StaticStruct(), &InStruct, CheckFlags, SkipFlags);
	}

public:

	static bool UStructFormJavaObject(const UStruct* StructDefinition, void* OutStruct, JNIEnv* Env, jobject JavaContact, int64 CheckFlags = 0 , int64 SkipFlags= 0 );

	static bool k2_StructTypeFormJavaObject(const UStruct* StructDefinition, void* OutStruct, JNIEnv* Env, jclass AGClass, jobject JavaContact, int64 CheckFlags = 0, int64 SkipFlags = 0 );

	template<typename InStructType>
	static InStructType UStructFormJavaObject(jobject JavaContact)
	{
		JNIEnv* Env = FAndroidApplication::GetJavaEnv();

		InStructType Value;

		if (!UStructFormJavaObject(InStructType::StaticStruct(), &Value, Env, JavaContact))
		{
			check(0);
		}

		return Value;
	}

	template<typename InStructType>
	static TArray<InStructType> UStructArrayFromJavaArray(jobjectArray JniArray)
	{

		TArray<InStructType> Result;

		if (JniArray == nullptr)
		{
			return Result;
		}

		JNIEnv* Env = FAndroidApplication::GetJavaEnv();

		UStruct* StructDefinition = InStructType::StaticStruct();

		FString JavaClassPath = FString::Printf(TEXT("com/ninevastudios/androidgoodies/%s"), *StructDefinition->GetFName().ToString());

		jclass AGClass = FAndroidApplication::FindJavaClass(TCHAR_TO_UTF8(*JavaClassPath));

		for (int32 i = 0; i < Env->GetArrayLength(JniArray); i++)
		{
			jobject JavaContact = Env->GetObjectArrayElement(JniArray, i);

			InStructType& Value = Result.AddDefaulted_GetRef();

			if (!k2_StructTypeFormJavaObject(StructDefinition, &Value, Env, AGClass, JavaContact))
			{
				check(0);
				continue;
			}
		}

		Env->DeleteLocalRef(AGClass);

		return Result;
	}


private:

	static bool UPropertyFormJniValue(FProperty* Property, void* OutStruct, JNIEnv* Env, jclass AGJniClass, jobject JavaContact);
};
#endif