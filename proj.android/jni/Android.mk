LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos/audio/include)
$(call import-add-path, $(LOCAL_PATH))

LOCAL_MODULE := MyGame_shared

LOCAL_MODULE_FILENAME := libMyGame

LOCAL_SRC_FILES := hellocpp/main.cpp \
\ \
../../proj.win10/App/pch.cpp \
\ \
../../Classes/AppDelegate.cpp \
\ \
../../../NRG_Engine/Classes/nrgFunctions.cpp \
\ \
../../../NRG_Engine/Classes/nrgSprite.cpp \
\ \
../../../NRG_Engine/Classes/nrgScene.cpp \
\ \
../../../NRG_Engine/Classes/nrgBox.cpp \
\ \
../../../NRG_Engine/Classes/nrgTimer.cpp \
\ \
../../../NRG_Engine/Classes/nrgText.cpp \
\ \
../../../NRG_Engine/Classes/nrgList.cpp \
\ \
../../../NRG_Engine/Classes/nrgMenu.cpp \
\ \
../../../NRG_Engine/Classes/nrgButton.cpp \
\ \
../../../NRG_Engine/Classes/nrgAudio.cpp \
\ \
../../Classes/OSLESMediaPlayer.cpp \
\ \
../../Classes/AndroidAudioClips.cpp \
\ \
../../Classes/GameData.cpp \
\ \
../../Classes/Start.cpp \
\ \
../../Classes/Game.cpp \
\ \
../../Classes/MainMenu.cpp \
\ \
../../Classes/Countdown.cpp \
\ \
../../Classes/Classic.cpp \
\ \
../../Classes/RushRelax.cpp \
\ \
../../Classes/MemoryMode.cpp \
\ \
../../Classes/Tutorial_Matching.cpp \
\ \
../../Classes/Tutorial_Bonus.cpp \
\ \
../../Classes/BaseTile.cpp \
\ \
../../Classes/Tile.cpp \
\ \
../../Classes/Options.cpp \ ../../Classes/HighScores.cpp \ ../../Classes/Store.cpp

PCH_FILE := $(LOCAL_PATH)/../../proj.win10/App/pch.h

LOCAL_CPPFLAGS := -include $(PCH_FILE) -DSDKBOX_ENABLED

LOCAL_LDLIBS := -landroid \
-llog

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes \
\ \
$(LOCAL_PATH)/../../Classes/Android_Audio_Headers \
$(LOCAL_PATH)/../../proj.win10/App \
$(LOCAL_PATH)/../../../NRG_Engine/Classes

LOCAL_WHOLE_STATIC_LIBRARIES := PluginIAP \
sdkbox \
android_native_app_glue \
PluginAdMob \
PluginChartboost \
chartboost \
PluginSdkboxPlay

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END


LOCAL_STATIC_LIBRARIES := cocos2dx_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)
$(call import-module, ./sdkbox)
$(call import-module, ./pluginiap)
$(call import-module, ./pluginadmob)
$(call import-module, ./pluginchartboost)
$(call import-module, ./pluginsdkboxplay)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
