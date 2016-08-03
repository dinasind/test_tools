LOCAL_PATH:= $(call my-dir)
$(shell mkdir -p $(TARGET_OUT)/pretest)
$(shell cp $(LOCAL_PATH)/*.png $(TARGET_OUT)/pretest)
include $(call all-makefiles-under,$(LOCAL_PATH))
