LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

include D:\opencv\OpenCV-2.4.8-android-sdk\sdk\native\jni\OpenCV.mk
include $(OPENCV_MK_PATH)

LOCAL_C_INCLUDES += ${NDKROOT}/sources/cxx-stl/gnu-libstdc++/4.6/include \
                    $(LOCAL_PATH)/../../
                    
LOCAL_MODULE    := mixed_sample
LOCAL_SRC_FILES := jni_part.cpp \
				   ../../Core/clipper.cpp \
				   ../../Core/LcObjectDetector.cpp
LOCAL_LDLIBS +=  -llog -ldl

include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_STATIC_LIBRARY)
