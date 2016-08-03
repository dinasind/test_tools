# Copyright 2009 Google Inc. All Rights Reserved.
#
# Android.mk for etc1tool
#

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := test_drv2605.cpp \
    motor.cpp \
    tips.cpp

#LOCAL_C_INCLUDES += build/libs/host/include

LOCAL_SHARED_LIBRARIES := liblog
LOCAL_MODULE := test_drv2605
LOCAL_MODULE_PATH := $(TARGET_OUT)/pretest

include $(BUILD_EXECUTABLE)
