#include "jnihelper.cpp"
#include "TextRenderer.cpp"

using namespace std;

std::vector<std::string> logs;
bool instantLoggingEnabled = false;

void Log(const char* format, ...)
{
	char buffer[1024];
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);
	
	if (instantLoggingEnabled)
	{
		std::cout << "<JFreeType>: " << buffer << std::endl;
	}
	else
	{
		logs.push_back(std::string(buffer));
		if (logs.size() > 128)
		{
			logs.erase(logs.begin());
		}
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

jobject CreateTextMetricsJavaObject(JNIEnv *env, int width, int height, int descender)
{
	jclass textMetricsClass = env->FindClass("com/cig/jfreetype/TextMetrics");
	jobject javaTextMetrics = JNIHelper::CreateJObject(env, textMetricsClass);

	jfieldID fieldWidth = env->GetFieldID(textMetricsClass, "width", "I");
	jfieldID fieldHeight = env->GetFieldID(textMetricsClass, "height", "I");
	jfieldID fieldDescender = env->GetFieldID(textMetricsClass, "descender", "I");

	env->SetIntField(javaTextMetrics, fieldWidth, (jint) width);
	env->SetIntField(javaTextMetrics, fieldHeight, (jint) height);
	env->SetIntField(javaTextMetrics, fieldDescender, (jint) descender);

	env->DeleteLocalRef(textMetricsClass);
	return javaTextMetrics;
}

jobject CreateBitmapJavaObject(JNIEnv *env, Bitmap &bitmap, int descender)
{
	jclass bitmapClass = env->FindClass("com/cig/jfreetype/Bitmap");

	jobject javaBitmap = JNIHelper::CreateJObject(env, bitmapClass);
	jobject javaTextMetrics = CreateTextMetricsJavaObject(env, bitmap.GetWidth(), bitmap.GetHeight(), descender);

	jfieldID fieldWidth = env->GetFieldID(bitmapClass, "width", "I");
	jfieldID fieldHeight = env->GetFieldID(bitmapClass, "height", "I");
	jfieldID fieldPixels = env->GetFieldID(bitmapClass, "pixels", "Ljava/nio/ByteBuffer;");
	jfieldID fieldTextMetrics = env->GetFieldID(bitmapClass, "textMetrics", "Lcom/cig/jfreetype/TextMetrics;");

	int width = bitmap.GetWidth();
	int height = bitmap.GetHeight();

	env->SetIntField(javaBitmap, fieldWidth, (jint) width);
	env->SetIntField(javaBitmap, fieldHeight, (jint) height);
	env->SetObjectField(javaBitmap, fieldTextMetrics, javaTextMetrics);

	// Copy texture data and set the field
	int length = width * height * 4; 
	char * temp = new char[length];
	memcpy(temp, bitmap.GetPixels(), length);
	jobject buffer = env->NewDirectByteBuffer(temp, length);
	env->SetObjectField(javaBitmap, fieldPixels, buffer);

	env->DeleteLocalRef(bitmapClass);
	return javaBitmap;
}

JNIEXPORT jobject JNICALL Java_com_cig_jfreetype_JFreeType_render(JNIEnv *env, jobject obj, jstring strFont, jstring strText, jint size, jint width, jint height)
{
	std::string font = JNIHelper::GetString(env, strFont);
	std::string text = JNIHelper::GetString(env, strText);
	Vector2 bounds(width, height);

	Bitmap bitmap;
	int descender = 0;
	std::string error = TextRenderer::instance->Render(bitmap, font, text, (int) size, bounds, &descender);

	if (!error.empty())
	{
		Log("%s", error.c_str());
		return NULL;
	}

	jobject javaBitmap = CreateBitmapJavaObject(env, bitmap, descender);

	return javaBitmap;
}

JNIEXPORT jobject JNICALL Java_com_cig_jfreetype_JFreeType_measure(JNIEnv *env, jobject obj, jstring strFont, jstring strText)
{
	std::string font = JNIHelper::GetString(env, strFont);
	std::string text = JNIHelper::GetString(env, strText);
	
	Vector2 size = TextRenderer::instance->Measure(font, text);

	jobject javaTextMetrics = CreateTextMetricsJavaObject(env, size.x, size.y, 0);

	return javaTextMetrics;	
}

JNIEXPORT jobject JNICALL Java_com_cig_jfreetype_JFreeType_renderWrapped(JNIEnv *env, jobject obj, jstring strFont, jstring strText, jint size, jint boundsWidth, jint boundsHeight, jint lineSpacing)
{

	std::string font = JNIHelper::GetString(env, strFont);
	std::string text = JNIHelper::GetString(env, strText);
	Bitmap *bitmap = TextRenderer::instance->RenderWrapped(font, text, (float) size, Vector2((int) boundsWidth, (int) boundsHeight), lineSpacing, TR_ALIGNMENT_CENTER);

	if (bitmap == NULL) {
		return NULL;
	}

	jobject javaBitmap = CreateBitmapJavaObject(env, *bitmap, 0);

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

JNIEXPORT void JNICALL Java_com_cig_jfreetype_JFreeType_setInstantLoggingEnabledImpl(JNIEnv *env, jboolean jInstantLoggingEnabled)
{
	instantLoggingEnabled = (bool) jInstantLoggingEnabled;
}

}