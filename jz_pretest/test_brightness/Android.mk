LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := test_brightness.sh
LOCAL_MODULE := test_brightness.sh
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH:= $(TARGET_OUT)/pretest
include $(BUILD_PREBUILT)
