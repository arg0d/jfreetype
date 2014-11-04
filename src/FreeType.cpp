#include "jnihelper.cpp"
#include "TextRenderer.cpp"

using namespace std;

extern "C" JNIEXPORT jobject JNICALL Java_com_cig_jfreetype_JFreeType_render(JNIEnv *env, jobject obj, jstring strFont, jstring strText, jfloat size, jint renderMode)
{
	std::string font = JNIHelper::GetString(env, strFont);
	std::string text = JNIHelper::GetString(env, strText);
	Bitmap *bitmap = TextRenderer::instance->Render(font, text, (float) size, renderMode);

	if (bitmap == NULL) {
		return NULL;
	}

	jclass bitmapClass = env->FindClass("com/cig/jfreetype/Bitmap");

	jmethodID bitmapConstructor = env->GetMethodID(bitmapClass, "<init>", "()V");
	jobject javaBitmap = env->NewObject(bitmapClass, bitmapConstructor);

	jfieldID fieldWidth = env->GetFieldID(bitmapClass, "width", "I");
	jfieldID fieldHeight = env->GetFieldID(bitmapClass, "height", "I");
	jfieldID fieldPixels = env->GetFieldID(bitmapClass, "pixels", "Ljava/nio/ByteBuffer;");

	int width = bitmap->width;
	int height = bitmap->height;

	env->SetIntField(javaBitmap, fieldWidth, (jint) width);
	env->SetIntField(javaBitmap, fieldHeight, (jint) height);	

	int length = width * height; 
	char * temp = new char[length];
	memcpy(temp, bitmap->pixels, length);
	jobject buffer = env->NewDirectByteBuffer(temp, length);
	env->SetObjectField(javaBitmap, fieldPixels, buffer);

	env->DeleteLocalRef(bitmapClass);
	delete bitmap;

	return javaBitmap;
}