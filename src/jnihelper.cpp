#include "jni.h"
#include <iostream>

namespace JNIHelper
{

	jobject CreateJObject(JNIEnv *env, jclass objectClass)
	{
		jmethodID objectConstructor = env->GetMethodID(objectClass, "<init>", "()V");
		jobject object = env->NewObject(objectClass, objectConstructor);
		return object;
	}

	jobject CreateJObject(JNIEnv* env, const std::string &className)
	{
		jclass objectClass = env->FindClass(className.c_str());
		jobject object = CreateJObject(env, objectClass);
		env->DeleteLocalRef(objectClass);
		return object;
	}

	std::string GetString(JNIEnv *env, jstring str)
	{
		const char *chars = env->GetStringUTFChars(str, NULL);
		std::string string(chars);
		env->ReleaseStringUTFChars(str, chars);
		return string;
	}

}