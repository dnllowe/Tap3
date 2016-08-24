//
// pch.h
// Header for standard system include files.
//

#pragma once

//Third Party Frameworks
#if(CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
#include "../../proj.ios_mac/PluginChartboost.framework/Versions/A/Headers/PluginChartboost.h"
#include "../../proj.ios_mac/PluginAdMob.framework/Versions/A/Headers/PluginAdMob.h"
#include "../../proj.ios_mac/sdkbox.framework/Versions/A/Headers/Sdkbox.h"
#include "../../proj.ios_mac/sdkbox.framework/Versions/A/Headers/SdkboxAds.h"
#include "../../proj.ios_mac/PluginIAP.framework/Versions/A/Headers/PluginIAP.h"
#include "../../proj.ios_mac/PluginSdkboxPlay.framework/Versions/A/Headers/PluginSdkboxPlay.h"
#endif

/*
#include "../../../proj.android/jni/pluginadmob/PluginAdMob.h"
#include "../../../proj.android/jni/pluginchartboost/PluginChartboost.h"
#include "../../../proj.android/jni/pluginiap/PluginIAP.h"
#include "../../../proj.android/jni/pluginsdkboxplay/PluginSdkboxPlay.h"
#include "../../../proj.android/jni/sdkbox/Sdkbox.h"
//*/

//C Headers
#include <functional>
#include <algorithm>
#include <unordered_set>
#include <iomanip>
#include <iostream>

//Cocos Headers
#include "cocos2d.h"
#include "cocos-ext.h"
#include "AppDelegate.h"
#include "SimpleAudioEngine.h"
#include "AudioEngine.h"
#include "ui/CocosGUI.h"

//Android-specific
#include "OSLESMediaPlayer.h"
#include "AndroidAudioClips.h"

// for native Android audio
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "pluginchartboost\PluginChartboost.h"
#include "pluginadmob\PluginAdMob.h"
#include "pluginiap\PluginIAP.h"
#include "pluginsdkboxplay\PluginSdkboxPlay.h"
#include "sdkbox/Sdkbox.h"
#include "AndroidAudio.h"
#include <android/log.h>
#include <jni.h> //For Java code interface
#include "platform/android/jni/JniHelper.h"
#endif

//NRG_Engine Headers
#include "nrgFunctions.h"
#include "nrgTimer.h"
#include "nrgAudio.h"
#include "nrgScene.h"
#include "nrgSprite.h"
#include "nrgBox.h"
#include "nrgText.h"
#include "nrgMenu.h"
#include "nrgList.h"
#include "nrgButton.h"

//Tap3 headers

#include "GameData.h"
#include "Start.h"
#include "BaseTile.h"
#include "Tile.h"
#include "Game.h"
#include "MainMenu.h"
#include "Countdown.h"
#include "Classic.h"
#include "RushRelax.h"
#include "MemoryMode.h"
#include "Tutorial_Matching.h"
#include "Tutorial_Bonus.h"
#include "Options.h"
#include "HighScores.h"
#include "Store.h"

//GLOABLS
extern bool ATTEMPTED_SIGN_IN;

/*
***** SOME NOTES ON THE GAME *****

BONUS ITEMS
-Show Match and Eliminate are limited to use only BEFORE a user has selected a tile for the following reasons:
--If the user selected the wrong tile at any point, than any bonus used afterwards would be wasted, because their match is already failed.
-Time Plus and reset can be used whenever, because they act independently of selecting tiles (although Time Plus could be wasted if user selects wrong tiles)
-Time Plus is limited to 2 uses per card to prevent user from adding a high amount of time, only to accidentally fail and lose potential money or earned bonuses
-Earned bonuses are used up first because they are assumed to be less valuable to the player than bonuses purchased with real-world money
-Earned bonuses are randomized to create uncertainty and "luck" during play. This may also encourage in-app purchases to get the exact bonus the player prefers
-Bonuses are reused when closing and opening the app to prevent user from "losing" bonuses when the game re-randomizes tiles

RE RANDOMIZING TILES ON APP RESUME
-Tiles are re-randomized after backing out of the app and resuming to prevent user from taking screenshot and using it to cheat the match
-To prevent user from losing lost bonus items, bonus items are restored and reused instantly after resuming the app
-If the user selects and tile and backs out, the game HAS to enter a game over state. Here's why:
--Imagine the user selects the wrong tile, then the game re-randomizes and puts the user in the same state, with the wrong tile selected. It is impossible for the user to know which tile the game "pressed" for them, and whether it was correct or not
--Imagine the user selects the correct tile, then the game re-randomizes and puts the userin the same state, with one correct tile selected. It is impossible for the user to know which other two tiles match the now missing tile
--If all the tiles are reset and the player is not placed in the same state, then closing and opening the app allows a free "Reset" with no penalty, and the game can be cheated

RUSH_RELAX / PRACTICE / 100 MATCHES
-These are all synonymous with the same game mode. It changed names several times during developments
-For example, some variables may say "practice" (such as "practiceButton) or enum as RUSH_RELAX or have a string of "100 Matches". 
-The above all refer to variables, strings, and objects that affect the same game mode

*/