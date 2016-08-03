LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := test_current.sh
LOCAL_MODULE := test_current.sh
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH:= $(TARGET_OUT)/pretest
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES += bootable/recovery/minui/
LOCAL_SRC_FILES := test_current.c
LOCAL_MODULE := test_current
LOCAL_STATIC_LIBRARIES := \
			libpretest \
			libminui \
			libpng \
			libselinux \
			libm \
			libc
LOCAL_MODULE_PATH:= $(TARGET_OUT)/pretest
LOCAL_FORCE_STATIC_EXECUTABLE := true
ifeq ($(TARGET_USERIMAGES_USE_EXT4), true)
    LOCAL_CFLAGS += -DUSE_EXT4
    LOCAL_C_INCLUDES += system/extras/ext4_utils
    LOCAL_STATIC_LIBRARIES += libext4_utils_static libz
endif
LOCAL_C_FLAGS :=
include $(BUILD_EXECUTABLE)
