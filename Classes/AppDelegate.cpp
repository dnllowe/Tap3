#pragma once
#include "pch.h"

static cocos2d::Size designResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 320);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1024, 768);
static cocos2d::Size largeResolutionSize = cocos2d::Size(2048, 1536);

AppDelegate::AppDelegate() 
{
}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

	cocos2d::GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages, 
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = cocos2d::Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("Tap 3", Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
        glview = cocos2d::GLViewImpl::create("Tap 3");
#endif
        director->setOpenGLView(glview);
    }

	//Initialize SDKBox Plugins
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    sdkbox::IAP::init();
    sdkbox::IAP::refresh();
    sdkbox::PluginChartboost::init();
    sdkbox::PluginAdMob::init();
    sdkbox::PluginAdMob::cache("home");
	sdkbox::PluginSdkboxPlay::init();
	//Only attempt to sign in once, only Chartboost once
	if (!ATTEMPTED_SIGN_IN)
	{
		if (!sdkbox::PluginSdkboxPlay::isSignedIn())
			sdkbox::PluginSdkboxPlay::signin();
		ATTEMPTED_SIGN_IN = true;
	}
#endif

    //Load game audio
    nrgAudio* audio = nrgAudio::getInstance();
    audio->LoadClip("tone1");
    audio->LoadClip("tone2");
    audio->LoadClip("warning_tone");
    audio->LoadClip("low_tone");
    audio->LoadClip("double_tone_high");
    audio->LoadClip("double_tone_higher");
    audio->LoadClip("double_tone_low");
    audio->LoadClip("triple_tone_high");
    audio->LoadMusic("Tap3-Theme");
    
    //Volume needs to be lower for iOS to avoid clipping
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    audio->SetMusicVolume(Game::GetiOSMaxVolumeBG());
    audio->SetClipVolume(Game::GetiOSMaxVolumeSFX());
#endif
    // turn on display FPS
    director->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);
	
    // Set the design resolution
	//Load appropriate assets according to user device screen resolution
    nrgFunctions::MultiResolutionSupport(ResolutionPolicy::FIXED_HEIGHT);

	register_all_packages();
	
    //Check play frequency in past 3 days
    Game::CheckPlayFrequency();
    
	CCLOG("Starting Game");

    // run
   director->runWithScene(Start::CreateScene());

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() 
{
	cocos2d::Director::getInstance()->stopAnimation();

	//Pause all timers
	if (nrgScene::timers.size() > 0)
	{
		for (int iii = 0; iii < nrgScene::timers.size(); iii++)
		{
			if (nrgScene::timers[iii]->IsUnpauseOnResume())
				nrgScene::timers[iii]->Pause(true);
		}
	}

	//Randomize tiles again when user backs out of App to prevent cheating (taking pictures of tiles, using app display screen, etc)
	if (Game::getInstance())
	{
		if (Game::getInstance()->IsGameStarted() && !Game::getInstance()->IsGameOver() && Game::getInstance()->AllowRerandomize())
				Game::getInstance()->ReRandomizeTiles();
	}

	// if you use SimpleAudioEngine, it must be pause
	CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
	CocosDenshion::SimpleAudioEngine::getInstance()->pauseAllEffects();
	
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	if (nrgAudio::GetAndroidAudioPlayer() != NULL);
	nrgAudio::GetAndroidAudioPlayer()->Pause();
#endif
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() 
{
	cocos2d::Director::getInstance()->startAnimation();

	//Prevent Chartboost from constantly displaying interstitials on app resume
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	sdkbox::PluginChartboost::closeImpression();
#endif

	//Unpause all paused timers
	if (nrgScene::timers.size() > 0)
	{
		for (int iii = 0; iii < nrgScene::timers.size(); iii++)
		{
			if (nrgScene::timers[iii]->IsPaused() && nrgScene::timers[iii]->IsUnpauseOnResume())
				nrgScene::timers[iii]->Unpause();

		}
	}

	if (Game::GetMode() == Game::MEMORY)
		Game::getInstance()->RestartTimers();

	// if you use SimpleAudioEngine, it must resume here
	CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
	CocosDenshion::SimpleAudioEngine::getInstance()->resumeAllEffects();
	
	//Check play frequency in past 3 days
    Game::CheckPlayFrequency();
}
