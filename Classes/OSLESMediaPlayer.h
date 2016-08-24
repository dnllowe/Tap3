#pragma once
#include "pch.h"

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "SLES/OpenSLES.h"
#include "SLES/OpenSLES_Android.h"
#include <jni.h> //For Java code interface
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "platform/android/CCFileUtils-android.h"
#include "C:/Users/Daniel/Desktop/Projects/Tap3/cocos2d/cocos/audio/android/ccdandroidUtils.h"


#endif

// other necessary headers
#include <assert.h>
#include <sys/types.h>

//All the Android Sound files
#include "AndroidAudioClips.h"

class OSLESMediaPlayer 
{
public:
	
	//Default construtor runs createEngine()
	OSLESMediaPlayer();

	//Default deconstructor runs releaseEngine()
	~OSLESMediaPlayer();

	// create the engine and output mix objects
	void createEngine();

	// create audio player
	bool createAudioPlayer();
	
	//Destroy objects and free memory
	void releaseEngine();

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	//Get OpenSLES play state for sound player
	SLuint32 getPlayState(SLPlayItf& bufferPlayer);

	//Set OpenSLES play state for sound player
	void setPlayState(SLuint32 state, SLPlayItf& bufferPlayer);

	void Stop(SLPlayItf& bufferPlayer);
	

#endif

    void Play(std::string clip);
	void StartBGM(std::string clip);
	void StopBGM();
	void MuteSound();
	void UnmuteSound();
	void ReleaseBGM();
	void StopAll();
    void Pause();
	void Resume();
 //   bool IsPlaying();
    void SeekTo(int position);
    int GetDuration();
    int GetPosition();
    void SetRate(int rate);
    int GetRate();
    bool SetLoop( int startPos, int endPos );
    bool SetNoLoop();
	bool CheckCompletion();
	
protected:
	bool muteSound = false;
	char strConverter[256]; //Converts integers to strings
};
