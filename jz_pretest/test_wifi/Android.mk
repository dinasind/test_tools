LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE:= libnetutilsm
LOCAL_SRC_FILES:= \
	libnetutils/dhcpclient.c \
	libnetutils/dhcpmsg.c \
	libnetutils/dhcp_utils.c \
	libnetutils/ifc_utils.c \
	libnetutils/packet.c
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := netcfg.c

LOCAL_MODULE := netcfg_s

LOCAL_STATIC_LIBRARIES := libc libnetutilsm libcutils liblog
LOCAL_MODULE_PATH:= $(TARGET_OUT)/pretest
LOCAL_FORCE_STATIC_EXECUTABLE := true


include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := test_wifi.sh
LOCAL_MODULE := test_wifi.sh
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH:= $(TARGET_OUT)/pretest
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := fw_bcm43438.bin
LOCAL_MODULE := fw_bcm43438.bin
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH:= $(TARGET_OUT)/pretest
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := bcm43438.cal
LOCAL_MODULE := bcm43438.cal
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH:= $(TARGET_OUT)/pretest
include $(BUILD_PREBUILT)
