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
                    $(LOCAL_PATH)/../../
                    
LOCAL_MODULE    := crush_around

LOCAL_SRC_FILES := \
jni_part.cpp \
../../Core/CVUtils.cpp \
../../Core/clipper.cpp \
../../Core/LcObjectDetector.cpp \
../../Core/LcInPaint.cpp \
../../Core/OpenCvDebugDraw.cpp \
../../Core/MyContactListener.cpp \
../../Core/World.cpp \
../../Core/VideoTracking.cpp \
../../Box2d/Collision/Shapes/b2ChainShape.cpp \
../../Box2d/Collision/Shapes/b2CircleShape.cpp \
../../Box2d/Collision/Shapes/b2EdgeShape.cpp \
../../Box2d/Collision/Shapes/b2PolygonShape.cpp \
../../Box2d/Collision/b2BroadPhase.cpp \
../../Box2d/Collision/b2CollideCircle.cpp \
../../Box2d/Collision/b2CollideEdge.cpp \
../../Box2d/Collision/b2CollidePolygon.cpp \
../../Box2d/Collision/b2Collision.cpp \
../../Box2d/Collision/b2Distance.cpp \
../../Box2d/Collision/b2DynamicTree.cpp \
../../Box2d/Collision/b2TimeOfImpact.cpp \
../../Box2d/Common/b2BlockAllocator.cpp \
../../Box2d/Common/b2Draw.cpp \
../../Box2d/Common/b2Math.cpp \
../../Box2d/Common/b2Settings.cpp \
../../Box2d/Common/b2StackAllocator.cpp \
../../Box2d/Common/b2Timer.cpp \
../../Box2d/Dynamics/Contacts/b2ChainAndCircleContact.cpp \
../../Box2d/Dynamics/Contacts/b2ChainAndPolygonContact.cpp \
../../Box2d/Dynamics/Contacts/b2CircleContact.cpp \
../../Box2d/Dynamics/Contacts/b2Contact.cpp \
../../Box2d/Dynamics/Contacts/b2ContactSolver.cpp \
../../Box2d/Dynamics/Contacts/b2EdgeAndCircleContact.cpp \
../../Box2d/Dynamics/Contacts/b2EdgeAndPolygonContact.cpp \
../../Box2d/Dynamics/Contacts/b2PolygonAndCircleContact.cpp \
../../Box2d/Dynamics/Contacts/b2PolygonContact.cpp \
../../Box2d/Dynamics/Joints/b2DistanceJoint.cpp \
../../Box2d/Dynamics/Joints/b2FrictionJoint.cpp \
../../Box2d/Dynamics/Joints/b2GearJoint.cpp \
../../Box2d/Dynamics/Joints/b2Joint.cpp \
../../Box2d/Dynamics/Joints/b2MotorJoint.cpp \
../../Box2d/Dynamics/Joints/b2MouseJoint.cpp \
../../Box2d/Dynamics/Joints/b2PrismaticJoint.cpp \
../../Box2d/Dynamics/Joints/b2PulleyJoint.cpp \
../../Box2d/Dynamics/Joints/b2RevoluteJoint.cpp \
../../Box2d/Dynamics/Joints/b2RopeJoint.cpp \
../../Box2d/Dynamics/Joints/b2WeldJoint.cpp \
../../Box2d/Dynamics/Joints/b2WheelJoint.cpp \
../../Box2d/Dynamics/b2Body.cpp \
../../Box2d/Dynamics/b2ContactManager.cpp \
../../Box2d/Dynamics/b2Fixture.cpp \
../../Box2d/Dynamics/b2Island.cpp \
../../Box2d/Dynamics/b2World.cpp \
../../Box2d/Dynamics/b2WorldCallbacks.cpp \
../../Box2d/Rope/b2Rope.cpp		   
				   
LOCAL_LDLIBS +=  -llog -ldl# -lbox2d

#LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static

include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_STATIC_LIBRARY)

#$(call import-module, Box2D)
