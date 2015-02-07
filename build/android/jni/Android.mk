LOCAL_PATH := $(call my-dir)/../../..

include $(CLEAR_VARS)
LOCAL_MODULE    := jfreetype
LOCAL_C_INCLUDES := $(LOCAL_PATH)/libs/freetype/include
LOCAL_SRC_FILES := src/FreeType.cpp
LOCAL_STATIC_LIBRARIES := freetype
include $(BUILD_SHARED_LIBRARY)

include $(LOCAL_PATH)/libs/freetype/Android.mk