# 由 Tomato sauce 移植
# 由 Tomato sauce 移植
# 由 Tomato sauce 移植	

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := NewImGuiUI
LOCAL_CPPFLAGS := -w -std=c++17

LOCAL_C_INCLUDES +=$(LOCAL_PATH)/include
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/include/Android_draw
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/include/Android_read
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/include/Android_shm
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/include/Android_touch
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/include/ImGui
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/include/native_surface
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/include/native_surface/aosp_10
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/include/native_surface/aosp_11
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/include/native_surface/aosp_12
LOCAL_C_INCLUDES += $(LOCAL_C_INCLUDES:$(LOCAL_PATH)/%:=%)

FILE_LIST += $(wildcard $(LOCAL_PATH)/src/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/Android_draw/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/Android_read/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/Android_shm/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/Android_touch/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/ImGui/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/src/native_surface/*.c*)
LOCAL_SRC_FILES += $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_LDFLAGS += -lEGL -lGLESv2 -lGLESv3 -landroid -llog
include $(BUILD_EXECUTABLE)
