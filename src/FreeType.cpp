#include "jnihelper.cpp"
#include "TextRenderer.cpp"

using namespace std;

std::vector<std::string> logs;
void Log(const char* format, ...)
{
	char buffer[1024];
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);
	
	logs.push_back(std::string(buffer));

	if (logs.size() > 128)
	{
		logs.erase(logs.begin());
	}
}

extern "C"
{

JNIEXPORT jboolean JNICALL Java_com_cig_jfreetype_JFreeType_createImpl(JNIEnv *env, jobject obj)
{
	TextRenderer::instance = new TextRenderer();

	if (!TextRenderer::instance->Initialize())
	{
		delete TextRenderer::instance;
		TextRenderer::instance = NULL;
		return false;
	}

	return true;
}

JNIEXPORT jboolean JNICALL Java_com_cig_jfreetype_JFreeType_destroyImpl(JNIEnv *env, jobject obj)
{
	if (!TextRenderer::instance->Cleanup())
	{	
		return false;
	}

	delete TextRenderer::instance;
	TextRenderer::instance = NULL;
	return true;
}

JNIEXPORT jobject JNICALL Java_com_cig_jfreetype_JFreeType_render(JNIEnv *env, jobject obj, jstring strFont, jstring strText, jint size)
{
	std::string font = JNIHelper::GetString(env, strFont);
	std::string text = JNIHelper::GetString(env, strText);
	
	Bitmap bitmap;
	std::string error = TextRenderer::instance->Render(bitmap, font, text, (int) size);

	if (!error.empty())
	{
		Log("%s", error.c_str());
		return NULL;
	}

	jclass bitmapClass = env->FindClass("com/cig/jfreetype/Bitmap");

	jmethodID bitmapConstructor = env->GetMethodID(bitmapClass, "<init>", "()V");
	jobject javaBitmap = env->NewObject(bitmapClass, bitmapConstructor);

	jfieldID fieldWidth = env->GetFieldID(bitmapClass, "width", "I");
	jfieldID fieldHeight = env->GetFieldID(bitmapClass, "height", "I");
	jfieldID fieldPixels = env->GetFieldID(bitmapClass, "pixels", "Ljava/nio/ByteBuffer;");

	int width = bitmap.GetWidth();
	int height = bitmap.GetHeight();

	env->SetIntField(javaBitmap, fieldWidth, (jint) width);
	env->SetIntField(javaBitmap, fieldHeight, (jint) height);	

	int length = width * height * 4; 
	char * temp = new char[length];
	memcpy(temp, bitmap.GetPixels(), length);
	jobject buffer = env->NewDirectByteBuffer(temp, length);
	env->SetObjectField(javaBitmap, fieldPixels, buffer);

	env->DeleteLocalRef(bitmapClass);

	return javaBitmap;
}

JNIEXPORT jobject JNICALL Java_com_cig_jfreetype_JFreeType_meassure(JNIEnv *env, jobject obj, jstring strFont, jstring strText)
{
	std::string font = JNIHelper::GetString(env, strFont);
	std::string text = JNIHelper::GetString(env, strText);
	
	Vector2 size = TextRenderer::instance->Measure(font, text);

	jclass vector2Class = env->FindClass("com/cig/jfreetype/Vector2");

	jmethodID vector2Constructor = env->GetMethodID(vector2Class, "<init>", "(II)V");
	jobject javaVector2 = env->NewObject(vector2Class, vector2Constructor);

	jfieldID fieldX = env->GetFieldID(vector2Class, "x", "I");
	jfieldID fieldY = env->GetFieldID(vector2Class, "y", "I");

	int x = size.x;
	int y = size.y;

	env->SetIntField(javaVector2, fieldX, (jint) x);
	env->SetIntField(javaVector2, fieldY, (jint) y);	

	env->DeleteLocalRef(vector2Class);

	return javaVector2;	
}

JNIEXPORT jobject JNICALL Java_com_cig_jfreetype_JFreeType_renderWrapped(JNIEnv *env, jobject obj, jstring strFont, jstring strText, jint size, jint boundsWidth, jint boundsHeight)
{
	std::string font = JNIHelper::GetString(env, strFont);
	std::string text = JNIHelper::GetString(env, strText);
	Bitmap *bitmap = TextRenderer::instance->RenderWrapped(font, text, (float) size, Vector2((int) boundsWidth, (int) boundsHeight), TR_ALIGNMENT_CENTER);

	if (bitmap == NULL) {
		return NULL;
	}

	jclass bitmapClass = env->FindClass("com/cig/jfreetype/Bitmap");

	jmethodID bitmapConstructor = env->GetMethodID(bitmapClass, "<init>", "()V");
	jobject javaBitmap = env->NewObject(bitmapClass, bitmapConstructor);

	jfieldID fieldWidth = env->GetFieldID(bitmapClass, "width", "I");
	jfieldID fieldHeight = env->GetFieldID(bitmapClass, "height", "I");
	jfieldID fieldPixels = env->GetFieldID(bitmapClass, "pixels", "Ljava/nio/ByteBuffer;");

	int width = bitmap->GetWidth();
	int height = bitmap->GetHeight();

	env->SetIntField(javaBitmap, fieldWidth, (jint) width);
	env->SetIntField(javaBitmap, fieldHeight, (jint) height);	

	int length = width * height * 4; 
	char * temp = new char[length];
	memcpy(temp, bitmap->GetPixels(), length);
	jobject buffer = env->NewDirectByteBuffer(temp, length);
	env->SetObjectField(javaBitmap, fieldPixels, buffer);

	env->DeleteLocalRef(bitmapClass);
	delete bitmap;

	return javaBitmap;
}

JNIEXPORT jstring JNICALL Java_com_cig_jfreetype_JFreeType_getLogImpl(JNIEnv *env, jobject obj)
{
	if (logs.size() > 0)
	{
		std::string strLog = logs.at(0);
		jstring jLog = env->NewStringUTF(strLog.c_str());
		logs.erase(logs.begin());
		return jLog;
	} else return NULL;
}

}