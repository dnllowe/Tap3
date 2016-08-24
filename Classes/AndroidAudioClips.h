#pragma once
#include "pch.h"

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//Include all audio clip headers

#include <jni.h> //For Java code interface
#include "platform/android/jni/JniHelper.h"

#include "silence.h"
#include "double_tone_low.h"
#include "double_tone_high.h"
#include "triple_tone_high.h"
#include "tone.h"
#include "warning_tone.h"
#include "low_tone.h"
#include "double_tone_higher.h"

#endif
