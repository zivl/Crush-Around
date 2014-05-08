LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

# save the local path
MY_LOCAL_PATH := $(LOCAL_PATH)

include $(OPEN_CV_ANDROID)\sdk\native\jni\OpenCV.mk
include $(OPENCV_MK_PATH)


#include the make file for box2d
#include $(LOCAL_PATH)/../../Box2D/Android.mk

#restore the local path
LOCAL_PATH := $(MY_LOCAL_PATH)

LOCAL_C_INCLUDES += ${NDKROOT}/sources/cxx-stl/gnu-libstdc++/4.6/include \
					$(LOCAL_PATH)/../../Box2D \
                    $(LOCAL_PATH)/../../
                    
LOCAL_MODULE    := mixed_sample
LOCAL_SRC_FILES := jni_part.cpp \
				   ../../Core/clipper.cpp \
				   ../../Core/LcObjectDetector.cpp \
				   ../../Core/LcInPaint.cpp \
				   ../../Core/OpenCvDebugDraw.cpp \
				   ../../Core/VideoTracking.cpp			   
				   
LOCAL_LDLIBS +=  -llog -ldl -lbox2d

LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static

include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_STATIC_LIBRARY)

#$(call import-module, Box2D)
