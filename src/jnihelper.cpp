#include "jni.h"
#include <iostream>

namespace JNIHelper
{

	std::string GetString(JNIEnv *env, jstring str)
	{
		const char * chars = env->GetStringUTFChars(str, NULL);

		std::string string(chars);

		env->ReleaseStringUTFChars(str, chars);

		return string;
	}

}