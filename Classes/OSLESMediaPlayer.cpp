#pragma once
#include "pch.h"
#include "OSLESMediaPlayer.h"

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, "OSLESMediaPlayer", __VA_ARGS__) 
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , "OSLESMediaPlayer", __VA_ARGS__) 
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO   , "OSLESMediaPlayer", __VA_ARGS__) 
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN   , "OSLESMediaPlayer", __VA_ARGS__) 
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , "OSLESMediaPlayer", __VA_ARGS__) 

// engine interfaces
static SLObjectItf engineObject = NULL;
static SLEngineItf engineEngine;

// player interfaces
static SLObjectItf bufferObject1, bufferObject2, bufferObject3, bufferObject4, bufferObject5, bufferObject6, bufferObject7, bufferObject8, bgmPlayerObject = NULL;
static SLPlayItf bufferPlayer1, bufferPlayer2, bufferPlayer3, bufferPlayer4, bufferPlayer5, bufferPlayer6, bufferPlayer7, bufferPlayer8, bgmPlayer;
static SLAndroidSimpleBufferQueueItf bufferQueue1, bufferQueue2, bufferQueue3, bufferQueue4, bufferQueue5, bufferQueue6, bufferQueue7, bufferQueue8;
static SLVolumeItf playerVolume;

// Interface objects
//static SLDynamicInterfaceManagementItf dynamicInterface;
static SLSeekItf playerSeek;
static SLPlaybackRateItf bgmPlaybackRate;

// output mix interfaces
static SLObjectItf outputMixObject = NULL;

// playback rate (default 1x:1000)
static SLpermille bgmPlaybackMinRate = 500;
static SLpermille bgmPlaybackMaxRate = 2000;
static SLpermille bgmPlaybackRateStepSize;

#endif

// pointer and size of the next player buffer to enqueue, and number of remaining buffers
static short *nextBuffer = NULL;
static unsigned int nextSize = 0;

//Whether the buffer has completed running through data
static bool bufferIsDone1 = true;
static bool bufferIsDone2 = true;
static bool bufferIsDone3 = true;
static bool bufferIsDone4 = true;
static bool bufferIsDone5 = true;
static bool bufferIsDone6 = true;
static bool bufferIsDone7 = true;
static bool bufferIsDone8 = true;
static bool bgmIsDone = true;

class nrgTimer;
nrgTimer timer;

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
 // this callback handler is called every time a buffer finishes playing
void bufferPlayerCallback1(SLAndroidSimpleBufferQueueItf bq, void *context)
{}
void bufferPlayerCallback2(SLAndroidSimpleBufferQueueItf bq, void *context)
{}
void bufferPlayerCallback3(SLAndroidSimpleBufferQueueItf bq, void *context)
{}
void bufferPlayerCallback4(SLAndroidSimpleBufferQueueItf bq, void *context)
{}
void bufferPlayerCallback5(SLAndroidSimpleBufferQueueItf bq, void *context)
{}
void bufferPlayerCallback6(SLAndroidSimpleBufferQueueItf bq, void *context)
{}
void bufferPlayerCallback7(SLAndroidSimpleBufferQueueItf bq, void *context)
{}
void bufferPlayerCallback8(SLAndroidSimpleBufferQueueItf bq, void *context)
{}

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
void OSLESMediaPlayer::Play(std::string clip)
{
	if (muteSound)
		return;

	//Check which audio clip to play
	if (clip == "tone1")
	{
		nextBuffer = (short *)tone1;
		nextSize = sizeof(tone1);
	}
	else if (clip == "tone2")
	{
		nextBuffer = (short *)tone2;
		nextSize = sizeof(tone2);
	}
	else if (clip == "warning_tone")
	{
		nextBuffer = (short *)warning_tone;
		nextSize = sizeof(warning_tone);
	}
	else if (clip == "triple_tone_high")
	{
		nextBuffer = (short *)triple_tone_high;
		nextSize = sizeof(triple_tone_high);
	}
	else if (clip == "double_tone_high")
	{
		nextBuffer = (short *)double_tone_high;
		nextSize = sizeof(double_tone_high);
	}
	else if (clip == "double_tone_low")
	{
		nextBuffer = (short *)double_tone_low;
		nextSize = sizeof(double_tone_low);
	}
	else if (clip == "double_tone_higher")
	{
		nextBuffer = (short *)double_tone_higher;
		nextSize = sizeof(double_tone_higher);
	}
	else if (clip == "low_tone")
	{
		nextBuffer = (short *)low_tone;
		nextSize = sizeof(low_tone);
	}
	SLresult result;

	//Select first free buffer
	if (bufferIsDone1)
	{
		result = (*bufferQueue1)->Enqueue(bufferQueue1, nextBuffer, nextSize);
		//if (SL_RESULT_SUCCESS == result)
		//LOGD("Audio Clip added to Buffer Queue 1");

		result = (*bufferPlayer1)->SetPlayState(bufferPlayer1, SL_PLAYSTATE_PLAYING);
		assert(SL_RESULT_SUCCESS == result);
		//if (SL_RESULT_SUCCESS == result)
		//LOGD("Playing Audio Clip");

		bufferIsDone1 = false;
		(void)result;
	}
	else if (bufferIsDone2)
	{
		result = (*bufferQueue2)->Enqueue(bufferQueue2, nextBuffer, nextSize);
		//if (SL_RESULT_SUCCESS == result)
		//LOGD("Audio Clip added to Buffer Queue 2");

		result = (*bufferPlayer2)->SetPlayState(bufferPlayer2, SL_PLAYSTATE_PLAYING);
		assert(SL_RESULT_SUCCESS == result);
		//if (SL_RESULT_SUCCESS == result)
		//LOGD("Playing Audio Clip");

		bufferIsDone2 = false;
		(void)result;
	}
	else if (bufferIsDone3)
	{
		result = (*bufferQueue3)->Enqueue(bufferQueue3, nextBuffer, nextSize);
		//if (SL_RESULT_SUCCESS == result)
		//LOGD("Audio Clip added to Buffer Queue 3");

		result = (*bufferPlayer3)->SetPlayState(bufferPlayer3, SL_PLAYSTATE_PLAYING);
		assert(SL_RESULT_SUCCESS == result);
		//if (SL_RESULT_SUCCESS == result)
		//LOGD("Playing Audio Clip");

		bufferIsDone3 = false;
		(void)result;
	}
	else if (bufferIsDone4)
	{
		result = (*bufferQueue4)->Enqueue(bufferQueue4, nextBuffer, nextSize);
		//if (SL_RESULT_SUCCESS == result)
		//LOGD("Audio Clip added to Buffer Queue 4");

		result = (*bufferPlayer4)->SetPlayState(bufferPlayer4, SL_PLAYSTATE_PLAYING);
		assert(SL_RESULT_SUCCESS == result);
		//if (SL_RESULT_SUCCESS == result)
		//LOGD("Playing Audio Clip");

		bufferIsDone4 = false;
		(void)result;
	}
	else if (bufferIsDone5)
	{
		result = (*bufferQueue5)->Enqueue(bufferQueue5, nextBuffer, nextSize);
		//if (SL_RESULT_SUCCESS == result)
		//LOGD("Audio Clip added to Buffer Queue 5");

		result = (*bufferPlayer5)->SetPlayState(bufferPlayer5, SL_PLAYSTATE_PLAYING);
		assert(SL_RESULT_SUCCESS == result);
		//if (SL_RESULT_SUCCESS == result)
		//LOGD("Playing Audio Clip");

		bufferIsDone5 = false;
		(void)result;
	}
	else if (bufferIsDone6)
	{
		result = (*bufferQueue6)->Enqueue(bufferQueue6, nextBuffer, nextSize);
		//if (SL_RESULT_SUCCESS == result)
		//LOGD("Audio Clip added to Buffer Queue 6");

		result = (*bufferPlayer6)->SetPlayState(bufferPlayer6, SL_PLAYSTATE_PLAYING);
		assert(SL_RESULT_SUCCESS == result);
		//if (SL_RESULT_SUCCESS == result)
		//LOGD("Playing Audio Clip");

		bufferIsDone6 = false;
		(void)result;
	}
	else if (bufferIsDone7)
	{
		result = (*bufferQueue7)->Enqueue(bufferQueue7, nextBuffer, nextSize);
		//if (SL_RESULT_SUCCESS == result)
		//LOGD("Audio Clip added to Buffer Queue 7");

		result = (*bufferPlayer7)->SetPlayState(bufferPlayer7, SL_PLAYSTATE_PLAYING);
		assert(SL_RESULT_SUCCESS == result);
		//if (SL_RESULT_SUCCESS == result)
		//LOGD("Playing Audio Clip");

		bufferIsDone7 = false;
		(void)result;
	}
	else if (bufferIsDone8)
	{
		result = (*bufferQueue8)->Enqueue(bufferQueue8, nextBuffer, nextSize);
		//if (SL_RESULT_SUCCESS == result)
		//LOGD("Audio Clip added to Buffer Queue 8");

		result = (*bufferPlayer8)->SetPlayState(bufferPlayer8, SL_PLAYSTATE_PLAYING);
		assert(SL_RESULT_SUCCESS == result);
		//if (SL_RESULT_SUCCESS == result)
		//LOGD("Playing Audio Clip");

		bufferIsDone8 = false;
		(void)result;
	}
	else
	{
		//Queue on first buffer queue to get played next
		result = (*bufferQueue1)->Enqueue(bufferQueue1, nextBuffer, nextSize);
		if (SL_RESULT_SUCCESS == result)
			//LOGD("Audio Clip added to follow Buffer Queue 1");

			result = (*bufferPlayer1)->SetPlayState(bufferPlayer1, SL_PLAYSTATE_PLAYING);
		assert(SL_RESULT_SUCCESS == result);
		if (SL_RESULT_SUCCESS == result)
			//LOGD("Playing Audio Clip");

			bufferIsDone1 = false;
		(void)result;
	}

}
#endif

//Callback to check and see if buffer has reached end of data
void SLAPIENTRY playCallback1(SLPlayItf buffer, void* context, SLuint32 event)
{
	if (event& SL_PLAYEVENT_HEADATEND)
	{
		bufferIsDone1 = true;
		//LOGD("Buffer1 has reached end");
	}
}

//Callback to check and see if buffer has reached end of data
void SLAPIENTRY playCallback2(SLPlayItf buffer, void* context, SLuint32 event)
{
	if (event& SL_PLAYEVENT_HEADATEND)
	{
		bufferIsDone2 = true;
		//LOGD("Buffer2 has reached end");
	}
}

//Callback to check and see if buffer has reached end of data
void SLAPIENTRY playCallback3(SLPlayItf buffer, void* context, SLuint32 event)
{
	if (event& SL_PLAYEVENT_HEADATEND)
	{
		bufferIsDone3 = true;
		//LOGD("Buffer3 has reached end");
	}
}

//Callback to check and see if buffer has reached end of data
void SLAPIENTRY playCallback4(SLPlayItf buffer, void* context, SLuint32 event)
{
	if (event& SL_PLAYEVENT_HEADATEND)
	{
		bufferIsDone4 = true;
		//LOGD("Buffer4 has reached end");
	}
}

//Callback to check and see if buffer has reached end of data
void SLAPIENTRY playCallback5(SLPlayItf buffer, void* context, SLuint32 event)
{
	if (event& SL_PLAYEVENT_HEADATEND)
	{
		bufferIsDone5 = true;
		//LOGD("Buffer5 has reached end");
	}
}

//Callback to check and see if buffer has reached end of data
void SLAPIENTRY playCallback6(SLPlayItf buffer, void* context, SLuint32 event)
{
	if (event& SL_PLAYEVENT_HEADATEND)
	{
		bufferIsDone6 = true;
		//LOGD("Buffer6 has reached end");
	}
}

//Callback to check and see if buffer has reached end of data
void SLAPIENTRY playCallback7(SLPlayItf buffer, void* context, SLuint32 event)
{
	if (event& SL_PLAYEVENT_HEADATEND)
	{
		bufferIsDone7 = true;
		//LOGD("Buffer7 has reached end");
	}
}

//Callback to check and see if buffer has reached end of data
void SLAPIENTRY playCallback8(SLPlayItf buffer, void* context, SLuint32 event)
{
	if (event& SL_PLAYEVENT_HEADATEND)
	{
		bufferIsDone8 = true;
		//LOGD("Buffer8 has reached end");
	}
}

//Callback to check and see if buffer has reached end of data
void SLAPIENTRY playCallbackBGM(SLPlayItf player, void* context, SLuint32 event)
{
	if (event& SL_PLAYEVENT_HEADATEND)
	{
		bgmIsDone = true;
		//LOGD("BGM has reached end");
	}
}

void OSLESMediaPlayer::setPlayState(SLuint32 state, SLPlayItf& player)
{
    SLresult result;

    // make sure the audio player was created
    if (NULL != player) 
	{
        // set the player's state
        result = (*player)->SetPlayState(player, state);
        assert(SL_RESULT_SUCCESS == result);
    }
	
}

SLuint32 OSLESMediaPlayer::getPlayState(SLPlayItf& player)
{
    SLresult result;
	
    // make sure the audio player was created
    if (NULL != player) 
	{
        SLuint32 state;
        result = (*player)->GetPlayState(player, &state);
        assert(SL_RESULT_SUCCESS == result);

        return state;
    }
	
    return 0;
}
#endif

OSLESMediaPlayer::OSLESMediaPlayer()
{
	createEngine();
	createAudioPlayer();
}

void OSLESMediaPlayer::createEngine()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	SLresult result;

	// create engine

	//LOGD("create engine");
	result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
	assert(SL_RESULT_SUCCESS == result);
	(void)result;

	// realize the engine
	//LOGD("realize the engine");
	result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result);
	(void)result;

	// get the engine interface, which is needed in order to create other objects
	//LOGD("get the engine interface");
	result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
	assert(SL_RESULT_SUCCESS == result);
	(void)result;

	// create output mix, with playback rate specified as a non-required interface
	//LOGD("create output mix");
	const SLInterfaceID ids[0] = {}; //LOAD ANY NECESSARY OUTPUT EFFECTS HERE ()
	const SLboolean req[0] = {};
	result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, ids, req);
	assert(SL_RESULT_SUCCESS == result);
	(void)result;

	// realize the output mix
	//LOGD("realize the output mix");
	result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result);
	(void)result;
#endif
}

void OSLESMediaPlayer::releaseEngine()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	// destroy audio player object, and invalidate all associated interfaces

	playerSeek = NULL;
	bgmPlaybackRate = NULL;

	if (bgmPlayerObject != NULL) 
	{
		(*bgmPlayerObject)->Destroy(bgmPlayerObject);
		bgmPlayerObject = NULL;
		bgmPlayer = NULL;
	}

	if (bufferObject1 != NULL)
	{
		(*bufferObject1)->Destroy(bufferObject1);
		bufferObject1 = NULL;
		bufferPlayer1 = NULL;
	}
	if (bufferObject2 != NULL)
	{
		(*bufferObject2)->Destroy(bufferObject2);
		bufferObject2 = NULL;
		bufferPlayer2 = NULL;
	}
	if (bufferObject3 != NULL)
	{
		(*bufferObject3)->Destroy(bufferObject3);
		bufferObject3 = NULL;
		bufferPlayer3 = NULL;
	}
	if (bufferObject4 != NULL)
	{
		(*bufferObject4)->Destroy(bufferObject4);
		bufferObject4 = NULL;
		bufferPlayer4 = NULL;
	}
	if (bufferObject5 != NULL)
	{
		(*bufferObject5)->Destroy(bufferObject5);
		bufferObject5 = NULL;
		bufferPlayer5 = NULL;
	}
	if (bufferObject6 != NULL)
	{
		(*bufferObject6)->Destroy(bufferObject6);
		bufferObject6 = NULL;
		bufferPlayer6 = NULL;
	}
	if (bufferObject7 != NULL)
	{
		(*bufferObject7)->Destroy(bufferObject7);
		bufferObject7 = NULL;
		bufferPlayer7 = NULL;
	}
	if (bufferObject8 != NULL)
	{
		(*bufferObject8)->Destroy(bufferObject8);
		bufferObject8 = NULL;
		bufferPlayer8 = NULL;
	}

	// destroy output mix object, and invalidate all associated interfaces
	if (outputMixObject != NULL) 
	{
		(*outputMixObject)->Destroy(outputMixObject);
		outputMixObject = NULL;
	}

	// destroy engine object, and invalidate all associated interfaces
	if (engineObject != NULL) 
	{
		(*engineObject)->Destroy(engineObject);
		engineObject = NULL;
		engineEngine = NULL;
	}
#endif
}

bool OSLESMediaPlayer::createAudioPlayer()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	SLresult result;

	//
	//CREATE BUFFER PLAYERS 
	//

	// configure audio source
	SLDataLocator_AndroidSimpleBufferQueue loc_bufq = { SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2 };
	SLDataFormat_PCM format_pcm = 
	{ 
		SL_DATAFORMAT_PCM, 
		1, 
		SL_SAMPLINGRATE_44_1,
		SL_PCMSAMPLEFORMAT_FIXED_16, 
		SL_PCMSAMPLEFORMAT_FIXED_16,
		SL_SPEAKER_FRONT_CENTER, 
		SL_BYTEORDER_LITTLEENDIAN 
	};
	
	SLDataSource audioSrc = { &loc_bufq, &format_pcm };

	// configure audio sink
	SLDataLocator_OutputMix loc_outmix = { SL_DATALOCATOR_OUTPUTMIX, outputMixObject };
	SLDataSink audioSnk = { &loc_outmix, NULL };

	// create audio players
	const SLInterfaceID ids[2] = { SL_IID_BUFFERQUEUE, SL_IID_VOLUME };
	const SLboolean req[1] = { SL_BOOLEAN_TRUE };
	result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bufferObject1, &audioSrc, &audioSnk, 2, ids, req);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Buffer 1 created");
	(void)result;
	result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bufferObject2, &audioSrc, &audioSnk, 2, ids, req);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Buffer 2 created");
	(void)result;
	result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bufferObject3, &audioSrc, &audioSnk, 2, ids, req);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Buffer 3 created");
	(void)result;
	result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bufferObject4, &audioSrc, &audioSnk, 2, ids, req);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Buffer 4 created");
	(void)result;
	result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bufferObject5, &audioSrc, &audioSnk, 2, ids, req);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Buffer 5 created");
	(void)result;
	result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bufferObject6, &audioSrc, &audioSnk, 2, ids, req);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Buffer 6 created");
	(void)result;
	result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bufferObject7, &audioSrc, &audioSnk, 2, ids, req);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Buffer 7 created");
	(void)result;
	result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bufferObject8, &audioSrc, &audioSnk, 2, ids, req);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Buffer 8 created");
	(void)result;

	// realize the players
	result = (*bufferObject1)->Realize(bufferObject1, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Buffer 1 realized");
	(void)result;
	result = (*bufferObject2)->Realize(bufferObject2, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Buffer 2 realized");
	(void)result;
	result = (*bufferObject3)->Realize(bufferObject3, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Buffer 3 realized");
	(void)result;
	result = (*bufferObject4)->Realize(bufferObject4, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Buffer 4 realized");
	(void)result;
	result = (*bufferObject5)->Realize(bufferObject5, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Buffer 5 realized");
	(void)result;
	result = (*bufferObject6)->Realize(bufferObject6, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Buffer 6 realized");
	(void)result;
	result = (*bufferObject7)->Realize(bufferObject7, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Buffer 7 realized");
	(void)result;
	result = (*bufferObject8)->Realize(bufferObject8, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Buffer 8 realized");
	(void)result;
	
	// get the play interfaces
	result = (*bufferObject1)->GetInterface(bufferObject1, SL_IID_PLAY, &bufferPlayer1);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Received Buffer 1 PLAYER Interface");
	(void)result;
	result = (*bufferObject2)->GetInterface(bufferObject2, SL_IID_PLAY, &bufferPlayer2);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Received Buffer 2 PLAYER Interface");
	(void)result;
	result = (*bufferObject3)->GetInterface(bufferObject3, SL_IID_PLAY, &bufferPlayer3);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Received Buffer 3 PLAYER Interface");
	(void)result;
	result = (*bufferObject4)->GetInterface(bufferObject4, SL_IID_PLAY, &bufferPlayer4);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Received Buffer 4 PLAYER Interface");
	(void)result;
	result = (*bufferObject5)->GetInterface(bufferObject5, SL_IID_PLAY, &bufferPlayer5);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Received Buffer 5 PLAYER Interface");
	(void)result;
	result = (*bufferObject6)->GetInterface(bufferObject6, SL_IID_PLAY, &bufferPlayer6);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Received Buffer 6 PLAYER Interface");
	(void)result;
	result = (*bufferObject7)->GetInterface(bufferObject7, SL_IID_PLAY, &bufferPlayer7);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Received Buffer 7 PLAYER Interface");
	(void)result;
	result = (*bufferObject8)->GetInterface(bufferObject8, SL_IID_PLAY, &bufferPlayer8);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Received Buffer 8 PLAYER Interface");
	(void)result;

	// get the buffer queue interfaces
	result = (*bufferObject1)->GetInterface(bufferObject1, SL_IID_BUFFERQUEUE, &bufferQueue1);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Received Buffer 1 BUFFER Interface");
	(void)result;
	result = (*bufferObject2)->GetInterface(bufferObject2, SL_IID_BUFFERQUEUE, &bufferQueue2);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Received Buffer 2 BUFFER Interface");
	(void)result;
	result = (*bufferObject3)->GetInterface(bufferObject3, SL_IID_BUFFERQUEUE, &bufferQueue3);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Received Buffer 3 BUFFER Interface");
	(void)result;
	result = (*bufferObject4)->GetInterface(bufferObject4, SL_IID_BUFFERQUEUE, &bufferQueue4);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Received Buffer 4 BUFFER Interface");
	(void)result;
	result = (*bufferObject5)->GetInterface(bufferObject5, SL_IID_BUFFERQUEUE, &bufferQueue5);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Received Buffer 5 BUFFER Interface");
	(void)result;
	result = (*bufferObject6)->GetInterface(bufferObject6, SL_IID_BUFFERQUEUE, &bufferQueue6);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Received Buffer 6 BUFFER Interface");
	(void)result;
	result = (*bufferObject7)->GetInterface(bufferObject7, SL_IID_BUFFERQUEUE, &bufferQueue7);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Received Buffer 7 BUFFER Interface");
	(void)result;
	result = (*bufferObject8)->GetInterface(bufferObject8, SL_IID_BUFFERQUEUE, &bufferQueue8);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Received Buffer 8 BUFFER Interface");
	(void)result;

	// get the volume interfaces
	result = (*bufferObject1)->GetInterface(bufferObject1, SL_IID_VOLUME, &playerVolume);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Received Volume Interface");
	(void)result;
	result = (*bufferObject2)->GetInterface(bufferObject2, SL_IID_VOLUME, &playerVolume);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Received Volume Interface");
	(void)result;
	result = (*bufferObject3)->GetInterface(bufferObject3, SL_IID_VOLUME, &playerVolume);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Received Volume Interface");
	(void)result;
	result = (*bufferObject4)->GetInterface(bufferObject4, SL_IID_VOLUME, &playerVolume);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Received Volume Interface");
	(void)result;
	result = (*bufferObject5)->GetInterface(bufferObject5, SL_IID_VOLUME, &playerVolume);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Received Volume Interface");
	(void)result;
	result = (*bufferObject6)->GetInterface(bufferObject6, SL_IID_VOLUME, &playerVolume);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Received Volume Interface");
	(void)result;
	result = (*bufferObject7)->GetInterface(bufferObject7, SL_IID_VOLUME, &playerVolume);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Received Volume Interface");
	(void)result;
	result = (*bufferObject8)->GetInterface(bufferObject8, SL_IID_VOLUME, &playerVolume);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Received Volume Interface");
	(void)result;

	// register callback on the buffer queue
	result = (*bufferQueue1)->RegisterCallback(bufferQueue1, bufferPlayerCallback1, NULL);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Buffer 1 callback registered");
	(void)result;
	result = (*bufferQueue2)->RegisterCallback(bufferQueue2, bufferPlayerCallback2, NULL);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Buffer 2 callback registered");
	(void)result;
	result = (*bufferQueue3)->RegisterCallback(bufferQueue3, bufferPlayerCallback3, NULL);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Buffer 3 callback registered");
	(void)result;
	result = (*bufferQueue4)->RegisterCallback(bufferQueue4, bufferPlayerCallback4, NULL);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Buffer 4 callback registered");
	(void)result;
	result = (*bufferQueue5)->RegisterCallback(bufferQueue5, bufferPlayerCallback5, NULL);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Buffer 5 callback registered");
	(void)result;
	result = (*bufferQueue6)->RegisterCallback(bufferQueue6, bufferPlayerCallback6, NULL);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Buffer 6 callback registered");
	(void)result;
	result = (*bufferQueue7)->RegisterCallback(bufferQueue7, bufferPlayerCallback7, NULL);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Buffer 7 callback registered");
	(void)result;
	result = (*bufferQueue8)->RegisterCallback(bufferQueue8, bufferPlayerCallback8, NULL);
	assert(SL_RESULT_SUCCESS == result);
	//LOGD("Buffer 8 callback registered");
	(void)result;

	//Register and set callback to check if player is done
	(*bufferPlayer1)->RegisterCallback(bufferPlayer1, playCallback1, nullptr);
	(*bufferPlayer1)->SetCallbackEventsMask(bufferPlayer1, SL_PLAYEVENT_HEADATEND);
	(*bufferPlayer2)->RegisterCallback(bufferPlayer2, playCallback2, nullptr);
	(*bufferPlayer2)->SetCallbackEventsMask(bufferPlayer2, SL_PLAYEVENT_HEADATEND);
	(*bufferPlayer3)->RegisterCallback(bufferPlayer3, playCallback3, nullptr);
	(*bufferPlayer3)->SetCallbackEventsMask(bufferPlayer3, SL_PLAYEVENT_HEADATEND);
	(*bufferPlayer4)->RegisterCallback(bufferPlayer4, playCallback4, nullptr);
	(*bufferPlayer4)->SetCallbackEventsMask(bufferPlayer4, SL_PLAYEVENT_HEADATEND);
	(*bufferPlayer5)->RegisterCallback(bufferPlayer5, playCallback5, nullptr);
	(*bufferPlayer5)->SetCallbackEventsMask(bufferPlayer5, SL_PLAYEVENT_HEADATEND);
	(*bufferPlayer6)->RegisterCallback(bufferPlayer6, playCallback6, nullptr);
	(*bufferPlayer6)->SetCallbackEventsMask(bufferPlayer6, SL_PLAYEVENT_HEADATEND);
	(*bufferPlayer7)->RegisterCallback(bufferPlayer7, playCallback7, nullptr);
	(*bufferPlayer7)->SetCallbackEventsMask(bufferPlayer7, SL_PLAYEVENT_HEADATEND);
	(*bufferPlayer8)->RegisterCallback(bufferPlayer8, playCallback8, nullptr);
	(*bufferPlayer8)->SetCallbackEventsMask(bufferPlayer8, SL_PLAYEVENT_HEADATEND);

	// set the player's state to playing
	result = (*bufferPlayer1)->SetPlayState(bufferPlayer1, SL_PLAYSTATE_PLAYING);
	assert(SL_RESULT_SUCCESS == result);
	(void)result;
	result = (*bufferPlayer2)->SetPlayState(bufferPlayer2, SL_PLAYSTATE_PLAYING);
	assert(SL_RESULT_SUCCESS == result);
	(void)result;
	result = (*bufferPlayer3)->SetPlayState(bufferPlayer3, SL_PLAYSTATE_PLAYING);
	assert(SL_RESULT_SUCCESS == result);
	(void)result;
	result = (*bufferPlayer4)->SetPlayState(bufferPlayer4, SL_PLAYSTATE_PLAYING);
	assert(SL_RESULT_SUCCESS == result);
	(void)result;
	result = (*bufferPlayer5)->SetPlayState(bufferPlayer5, SL_PLAYSTATE_PLAYING);
	assert(SL_RESULT_SUCCESS == result);
	(void)result;
	result = (*bufferPlayer6)->SetPlayState(bufferPlayer6, SL_PLAYSTATE_PLAYING);
	assert(SL_RESULT_SUCCESS == result);
	(void)result;
	result = (*bufferPlayer7)->SetPlayState(bufferPlayer7, SL_PLAYSTATE_PLAYING);
	assert(SL_RESULT_SUCCESS == result);
	(void)result;
	result = (*bufferPlayer8)->SetPlayState(bufferPlayer8, SL_PLAYSTATE_PLAYING);
	assert(SL_RESULT_SUCCESS == result);
	(void)result;

#endif
	return true;
}

void OSLESMediaPlayer::StartBGM(std::string clip)
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	//LOGD("Loading BGM audio file");

	//Get Cocos file path for clip
	std::string prefix = "Audio/BGM/";
	std::string suffix = ".wav";
	std::string filepathRelative = prefix + clip + suffix;
	std::string fullPath = CocosDenshion::android::getFullPathWithoutAssetsPrefix(filepathRelative.c_str());
	//LOGD("Full Path retrieved");

	//Load the asset using Java Asset Manager (get asset manager using Cocos)
	auto asset = AAssetManager_open(cocos2d::FileUtilsAndroid::getAssetManager(), filepathRelative.c_str(), AASSET_MODE_UNKNOWN);
	assert(asset != NULL);
	//LOGD("File loaded by Asset Manager");

	// open asset as file descriptor
	off_t start, length;
	
	int fd = AAsset_openFileDescriptor(asset, &start, &length);
	assert(0 <= fd);
	AAsset_close(asset);
	//LOGD("File stored in variable. File closed");

	// configure audio source
	//LOGD("BGM File opened. Configuring audio source from file");
	SLDataLocator_AndroidFD loc_fd = { SL_DATALOCATOR_ANDROIDFD, fd, start, length };
	SLDataFormat_MIME format_mime = { SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED };
	SLDataSource audioSrc = { &loc_fd, &format_mime };

	SLresult result;

	// configure audio sink
	SLDataLocator_OutputMix loc_outmix = { SL_DATALOCATOR_OUTPUTMIX, outputMixObject };
	SLDataSink audioSnk = { &loc_outmix, NULL };
	//LOGD("BGM Audio sink configured");

	// create audio player
	//LOGD("Creating BGM audio player");
	const SLInterfaceID ids[1] = { SL_IID_SEEK/*, SL_IID_PLAYBACKRATE*/ };
	const SLboolean req[1] = { SL_BOOLEAN_FALSE/*, SL_BOOLEAN_TRUE*/ };
	result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bgmPlayerObject, &audioSrc, &audioSnk, 1, ids, req);
	assert(SL_RESULT_SUCCESS == result);

	// realize the player
	//LOGD("Realizng BGM audio player");
	result = (*bgmPlayerObject)->Realize(bgmPlayerObject, SL_BOOLEAN_FALSE);
	// this will always succeed on Android, but we check result for portability to other platforms
	if (SL_RESULT_SUCCESS != result) 
	{
		(*bgmPlayerObject)->Destroy(bgmPlayerObject);
		bgmPlayerObject = NULL;
	}

	// get the play interface
	//LOGD("Getting BGM audio player interface");
	result = (*bgmPlayerObject)->GetInterface(bgmPlayerObject, SL_IID_PLAY, &bgmPlayer);
	assert(SL_RESULT_SUCCESS == result);

	// get the seek interface
	//LOGD("Getting BGM seek interface");
	result = (*bgmPlayerObject)->GetInterface(bgmPlayerObject, SL_IID_SEEK, &playerSeek);
	assert(SL_RESULT_SUCCESS == result);
	
	/*
	// get playback rate interface
	//LOGD("Getting BGM Buffer playback rate interface");
	result = (*bgmPlayerObject)->GetInterface(bgmPlayerObject, SL_IID_PLAYBACKRATE, &bgmPlaybackRate);
	assert(SL_RESULT_SUCCESS == result);

	SLuint32 capa1;
	result = (*bgmPlaybackRate)->GetRateRange(bgmPlaybackRate, 0, &bgmPlaybackMinRate, &bgmPlaybackMaxRate, &bgmPlaybackRateStepSize, &capa1);
	assert(SL_RESULT_SUCCESS == result);

	result = (*bgmPlaybackRate)->SetPropertyConstraints(bgmPlaybackRate, SL_RATEPROP_NOPITCHCORAUDIO);

	if (SL_RESULT_PARAMETER_INVALID == result) {
		//LOGD("Parameter Invalid");
	}
	if (SL_RESULT_FEATURE_UNSUPPORTED == result) {
		//LOGD("Feature Unsupported");
	}
	if (SL_RESULT_SUCCESS == result) {
		assert(SL_RESULT_SUCCESS == result);
		//LOGD("Playback Rate Interface loaded");
		//LOGD("BGM Buffer Audo Player setup SUCCESS");
	}
	*/

	// register callback function
	//LOGD("Registering BGM audio player callback");
	result = (*bgmPlayer)->RegisterCallback(bgmPlayer, playCallbackBGM, 0);
	assert(SL_RESULT_SUCCESS == result);
	result = (*bgmPlayer)->SetCallbackEventsMask(bgmPlayer, SL_PLAYEVENT_HEADATEND); // head at end
	assert(SL_RESULT_SUCCESS == result);

	SLmillisecond msec;
	result = (*bgmPlayer)->GetDuration(bgmPlayer, &msec);
	assert(SL_RESULT_SUCCESS == result);

	// set to loop
	result = (*playerSeek)->SetLoop(playerSeek, SL_BOOLEAN_TRUE, 999, msec);
	assert(SL_RESULT_SUCCESS == result);

	//Begin playing bgm
	result = (*bgmPlayer)->SetPlayState(bgmPlayer, SL_PLAYSTATE_PLAYING);
	assert(SL_RESULT_SUCCESS == result);
	if (SL_RESULT_SUCCESS == result)
		//LOGD("Playing BGM");
	bgmIsDone = false;

#endif
	return;
}

void OSLESMediaPlayer::StopBGM()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

	if (bgmPlayerObject != NULL)
	{
		setPlayState(SL_PLAYSTATE_STOPPED, bgmPlayer);

		SLresult result;
		// set the player's state to playing
		result = (*bgmPlayer)->SetPlayState(bgmPlayer, SL_PLAYSTATE_STOPPED);
		assert(SL_RESULT_SUCCESS == result);
		if (SL_RESULT_SUCCESS == result)
			//LOGD("BGM audio Stopped");
		(void)result;
		ReleaseBGM();
}
#endif
}

void OSLESMediaPlayer::MuteSound()
{
	muteSound = true;
	/*
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	(*playerVolume)->SetVolumeLevel(playerVolume,
		(SLmillibel)(0));
#endif
	*/
	return;
}

void OSLESMediaPlayer::UnmuteSound()
{
	muteSound = false;
	return;
}

void OSLESMediaPlayer::ReleaseBGM()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	playerSeek = NULL;

	if (bgmPlayerObject != NULL)
	{
		(*bgmPlayerObject)->Destroy(bgmPlayerObject);
		bgmPlayerObject = NULL;
		bgmPlayer = NULL;
	}
#endif
	return;
}

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
void OSLESMediaPlayer::Stop(SLPlayItf& player)
{
	setPlayState(SL_PLAYSTATE_STOPPED, player);

	SLresult result;
	// set the player's state to playing
	result = (*player)->SetPlayState(player, SL_PLAYSTATE_STOPPED);
	assert(SL_RESULT_SUCCESS == result);
	if (SL_RESULT_SUCCESS == result)
		//LOGD("Player Stopped");
	(void)result;
}
void OSLESMediaPlayer::StopAll()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	//LOGD("Stopping all Audio");

	SLresult result;

	// set all buffer states to pause
	if (bufferPlayer1 != NULL)
	{
		setPlayState(SL_PLAYSTATE_STOPPED, bufferPlayer1);
		result = (*bufferPlayer1)->SetPlayState(bufferPlayer1, SL_PLAYSTATE_STOPPED);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
	if (bufferPlayer2 != NULL)
	{
		setPlayState(SL_PLAYSTATE_STOPPED, bufferPlayer2);
		result = (*bufferPlayer2)->SetPlayState(bufferPlayer2, SL_PLAYSTATE_STOPPED);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
	if (bufferPlayer3 != NULL)
	{
		setPlayState(SL_PLAYSTATE_STOPPED, bufferPlayer3);
		result = (*bufferPlayer3)->SetPlayState(bufferPlayer3, SL_PLAYSTATE_STOPPED);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
	if (bufferPlayer4 != NULL)
	{
		setPlayState(SL_PLAYSTATE_STOPPED, bufferPlayer4);
		result = (*bufferPlayer4)->SetPlayState(bufferPlayer4, SL_PLAYSTATE_STOPPED);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
	if (bufferPlayer5 != NULL)
	{
		setPlayState(SL_PLAYSTATE_STOPPED, bufferPlayer5);
		result = (*bufferPlayer5)->SetPlayState(bufferPlayer5, SL_PLAYSTATE_STOPPED);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
	if (bufferPlayer6 != NULL)
	{
		setPlayState(SL_PLAYSTATE_STOPPED, bufferPlayer6);
		result = (*bufferPlayer6)->SetPlayState(bufferPlayer6, SL_PLAYSTATE_STOPPED);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
	if (bufferPlayer7 != NULL)
	{
		setPlayState(SL_PLAYSTATE_PAUSED, bufferPlayer7);
		result = (*bufferPlayer7)->SetPlayState(bufferPlayer7, SL_PLAYSTATE_PAUSED);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
	if (bufferPlayer8 != NULL)
	{
		setPlayState(SL_PLAYSTATE_PAUSED, bufferPlayer8);
		result = (*bufferPlayer8)->SetPlayState(bufferPlayer8, SL_PLAYSTATE_PAUSED);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
	if (bgmPlayer != NULL)
	{
		setPlayState(SL_PLAYSTATE_PAUSED, bgmPlayer);
		result = (*bgmPlayer)->SetPlayState(bgmPlayer, SL_PLAYSTATE_PAUSED);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
#endif
}
#endif	

void OSLESMediaPlayer::Pause()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	//LOGD("Pausing all Audio");

	SLresult result;

	// set all buffer states to pause
	if (bufferPlayer1 != NULL)
	{
		setPlayState(SL_PLAYSTATE_PAUSED, bufferPlayer1);
		result = (*bufferPlayer1)->SetPlayState(bufferPlayer1, SL_PLAYSTATE_PAUSED);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
	if (bufferPlayer2 != NULL)
	{
		setPlayState(SL_PLAYSTATE_PAUSED, bufferPlayer2);
		result = (*bufferPlayer2)->SetPlayState(bufferPlayer2, SL_PLAYSTATE_PAUSED);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
	if (bufferPlayer3 != NULL)
	{
		setPlayState(SL_PLAYSTATE_PAUSED, bufferPlayer3);
		result = (*bufferPlayer3)->SetPlayState(bufferPlayer3, SL_PLAYSTATE_PAUSED);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
	if (bufferPlayer4 != NULL)
	{
		setPlayState(SL_PLAYSTATE_PAUSED, bufferPlayer4);
		result = (*bufferPlayer4)->SetPlayState(bufferPlayer4, SL_PLAYSTATE_PAUSED);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
	if (bufferPlayer5 != NULL)
	{
		setPlayState(SL_PLAYSTATE_PAUSED, bufferPlayer5);
		result = (*bufferPlayer5)->SetPlayState(bufferPlayer5, SL_PLAYSTATE_PAUSED);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
	if (bufferPlayer6 != NULL)
	{
		setPlayState(SL_PLAYSTATE_PAUSED, bufferPlayer6);
		result = (*bufferPlayer6)->SetPlayState(bufferPlayer6, SL_PLAYSTATE_PAUSED);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
	if (bufferPlayer7 != NULL)
	{
		setPlayState(SL_PLAYSTATE_PAUSED, bufferPlayer7);
		result = (*bufferPlayer7)->SetPlayState(bufferPlayer7, SL_PLAYSTATE_PAUSED);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
	if (bufferPlayer8 != NULL)
	{
		setPlayState(SL_PLAYSTATE_PAUSED, bufferPlayer8);
		result = (*bufferPlayer8)->SetPlayState(bufferPlayer8, SL_PLAYSTATE_PAUSED);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
	if (bgmPlayer != NULL)
	{
		setPlayState(SL_PLAYSTATE_PAUSED, bgmPlayer);
		result = (*bgmPlayer)->SetPlayState(bgmPlayer, SL_PLAYSTATE_PAUSED);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
#endif
	return;
}

void OSLESMediaPlayer::Resume()
{

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	
	//LOGD("Resuming all Audio");

	SLresult result;

	// set all buffer states to play
	if (bufferPlayer1 != NULL)
	{
		setPlayState(SL_PLAYSTATE_PLAYING, bufferPlayer1);
		result = (*bufferPlayer1)->SetPlayState(bufferPlayer1, SL_PLAYSTATE_PLAYING);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
	if (bufferPlayer2 != NULL)
	{
		setPlayState(SL_PLAYSTATE_PLAYING, bufferPlayer2);
		result = (*bufferPlayer2)->SetPlayState(bufferPlayer2, SL_PLAYSTATE_PLAYING);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
	if (bufferPlayer3 != NULL)
	{
		setPlayState(SL_PLAYSTATE_PLAYING, bufferPlayer3);
		result = (*bufferPlayer3)->SetPlayState(bufferPlayer3, SL_PLAYSTATE_PLAYING);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
	if (bufferPlayer4 != NULL)
	{
		setPlayState(SL_PLAYSTATE_PLAYING, bufferPlayer4);
		result = (*bufferPlayer4)->SetPlayState(bufferPlayer4, SL_PLAYSTATE_PLAYING);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
	if (bufferPlayer5 != NULL)
	{
		setPlayState(SL_PLAYSTATE_PLAYING, bufferPlayer5);
		result = (*bufferPlayer5)->SetPlayState(bufferPlayer5, SL_PLAYSTATE_PLAYING);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
	if (bufferPlayer6 != NULL)
	{
		setPlayState(SL_PLAYSTATE_PLAYING, bufferPlayer6);
		result = (*bufferPlayer6)->SetPlayState(bufferPlayer6, SL_PLAYSTATE_PLAYING);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
	if (bufferPlayer7 != NULL)
	{
		setPlayState(SL_PLAYSTATE_PLAYING, bufferPlayer7);
		result = (*bufferPlayer7)->SetPlayState(bufferPlayer7, SL_PLAYSTATE_PLAYING);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
	if (bufferPlayer8 != NULL)
	{
		setPlayState(SL_PLAYSTATE_PLAYING, bufferPlayer8);
		result = (*bufferPlayer8)->SetPlayState(bufferPlayer8, SL_PLAYSTATE_PLAYING);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}
	if (bgmPlayer != NULL)
	{
		setPlayState(SL_PLAYSTATE_PLAYING, bgmPlayer);
		result = (*bgmPlayer)->SetPlayState(bgmPlayer, SL_PLAYSTATE_PLAYING);
		assert(SL_RESULT_SUCCESS == result);
		(void)result;
	}

#endif
	return;
}

/*
bool OSLESMediaPlayer::IsPlaying()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	return (getPlayState() == SL_PLAYSTATE_PLAYING);
#endif
	return true;
}
*/

void OSLESMediaPlayer::SeekTo(int position)
{
	/*
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if (NULL != bufferPlayer) 
	{
		//SLuint32 state = getPlayState();
		//setPlayState(SL_PLAYSTATE_PAUSED);

		SLresult result;

		result = (*playerSeek)->SetPosition(playerSeek, position,
			SL_SEEKMODE_FAST);
		assert(SL_RESULT_SUCCESS == result);

		//setPlayState(state);
	}
#endif
*/
}

int OSLESMediaPlayer::GetDuration()
{
	/*
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if (NULL != bufferPlayer) 
	{
		SLresult result;

		SLmillisecond msec;
		result = (*bufferPlayer)->GetDuration(bufferPlayer, &msec);
		assert(SL_RESULT_SUCCESS == result);

		return msec;
	}
#endif
*/
	return 0.0f;
}

int OSLESMediaPlayer::GetPosition()
{
	/*
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if (NULL != bufferPlayer) 
	{

		SLresult result;

		SLmillisecond msec;
		result = (*bufferPlayer)->GetPosition(bufferPlayer, &msec);
		assert(SL_RESULT_SUCCESS == result);

		return msec;
	}
#endif
*/
	return 0.0f;
}

void OSLESMediaPlayer::SetRate(int rate)
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if (NULL != bgmPlaybackRate) 
	{
		//LOGD("Changing Playback Rate");
		SLresult result;

		result = (*bgmPlaybackRate)->SetRate(bgmPlaybackRate, rate);
		assert(SL_RESULT_SUCCESS == result);
	}
#endif
}

int OSLESMediaPlayer::GetRate()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if (NULL != bgmPlaybackRate) 
	{
		SLresult result;

		SLpermille rate;
		result = (*bgmPlaybackRate)->GetRate(bgmPlaybackRate, &rate);
		assert(SL_RESULT_SUCCESS == result);

		return rate;
	}
#endif
	return 0;
}

bool OSLESMediaPlayer::SetLoop(int startPos, int endPos)
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	SLresult result;

	result = (*playerSeek)->SetLoop(playerSeek, SL_BOOLEAN_TRUE, startPos,
		endPos);
	assert(SL_RESULT_SUCCESS == result);
#endif
	return true;

}

bool OSLESMediaPlayer::SetNoLoop()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	SLresult result;
	if (NULL != playerSeek)
	{
		// enable whole file looping
		result = (*playerSeek)->SetLoop(playerSeek, SL_BOOLEAN_TRUE, 0,
			SL_TIME_UNKNOWN);
		assert(SL_RESULT_SUCCESS == result);

	}
#endif
	return true;
}

bool OSLESMediaPlayer::CheckCompletion()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

	if (getPlayState(bufferPlayer1) == SL_PLAYSTATE_PLAYING && bufferIsDone1)
	{
		//Stop(bufferPlayer1);
		(*bufferQueue1)->Clear(bufferQueue1);
		//LOGD("Buffer1 Cleared");
		//LOGD("Enqueing silence to prevent buffer starvation");
		//Queue on first buffer queue to get played next
		(*bufferQueue1)->Enqueue(bufferQueue1, (short*)(silence), sizeof(silence));

	}
	if (getPlayState(bufferPlayer2) == SL_PLAYSTATE_PLAYING && bufferIsDone2)
	{
		//Stop(bufferPlayer2);
		(*bufferQueue2)->Clear(bufferQueue2);
		//LOGD("Buffer2 Cleared");
		//LOGD("Enqueing silence to prevent buffer starvation");
		//Queue on first buffer queue to get played next
		(*bufferQueue2)->Enqueue(bufferQueue2, (short*)(silence), sizeof(silence));
	}
	if (getPlayState(bufferPlayer3) == SL_PLAYSTATE_PLAYING && bufferIsDone3)
	{
		//Stop(bufferPlayer3);
		(*bufferQueue3)->Clear(bufferQueue3);
		//LOGD("Buffer3 Cleared");
		//LOGD("Enqueing silence to prevent buffer starvation");
		//Queue on first buffer queue to get played next
		(*bufferQueue3)->Enqueue(bufferQueue3, (short*)(silence), sizeof(silence));
	}
	if (getPlayState(bufferPlayer4) == SL_PLAYSTATE_PLAYING && bufferIsDone4)
	{
		//Stop(bufferPlayer4);
		(*bufferQueue4)->Clear(bufferQueue4);
		//LOGD("Buffer4 Cleared");
		//LOGD("Enqueing silence to prevent buffer starvation");
		//Queue on first buffer queue to get played next
		(*bufferQueue4)->Enqueue(bufferQueue4, (short*)(silence), sizeof(silence));
	}
	if (getPlayState(bufferPlayer5) == SL_PLAYSTATE_PLAYING && bufferIsDone5)
	{
		//Stop(bufferPlayer5);
		(*bufferQueue5)->Clear(bufferQueue5);
		//LOGD("Buffer5 Cleared");
		//LOGD("Enqueing silence to prevent buffer starvation");
		//Queue on first buffer queue to get played next
		(*bufferQueue5)->Enqueue(bufferQueue5, (short*)(silence), sizeof(silence));
	}
	if (getPlayState(bufferPlayer6) == SL_PLAYSTATE_PLAYING && bufferIsDone6)
	{
		//Stop(bufferPlayer6);
		(*bufferQueue6)->Clear(bufferQueue6);
		//LOGD("Buffer6 Cleared");
		//LOGD("Enqueing silence to prevent buffer starvation");
		//Queue on first buffer queue to get played next
		(*bufferQueue6)->Enqueue(bufferQueue6, (short*)(silence), sizeof(silence));
	}
	if (getPlayState(bufferPlayer7) == SL_PLAYSTATE_PLAYING && bufferIsDone7)
	{
		//Stop(bufferPlayer7);
		(*bufferQueue7)->Clear(bufferQueue7);
		//LOGD("Buffer7 Cleared");
		//LOGD("Enqueing silence to prevent buffer starvation");
		//Queue on first buffer queue to get played next
		(*bufferQueue7)->Enqueue(bufferQueue7, (short*)(silence), sizeof(silence));
	}
	if (getPlayState(bufferPlayer8) == SL_PLAYSTATE_PLAYING && bufferIsDone8)
	{
		//Stop(bufferPlayer8);
		(*bufferQueue8)->Clear(bufferQueue8);
		//LOGD("Buffer8 Cleared");
		//LOGD("Enqueing silence to prevent buffer starvation");
		//Queue on first buffer queue to get played next
		(*bufferQueue8)->Enqueue(bufferQueue8, (short*)(silence), sizeof(silence));
	}

	int position = GetPosition();
	int duration = GetDuration();
	if (position == duration)
		return true;
	else
		return false;
#else
	return true;

#endif
}

OSLESMediaPlayer::~OSLESMediaPlayer()
{
	releaseEngine();
}
