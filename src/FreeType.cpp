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
	jfieldID fieldPixels = env->GetFieldID(bitmapClass, "pixels", "Ljava/nio/ByteBuffer;");

	env->SetIntField(javaBitmap, fieldWidth, (jint) bitmap->width);
	env->SetIntField(javaBitmap, fieldHeight, (jint) bitmap->height);	


	jobject buffer = env->NewDirectByteBuffer((void*) bitmap->pixels, bitmap->width * bitmap->height);
	env->SetObjectField(javaBitmap, fieldPixels, buffer);

	env->DeleteLocalRef(bitmapClass);

	return javaBitmap;
}