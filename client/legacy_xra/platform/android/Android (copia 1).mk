# =============================================================
# ARX ENGINE - CLIENTE - Android.mk
# Librerias: raylib, assimp, imgui, Jolt, glm, zlib
# Todas desde third_Party (Android 64 bits)
# =============================================================
LOCAL_PATH := $(call my-dir)

THIRD_PARTY := $(LOCAL_PATH)/../../../third_Party
COMMON_DIR  := $(LOCAL_PATH)/../../../common
MOTOR_PATH  := $(LOCAL_PATH)/../../../Motor/platform/android
TP_LIB      := $(THIRD_PARTY)/Android_64_Bits

# Prebuilt static libraries
include $(CLEAR_VARS)
LOCAL_MODULE := tp_raylib
LOCAL_SRC_FILES := $(TP_LIB)/libraylib.a
LOCAL_EXPORT_C_INCLUDES := $(THIRD_PARTY)/Include/raylib
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := tp_imgui
LOCAL_SRC_FILES := $(TP_LIB)/libimgui_android_64.a
LOCAL_EXPORT_C_INCLUDES := $(THIRD_PARTY)/Include/imgui
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := tp_jolt
LOCAL_SRC_FILES := $(TP_LIB)/libJolt_android_64.a
LOCAL_EXPORT_C_INCLUDES := $(THIRD_PARTY)/Include/Jolt
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := tp_assimp
LOCAL_SRC_FILES := $(TP_LIB)/libassimp.a
LOCAL_EXPORT_C_INCLUDES := $(THIRD_PARTY)/Include/assimp
include $(PREBUILT_STATIC_LIBRARY)

# Modulo principal del Cliente
include $(CLEAR_VARS)
LOCAL_MODULE := ARX_Cliente
LOCAL_SRC_FILES := main.cpp $(COMMON_DIR)/ARX_PlatformImpl.cpp
LOCAL_CPPFLAGS := -std=c++17 -O2 -Wall -DPLATFORM_ANDROID
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH) \
    $(LOCAL_PATH)/scripting \
    $(MOTOR_PATH) \
    $(MOTOR_PATH)/core \
    $(MOTOR_PATH)/graphics \
    $(MOTOR_PATH)/resources \
    $(THIRD_PARTY)/Include/raylib \
    $(THIRD_PARTY)/Include/assimp \
    $(THIRD_PARTY)/Include/glm \
    $(THIRD_PARTY)/Include/imgui \
    $(THIRD_PARTY)/Include/Jolt \
    $(COMMON_DIR)

LOCAL_LDLIBS := -llog -landroid -lEGL -lGLESv2 -lm -lpthread -lz
LOCAL_STATIC_LIBRARIES := tp_raylib tp_imgui tp_jolt tp_assimp

include $(BUILD_SHARED_LIBRARY)
