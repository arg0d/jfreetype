#include "jni.h"
#include "TextRenderer.cpp"

using namespace std;

extern "C" JNIEXPORT jobject JNICALL Java_com_cig_jfreetype_FreeType_render(JNIEnv *env, jobject obj, jstring str, jfloat scale)
{

	Bitmap *bitmap = TextRenderer::instance->Render("Coming down town!", 0.25f);
	delete TextRenderer::instance;

	jclass bitmapClass = env->FindClass("com/cig/jfreetype/Bitmap");

	jmethodID bitmapConstructor = env->GetMethodID(bitmapClass, "<init>", "()V");
	jobject javaBitmap = env->NewObject(bitmapClass, bitmapConstructor);

	jfieldID fieldWidth = env->GetFieldID(bitmapClass, "width", "I");
	jfieldID fieldHeight = env->GetFieldID(bitmapClass, "height", "I");
	jfieldID fieldPixels = env->GetFieldID(bitmapClass, "pixels", "[B");

	env->SetIntField(javaBitmap, fieldWidth, (jint) bitmap->width);
	env->SetIntField(javaBitmap, fieldHeight, (jint) bitmap->height);

	int size = bitmap->width * bitmap->height;
	jbyte temp[size];
	for (int i = 0; i < size; i++) {
		temp[i] = (jbyte) *(bitmap->pixels + i);
	}
	jbyteArray buffer = env->NewByteArray(size);
	env->SetByteArrayRegion(buffer, 0, size, temp);
	env->SetObjectField(javaBitmap, fieldPixels, buffer);

	env->DeleteLocalRef(bitmapClass);

	return javaBitmap;
}