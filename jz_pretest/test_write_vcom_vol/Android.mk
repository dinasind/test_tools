LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := vcom.c

LOCAL_MODULE := test_write_vcom

LOCAL_STATIC_LIBRARIES := libc libcutils liblog libcutils
LOCAL_MODULE_PATH:= $(TARGET_OUT)/pretest
LOCAL_FORCE_STATIC_EXECUTABLE := true

LOCAL_C_FLAGS :=

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := test_write_vcom_vol.sh
LOCAL_MODULE := test_write_vcom_vol.sh
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH:= $(TARGET_OUT)/pretest
include $(BUILD_PREBUILT)
