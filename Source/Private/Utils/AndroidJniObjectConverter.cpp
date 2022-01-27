#include "Utils/AndroidJniObjectConverter.h"

#if PLATFORM_ANDROID
namespace
{
	enum EValueState
	{
		InJniObject, // 输入JObject获取UProperty
		OutJniObject, // 输入UProperty获取JObject
	};

	template<typename T> const char* GetJniFieldStr(){ return  ""; }

	template<> const char* GetJniFieldStr<uint8>(){return "B";}
	template<> const char* GetJniFieldStr<int32>(){return "I";}
	template<> const char* GetJniFieldStr<float>(){return "F";}
	template<> const char* GetJniFieldStr<bool>(){return "Z";}
	template<> const char* GetJniFieldStr<int64>(){return "J";}
	template<> const char* GetJniFieldStr<FString>(){return "Ljava/lang/String;";}
	template<> const char* GetJniFieldStr<TArray<uint8>>(){return "[B";}
	template<> const char* GetJniFieldStr<TArray<int32>>(){return "[I"; }
	template<> const char* GetJniFieldStr<TArray<float>>(){return "[F"; }
	template<> const char* GetJniFieldStr<TArray<bool>>(){return "[Z"; }
	template<> const char* GetJniFieldStr<TArray<FString>>(){return "[Ljava/lang/String;";}


	template<typename InStructType>
	jfieldID GetNameFieldID(JNIEnv* Env, jclass AGJniClass, FProperty* Property)
	{
		return Env->GetFieldID(AGJniClass, TCHAR_TO_UTF8(*Property->GetFName().ToString()), GetJniFieldStr<InStructType>());
	}
	
	template<EValueState InState>
	void PropertyArrToJniObject(JNIEnv* Env, jclass AGJniClass, jobject JniObject, FArrayProperty* ArrayProperty, FProperty* InnerProperty, void* InValue, int64 CheckFlags = 0, int64 SkipFlags = 0)
	{
		if (!JniObject) return;
		FScriptArrayHelper Helper(ArrayProperty, InValue);
		if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(InnerProperty)) // 是否是枚举
		{
			if (InState == OutJniObject)
			{
				jbyteArray javaByteArray = (jbyteArray)Env->NewByteArray(Helper.Num());

				jbyte* javaByteArrayPtr = (jbyte*)malloc(Helper.Num() * sizeof(jbyte));
				for (int32 i = 0, n = Helper.Num(); i < n; ++i)
				{
					uint8 Value = (uint8)EnumProperty->GetUnderlyingProperty()->GetSignedIntPropertyValue(Helper.GetRawPtr(i));
					javaByteArrayPtr[i] = Value;
				}
				Env->SetByteArrayRegion(javaByteArray, 0, Helper.Num(), javaByteArrayPtr);
				Env->SetObjectField(JniObject, GetNameFieldID<TArray<uint8>>(Env, AGJniClass, InnerProperty), javaByteArray);

				free(javaByteArrayPtr);
				return;
			}
			else
			{

				jbyteArray JavaArray = static_cast<jbyteArray>(Env->GetObjectField(JniObject,GetNameFieldID<TArray<uint8>>(Env,AGJniClass,InnerProperty)));
				int32 ArraySize = Env->GetArrayLength(JavaArray);

				Helper.Resize(ArraySize);

				jbyte* javaBytePtr = Env->GetByteArrayElements(JavaArray, 0);

				for (int32 i = 0; i < ArraySize; i++)
				{
					EnumProperty->GetUnderlyingProperty()->SetIntPropertyValue(Helper.GetRawPtr(i), (int64)javaBytePtr[i]);
				}

				return;
			}
		}
		else if (FNumericProperty* NumericProperty = CastField<FNumericProperty>(InnerProperty)) // 是否是数字
		{
			// see if it's an enum
			UEnum* EnumDef = NumericProperty->GetIntPropertyEnum();
			if (EnumDef != NULL)
			{
				if (InState == OutJniObject)
				{
					jbyteArray javaByteArray = (jbyteArray)Env->NewByteArray(Helper.Num());

					jbyte* javaByteArrayPtr = (jbyte*)malloc(Helper.Num() * sizeof(jbyte));
					for (int32 i = 0, n = Helper.Num(); i < n; ++i)
					{
						uint8 Value = (uint8)NumericProperty->GetSignedIntPropertyValue(Helper.GetRawPtr(i));
						javaByteArrayPtr[i] = Value;
					}
					//Env->SetByteArrayRegion(javaByteArray, 0, Helper.Num(), javaByteArrayPtr);
					Env->SetObjectField(JniObject, GetNameFieldID<TArray<uint8>>(Env, AGJniClass, InnerProperty), javaByteArray);

					free(javaByteArrayPtr);
					return;
				}
				else
				{
					jbyteArray JavaArray = static_cast<jbyteArray>(Env->GetObjectField(JniObject,GetNameFieldID<TArray<uint8>>(Env,AGJniClass,InnerProperty)));
					int32 ArraySize = Env->GetArrayLength(JavaArray);

					Helper.Resize(ArraySize);

					jbyte* javaBytePtr = Env->GetByteArrayElements(JavaArray, 0);

					for (int32 i = 0; i < ArraySize; i++)
					{
						NumericProperty->SetIntPropertyValue(Helper.GetRawPtr(i), (int64)javaBytePtr[i]);
					}

					return;
				}
				
			}
			// 我们想把数字作为数字输出
			if (NumericProperty->IsFloatingPoint())
			{
				if (InState == OutJniObject)
				{
					jfloatArray javafloatArray = (jfloatArray)Env->NewFloatArray(Helper.Num());

					jfloat* javafloatArrayPtr = (jfloat*)malloc(Helper.Num() * sizeof(jfloat));
					for (int32 i = 0, n = Helper.Num(); i < n; ++i)
					{
						float Value = (float)NumericProperty->GetFloatingPointPropertyValue(Helper.GetRawPtr(i));
						javafloatArrayPtr[i] = Value;
					}
					//Env->SetFloatArrayRegion(javafloatArray, 0, Helper.Num(), javafloatArrayPtr);
					Env->SetObjectField(JniObject, GetNameFieldID<TArray<float>>(Env, AGJniClass, InnerProperty), javafloatArray);

					free(javafloatArrayPtr);
					return;
				}
				else
				{
					jfloatArray JavaArray = static_cast<jfloatArray>(Env->GetObjectField(JniObject,GetNameFieldID<TArray<float>>(Env,AGJniClass,InnerProperty)));
					int32 ArraySize = Env->GetArrayLength(JavaArray);

					Helper.Resize(ArraySize);

					jfloat* javaPtr = Env->GetFloatArrayElements(JavaArray, 0);

					for (int32 i = 0; i < ArraySize; i++)
					{
						NumericProperty->SetFloatingPointPropertyValue(Helper.GetRawPtr(i), (double)javaPtr[i]);
					}

					return;
				}
				
			}
			else if (NumericProperty->IsInteger())
			{
				if (InState == OutJniObject)
				{
					jintArray javaIntArray = (jintArray)Env->NewIntArray(Helper.Num());

					jint* javaIntArrayPtr = (jint*)malloc(Helper.Num() * sizeof(jint));
					for (int32 i = 0, n = Helper.Num(); i < n; ++i)
					{
						int32 Value = (int32)NumericProperty->GetSignedIntPropertyValue(Helper.GetRawPtr(i));
						javaIntArrayPtr[i] = Value;
					}
					//Env->SetIntArrayRegion(javaIntArray, 0, Helper.Num(), javaIntArrayPtr);
					Env->SetObjectField(JniObject, GetNameFieldID<TArray<int32>>(Env, AGJniClass, InnerProperty), javaIntArray);

					free(javaIntArrayPtr);
					return;
				}
				else
				{
					jintArray JavaArray = static_cast<jintArray>(Env->GetObjectField(JniObject,GetNameFieldID<TArray<int32>>(Env,AGJniClass,InnerProperty)));
					int32 ArraySize = Env->GetArrayLength(JavaArray);

					Helper.Resize(ArraySize);

					jint* javaPtr = Env->GetIntArrayElements(JavaArray, 0);

					for (int32 i = 0; i < ArraySize; i++)
					{
						NumericProperty->SetIntPropertyValue(Helper.GetRawPtr(i), (int64)javaPtr[i]);
					}

					return;
				}
			}
		}
		else if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(InnerProperty)) // 是否是布尔型
		{
			if (InState == OutJniObject)
			{
				jbooleanArray javaBooleanArray = (jbooleanArray)Env->NewBooleanArray(Helper.Num());

				jboolean* javaBooleanArrayPtr = (jboolean*)malloc(Helper.Num() * sizeof(jboolean));
				for (int32 i = 0, n = Helper.Num(); i < n; ++i)
				{
					int32 Value = (int32)BoolProperty->GetPropertyValue(Helper.GetRawPtr(i));
					javaBooleanArrayPtr[i] = Value;
				}
				//Env->SetBooleanArrayRegion(javaBooleanArray, 0, Helper.Num(), javaBooleanArrayPtr);

				Env->SetObjectField(JniObject, GetNameFieldID<TArray<bool>>(Env, AGJniClass, InnerProperty), javaBooleanArray);

				free(javaBooleanArrayPtr);
				return;
			}
			else
			{
				jbooleanArray JavaArray = static_cast<jbooleanArray>(Env->GetObjectField(JniObject,GetNameFieldID<TArray<bool>>(Env,AGJniClass,InnerProperty)));
				int32 ArraySize = Env->GetArrayLength(JavaArray);

				Helper.Resize(ArraySize);

				jboolean* javaPtr = Env->GetBooleanArrayElements(JavaArray, 0);

				for (int32 i = 0; i < ArraySize; i++)
				{
					BoolProperty->SetPropertyValue(Helper.GetRawPtr(i), (bool)javaPtr[i]);
				}

				return;
			}
		}
		else if (FStrProperty* StringProperty = CastField<FStrProperty>(InnerProperty))
		{
			if (InState == OutJniObject)
			{
				jobjectArray javaStringArray = (jobjectArray)Env->NewObjectArray(Helper.Num(), FJavaWrapper::JavaStringClass, nullptr);

				for (int32 i = 0, n = Helper.Num(); i < n; ++i)
				{
					Env->SetObjectArrayElement(javaStringArray, i, FAndroidJniObjectConverter::GetJavaString(StringProperty->GetPropertyValue(Helper.GetRawPtr(i))));
				}

				Env->SetObjectField(JniObject,GetNameFieldID<TArray<FString>>(Env, AGJniClass, InnerProperty), javaStringArray);

				return;
			}
			else
			{
				jobjectArray JavaArray = static_cast<jobjectArray>(Env->GetObjectField(JniObject, GetNameFieldID<TArray<FString>>(Env, AGJniClass, InnerProperty)));
				int32 ArraySize = Env->GetArrayLength(JavaArray);

				Helper.Resize(ArraySize);

				for (int32 i = 0; i < ArraySize; i++)
				{
					jstring javaString = static_cast<jstring>(Env->GetObjectArrayElement(JavaArray, i));
					StringProperty->SetPropertyValue(Helper.GetRawPtr(i), FAndroidJniObjectConverter::FromJavaString(javaString));
				}

				return;
			}

		}
		else if (FTextProperty* TextProperty = CastField<FTextProperty>(InnerProperty))
		{
			if (InState == OutJniObject)
			{
				jobjectArray javaStringArray = (jobjectArray)Env->NewObjectArray(Helper.Num(), FJavaWrapper::JavaStringClass, nullptr);

				for (int32 i = 0, n = Helper.Num(); i < n; ++i)
				{
					Env->SetObjectArrayElement(javaStringArray, i, FAndroidJniObjectConverter::GetJavaString(TextProperty->GetPropertyValue(Helper.GetRawPtr(i)).ToString()));
				}

				Env->SetObjectField(JniObject,GetNameFieldID<TArray<FString>>(Env, AGJniClass, InnerProperty), javaStringArray);

				return;
			}
			else
			{
				jobjectArray JavaArray = static_cast<jobjectArray>(Env->GetObjectField(JniObject, GetNameFieldID<TArray<FString>>(Env, AGJniClass, InnerProperty)));
				int32 ArraySize = Env->GetArrayLength(JavaArray);

				Helper.Resize(ArraySize);

				for (int32 i = 0; i < ArraySize; i++)
				{
					jstring javaString = static_cast<jstring>(Env->GetObjectArrayElement(JavaArray, i));
					FText JavaValue = FText::FromString(FAndroidJniObjectConverter::FromJavaString(javaString));
					TextProperty->SetPropertyValue(Helper.GetRawPtr(i), JavaValue);
				}

				return;
			}
		}
	}

	void SetPropertyToJniObject(JNIEnv* Env, jclass AGJniClass, jobject JniObject, FProperty* Property, void* InValue, int64 CheckFlags, int64 SkipFlags)
	{
		if (!JniObject) return;

		if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property)) // 是否是枚举
		{
			int64 Value = EnumProperty->GetUnderlyingProperty()->GetSignedIntPropertyValue(InValue);
			Env->SetByteField(JniObject, GetNameFieldID<uint8>(Env, AGJniClass, Property), (uint8)Value);
			return;
		}
		else if (FNumericProperty *NumericProperty = CastField<FNumericProperty>(Property)) // 是否是数字
		{
			// see if it's an enum
			UEnum* EnumDef = NumericProperty->GetIntPropertyEnum();
			if (EnumDef != NULL)
			{
				// 将枚举导出为字符串
				int64 Value = NumericProperty->GetSignedIntPropertyValue(InValue);
				Env->SetIntField(JniObject, GetNameFieldID<int32>(Env, AGJniClass, Property), (int32)Value);
				return;
			}
			// 我们想把数字作为数字输出
			if (NumericProperty->IsFloatingPoint())
			{
				Env->SetFloatField(JniObject, GetNameFieldID<float>(Env, AGJniClass, Property), (float)NumericProperty->GetFloatingPointPropertyValue(InValue));
				return;
			}
			else if (NumericProperty->IsInteger())
			{
				int64 Value = NumericProperty->GetSignedIntPropertyValue(InValue);
				Env->SetIntField(JniObject, GetNameFieldID<int32>(Env, AGJniClass, Property), (int32)Value);
				return;
			}
		}
		else if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property)) // 是否是布尔型
		{
			Env->SetBooleanField(JniObject, GetNameFieldID<bool>(Env, AGJniClass, Property), BoolProperty->GetPropertyValue(InValue));
			return;
		}
		else if (FStrProperty *StringProperty = CastField<FStrProperty>(Property))
		{
			Env->SetObjectField(JniObject, GetNameFieldID<FString>(Env, AGJniClass, Property), FAndroidJniObjectConverter::GetJavaString(StringProperty->GetPropertyValue(InValue)));
			return;
		}
		else if (FTextProperty* TextProperty = CastField<FTextProperty>(Property))
		{
			Env->SetObjectField(JniObject, GetNameFieldID<FString>(Env, AGJniClass, Property), FAndroidJniObjectConverter::GetJavaString(TextProperty->GetPropertyValue(InValue).ToString()));
			return;
		}
		else if (FArrayProperty *ArrayProperty = CastField<FArrayProperty>(Property))
		{
			PropertyArrToJniObject<EValueState::OutJniObject>(Env, AGJniClass, JniObject, ArrayProperty, ArrayProperty->Inner, InValue);
		}
		else if (FStructProperty *StructProperty = CastField<FStructProperty>(Property))
		{
			const FName NAME_DateTime(TEXT("DateTime"));
			UScriptStruct::ICppStructOps* TheCppStructOps = StructProperty->Struct->GetCppStructOps();
			if (StructProperty->Struct->GetFName() == NAME_DateTime && TheCppStructOps && TheCppStructOps->HasExportTextItem()) //如果是时间结构体
			{
				Env->SetLongField(JniObject, GetNameFieldID<int64>(Env, AGJniClass, Property), FAndroidJniObjectConverter::FDataTimeToJinLong(*((FDateTime const*)InValue)));
			}
		}
	}

}

jstring FAndroidJniObjectConverter::GetJavaString(const FString& InString)
{
	JNIEnv* JEnv = AndroidJavaEnv::GetJavaEnv();
	jstring local = JEnv->NewStringUTF(TCHAR_TO_UTF8(*InString));
	jstring result = (jstring)JEnv->NewGlobalRef(local);
	JEnv->DeleteLocalRef(local);
	return result;
}

FString FAndroidJniObjectConverter::FromJavaString(jstring JavaString)
{
	if (JavaString == nullptr)
	{
		return FString();
	}

	JNIEnv* Env = AndroidJavaEnv::GetJavaEnv();
	const char* UTFString = Env->GetStringUTFChars(JavaString, nullptr);
	FString Result(UTF8_TO_TCHAR(UTFString));
	Env->ReleaseStringUTFChars(JavaString, UTFString);
	Env->DeleteLocalRef(JavaString);

	return Result;
}

FString FAndroidJniObjectConverter::StandardizeCase(const FString& StringIn)
{
	/*// 这可能不适用于所有的情况，考虑完全降低字符串。
	FString FixedString = StringIn;
	FixedString[0] = FChar::ToLower(FixedString[0]); // 我们的json类/变量开始小写
	FixedString.ReplaceInline(TEXT("ID"), TEXT("Id"), ESearchCase::CaseSensitive); // Id是标准的而不是Id，我们的一些FName使用Id
	return FixedString;*/

	return StringIn;
}

int64 FAndroidJniObjectConverter::FDataTimeToJinLong(const FDateTime& InDataTime)
{
	FTimespan UtcDelta = FDateTime::Now() - FDateTime::UtcNow();
	FDateTime UtcStartDateTime = InDataTime - UtcDelta;

	return UtcStartDateTime.ToUnixTimestamp() * 1000;
}

FDateTime FAndroidJniObjectConverter::FDataTimeFormJinLong(const int64& InDataTime)
{
	FTimespan UtcDelta = FDateTime::Now() - FDateTime::UtcNow();
	return FDateTime::FromUnixTimestamp(InDataTime / 1000) + UtcDelta;
}

TSharedPtr<FJavaClassObject> FAndroidJniObjectConverter::UStructToJniObject(const UStruct* StructDefinition, const void* Struct, int64 CheckFlags /*= 0*/, int64 SkipFlags /*= 0*/)
{
	if (SkipFlags == 0)
	{
		// 如果没有指定的跳过标志，则在编写时默认情况下跳过已弃用、暂时和跳过序列化
		SkipFlags |= CPF_Deprecated | CPF_Transient;
	}

	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	if (!Env) return nullptr;
	
	FString JavaClassPath = FString::Printf(TEXT("com/Monocular/AndroidAttachThing/%s"), *StructDefinition->GetFName().ToString());

	jclass AGJniClass = FAndroidApplication::FindJavaClass(TCHAR_TO_UTF8(*JavaClassPath)); // 请把java结构体的package com.Monocular.AndroidAttachThing;
	TSharedPtr<FJavaClassObject> JavaClassObject = MakeShared<FJavaClassObject>(*JavaClassPath, "()V");

	jobject JavaObject = JavaClassObject->GetJObject();

	for (TFieldIterator<FProperty> It(StructDefinition); It; ++It)
	{
		FProperty* Property = *It;

		// 检查是否应忽略此属性
		if (CheckFlags != 0 && !Property->HasAnyPropertyFlags(CheckFlags))
		{
			continue;
		}
		if (Property->HasAnyPropertyFlags(SkipFlags))
		{
			continue;
		}

		const void* Value = Property->ContainerPtrToValuePtr<uint8>(Struct);

		UPropertyToJniValue(Env, AGJniClass, JavaObject, Property, const_cast<void*>(Value), CheckFlags, SkipFlags);
	}

	Env->DeleteLocalRef(AGJniClass);

	return JavaClassObject;
}

void FAndroidJniObjectConverter::UPropertyToJniValue(JNIEnv* Env, jclass AGJniClass, jobject JniObject, FProperty* Property, void* Value, int64 CheckFlags, int64 SkipFlags)
{
	SetPropertyToJniObject(Env, AGJniClass, JniObject, Property, Value, CheckFlags, SkipFlags);
}

bool FAndroidJniObjectConverter::UStructFormJavaObject(const UStruct* StructDefinition, void* OutStruct, JNIEnv* Env, jobject JavaContact, int64 CheckFlags /* = 0 */, int64 SkipFlags /* = 0 */)
{
	FString JavaClassPath = FString::Printf(TEXT("com/Monocular/AndroidAttachThing/%s"), *StructDefinition->GetFName().ToString());

	jclass AGClass = FAndroidApplication::FindJavaClass(TCHAR_TO_UTF8(*JavaClassPath));

	bool OutValue = k2_StructTypeFormJavaObject(StructDefinition, OutStruct, Env, AGClass, JavaContact, CheckFlags, SkipFlags);

	Env->DeleteLocalRef(AGClass);

	return OutValue;
}

bool FAndroidJniObjectConverter::k2_StructTypeFormJavaObject(const UStruct* StructDefinition, void* OutStruct, JNIEnv* Env, jclass AGClass, jobject JavaContact, int64 CheckFlags /* = 0 */, int64 SkipFlags /* = 0 */)
{
	for (TFieldIterator<FProperty> PropIt(StructDefinition); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;

		if (CheckFlags != 0 && !Property->HasAnyPropertyFlags(CheckFlags))
		{
			continue;
		}
		if (Property->HasAnyPropertyFlags(SkipFlags))
		{
			continue;
		}
		void* Value = Property->ContainerPtrToValuePtr<uint8>(OutStruct);
		UPropertyFormJniValue(Property, Value, Env, AGClass, JavaContact);

	}

	Env->DeleteLocalRef(JavaContact);

	return true;
}

bool FAndroidJniObjectConverter::UPropertyFormJniValue(FProperty* Property, void* OutStruct, JNIEnv* Env, jclass AGJniClass, jobject JavaContact)
{
	if (FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property)) // 是否是枚举
	{
		jbyte JavaValue = static_cast<jbyte>(Env->GetByteField(JavaContact, GetNameFieldID<uint8>(Env, AGJniClass, Property)));

		EnumProperty->GetUnderlyingProperty()->SetIntPropertyValue(OutStruct, (int64)JavaValue);

		return true;
	}
	else if (FNumericProperty* NumericProperty = CastField<FNumericProperty>(Property)) // 是否是数字
	{
		// see if it's an enum
		UEnum* EnumDef = NumericProperty->GetIntPropertyEnum();
		if (EnumDef != NULL)
		{
			jint JavaValue = static_cast<jint>(Env->GetIntField(JavaContact, GetNameFieldID<int32>(Env, AGJniClass, Property)));

			NumericProperty->SetIntPropertyValue(OutStruct, (int64)JavaValue);

			return true;
		}
		// 我们想把数字作为数字输出
		if (NumericProperty->IsFloatingPoint())
		{
			jfloat JavaValue = static_cast<jfloat>(Env->GetFloatField(JavaContact, GetNameFieldID<float>(Env, AGJniClass, Property)));
			NumericProperty->SetFloatingPointPropertyValue(OutStruct, (float)JavaValue);

			return true;
		}
		else if (NumericProperty->IsInteger())
		{
			jint JavaValue = static_cast<jint>(Env->GetIntField(JavaContact, GetNameFieldID<int32>(Env, AGJniClass, Property)));

			NumericProperty->SetIntPropertyValue(OutStruct, (int64)JavaValue);
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property)) // 是否是布尔型
	{
		jboolean JavaValue = static_cast<jboolean>(Env->GetIntField(JavaContact, GetNameFieldID<bool>(Env, AGJniClass, Property)));
		BoolProperty->SetPropertyValue(OutStruct, (bool)JavaValue);
		return true;
	}
	else if (FStrProperty* StringProperty = CastField<FStrProperty>(Property))
	{
		jstring javaString = static_cast<jstring>(Env->GetObjectField(JavaContact, GetNameFieldID<FString>(Env, AGJniClass, Property)));
		FString JavaValue = FromJavaString(javaString);

		StringProperty->SetPropertyValue(OutStruct, JavaValue);
		return true;
	}
	else if (FTextProperty* TextProperty = CastField<FTextProperty>(Property))
	{
		jstring javaString = static_cast<jstring>(Env->GetObjectField(JavaContact, GetNameFieldID<FString>(Env, AGJniClass, Property)));
		FText JavaValue = FText::FromString(FromJavaString(javaString));

		TextProperty->SetPropertyValue(OutStruct, JavaValue);
		return true;
	}
	else if (FArrayProperty *ArrayProperty = CastField<FArrayProperty>(Property))
	{
		PropertyArrToJniObject<EValueState::InJniObject>(Env, AGJniClass, JavaContact, ArrayProperty, ArrayProperty->Inner, OutStruct);
	}
	else if (FStructProperty *StructProperty = CastField<FStructProperty>(Property))
	{
		const FName NAME_DateTime(TEXT("DateTime"));
		if (StructProperty->Struct->GetFName() == NAME_DateTime)
		{
			jlong JavaValue = static_cast<jlong>(Env->GetLongField(JavaContact, GetNameFieldID<int64>(Env, AGJniClass, Property)));
			*(FDateTime*)OutStruct = FDataTimeFormJinLong((int64)JavaValue);
			return true;
		}
		else if (StructProperty->Struct->GetCppStructOps() && StructProperty->Struct->GetCppStructOps()->HasImportTextItem())
		{
			UScriptStruct::ICppStructOps* TheCppStructOps = StructProperty->Struct->GetCppStructOps();

		}
		return false;
	}

	return false;
}

#endif
