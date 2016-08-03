LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := test_touchscreen.sh
LOCAL_MODULE := test_touchscreen.sh
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH:= $(TARGET_OUT)/pretest
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := get_coordinate.c
LOCAL_MODULE := get_coordinate
LOCAL_STATIC_LIBRARIES := libc
LOCAL_MODULE_PATH:= $(TARGET_OUT)/pretest
LOCAL_FORCE_STATIC_EXECUTABLE := true
LOCAL_C_FLAGS :=
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_C_INCLUDES += bootable/recovery/minui/
LOCAL_SRC_FILES := test_touchbutton.c
LOCAL_MODULE := test_touchbutton
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
