ifeq ($(BOARD_HAVE_BLUETOOTH_BCM),true)

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := test_bluetooth.c

LOCAL_MODULE := test_bluetooth

LOCAL_STATIC_LIBRARIES := libc libcutils liblog
LOCAL_MODULE_PATH:= $(TARGET_OUT)/pretest
LOCAL_FORCE_STATIC_EXECUTABLE := true

LOCAL_C_FLAGS := \
	-DANDROID

include $(BUILD_EXECUTABLE)


include $(CLEAR_VARS)
LOCAL_SRC_FILES := test_bluetooth.sh
LOCAL_MODULE := test_bluetooth.sh
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH:= $(TARGET_OUT)/pretest
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := BCM4330.hcd
LOCAL_MODULE := BCM4330.hcd
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH:= $(TARGET_OUT)/pretest
include $(BUILD_PREBUILT)

endif
