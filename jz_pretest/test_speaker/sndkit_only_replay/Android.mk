LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := sndkit.c wav.c

LOCAL_MODULE := test_speaker

LOCAL_STATIC_LIBRARIES := libc libm libthread_db
#libcutils liblog
LOCAL_MODULE_PATH:= $(TARGET_OUT)/pretest
LOCAL_FORCE_STATIC_EXECUTABLE := true

LOCAL_C_FLAGS :=

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := sound.wav
LOCAL_MODULE := sound.wav
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH:= $(TARGET_OUT)/pretest
include $(BUILD_PREBUILT)
