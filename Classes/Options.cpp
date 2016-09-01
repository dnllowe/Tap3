#include "pch.h"
#include "Options.h"


Options::Options()
{
}


Options::~Options()
{
}

cocos2d::Scene * Options::CreateScene()
{
	//Create Scene
	cocos2d::Scene* scene = cocos2d::Scene::create();

	//Create layer
	cocos2d::Layer* layer = Options::create();

	scene->addChild(layer);

	return scene;
}

bool Options::init()
{
    if(!gameData)
        gameData = cocos2d::UserDefault::getInstance();

	//Initialize origin
	SCREEN_WIDTH = glview->getDesignResolutionSize().width;
	SCREEN_HEIGHT = glview->getDesignResolutionSize().height;
	center.x = SCREEN_WIDTH / 2;
	center.y = SCREEN_HEIGHT / 2;

	//Set clear color to white
	director->setClearColor(cocos2d::Color4F(1, 1, 1, 1));

	//Create cache and batch
	nrgFunctions::CreateSpriteBatch("spritebatch", batch, cache);
	addChild(batch);

	//Variables for text size. Will change based on platform
	int titleTextSize;
	int textSize;

    titleTextSize = 37;
    textSize = 20;
        
    std::string musicTextInput;
    std::string soundTextInput;
	std::string removeAdsTextInput;
	bool highlightMusic;;
	bool highlightSound;
	if (gameData->getBoolForKey(music_on, true))
	{
		musicTextInput = "On";
		highlightMusic = true;
	}
	else
	{
		musicTextInput = "Off";
		highlightMusic = false;
	}

	if (gameData->getBoolForKey(sound_on, true))
	{
		soundTextInput = "On";
		highlightSound = true;
	}
	else
	{
		soundTextInput = "Off";
		highlightSound = false;
	}

	if (gameData->getBoolForKey(ads_removed, false))
		removeAdsTextInput = "";
	else
		removeAdsTextInput = "Remove Ads ($2.99)";

	title = nrgText::create("Options", "fonts/alba.super.ttf", titleTextSize, SCREEN_WIDTH, 0, 0, cocos2d::Color4B(20, 175, 255, 128));
	musicText = nrgText::create("Music:", "fonts/POE.ttf", textSize, SCREEN_WIDTH, 0, 0, cocos2d::Color4B::GRAY);
	soundText = nrgText::create("Sound:", "fonts/POE.ttf", textSize, SCREEN_WIDTH, 0, 0, cocos2d::Color4B::GRAY);
	musicOnOff = nrgMenu::create(musicTextInput, "fonts/POE.ttf", 0, textSize, 0, cocos2d::Color4B::GRAY, cocos2d::Color4B::YELLOW, false);
	soundOnOff = nrgMenu::create(soundTextInput, "fonts/POE.ttf", 0, textSize, 0, cocos2d::Color4B::GRAY, cocos2d::Color4B::YELLOW, false);
	removeAds = nrgMenu::create(removeAdsTextInput, "fonts/POE.ttf", 0, textSize, 0, cocos2d::Color4B::GRAY, cocos2d::Color4B::YELLOW, false);
	howToPlay = nrgMenu::create("Tutorial: How To Play", "fonts/POE.ttf", 0, textSize, 0, cocos2d::Color4B::GRAY, cocos2d::Color4B::YELLOW, false);
	usingBonusItems = nrgMenu::create("Tutorial: Using Bonus Items", "fonts/POE.ttf", 0, textSize, 0, cocos2d::Color4B::GRAY, cocos2d::Color4B::YELLOW, false);
	restorePurchases = nrgMenu::create("Restore Purchases", "fonts/POE.ttf", 0, textSize, 0, cocos2d::Color4B::GRAY, cocos2d::Color4B::YELLOW, false);
	backButton = nrgButton::create(batch, "back_button.png", "back_button_pressed.png", false);

	if (!highlightMusic)
		musicOnOff->ChangeTextColor1(cocos2d::Color4B(225, 225, 225, 255));
	if (!highlightSound)
		soundOnOff->ChangeTextColor1(cocos2d::Color4B(225, 225, 225, 255));

	nrgFunctions::ResizeForDevice(backButton);
	backButton->addEvents();
	backButton->ToggleEnabled(false);
	backButton->setPositionX(10);
	backButton->SetTop(SCREEN_HEIGHT - 20);

	addChild(title);
	addChild(musicText);
	addChild(soundText);
	addChild(musicOnOff);
	addChild(soundOnOff);
	addChild(removeAds);
	addChild(howToPlay);
	addChild(usingBonusItems);
	addChild(restorePurchases);

	title->Center();
	title->SetBottom(SCREEN_HEIGHT * 0.7);

	soundText->Center();
	soundText->setPositionX(SCREEN_WIDTH / 3);
	soundOnOff->SetBottom(soundText->GetBottom());
	soundOnOff->setPositionX(SCREEN_WIDTH * 2 / 3);

	musicText->setPositionX(SCREEN_WIDTH / 3);
	musicText->SetBottom(soundText->getPositionY() + 30);
	musicOnOff->SetBottom(musicText->GetBottom());
	musicOnOff->setPositionX(SCREEN_WIDTH * 2 / 3);

	removeAds->Center();
	removeAds->SetTop(soundText->GetBottom() - 30);
	removeAds->GetItem(0)->runAction(cocos2d::RepeatForever::create(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(0.125),
			cocos2d::FadeTo::create(0.5, 64),
			cocos2d::FadeTo::create(0.5, 255),
			cocos2d::DelayTime::create(0.125), NULL)));

	howToPlay->Center();
	if(removeAds->GetItem(0)->getString() == "")
		howToPlay->SetTop(soundText->GetBottom() - 30);
	else
		howToPlay->SetTop(removeAds->GetBottom() - 50);
	howToPlay->GetItem(0)->runAction(cocos2d::RepeatForever::create(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(0.125),
			cocos2d::FadeTo::create(0.5, 64),
			cocos2d::FadeTo::create(0.5, 255),
			cocos2d::DelayTime::create(0.125), NULL)));

	usingBonusItems->Center();
	usingBonusItems->SetTop(howToPlay->GetBottom() - 50);
	usingBonusItems->GetItem(0)->runAction(cocos2d::RepeatForever::create(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(0.125),
			cocos2d::FadeTo::create(0.5, 64),
			cocos2d::FadeTo::create(0.5, 255),
			cocos2d::DelayTime::create(0.125), NULL)));
	
	restorePurchases->Center();
	restorePurchases->SetTop(usingBonusItems->GetBottom() - 50);
	restorePurchases->GetItem(0)->runAction(cocos2d::RepeatForever::create(
		cocos2d::Sequence::create(
			cocos2d::DelayTime::create(0.125),
			cocos2d::FadeTo::create(0.5, 64),
			cocos2d::FadeTo::create(0.5, 255),
			cocos2d::DelayTime::create(0.125), NULL)));

	scheduleUpdate();
	return true;
}

void Options::update(float dt)
{
	if (musicOnOff->GetConfirmedSelection() == 0)
	{
		if (gameData->getBoolForKey(music_on, true))
		{
			gameData->setBoolForKey(music_on, false);
			audio->StopMusic();
			musicOnOff->ChangeMenu("Off", 0, false);
			musicOnOff->ChangeTextColor1(cocos2d::Color4B(225, 225, 225, 255));
			musicOnOff->SetBottom(musicText->GetBottom());
			musicOnOff->setPositionX(SCREEN_WIDTH * 2 / 3);
		}

		else
		{
			audio->PlayClip("double_tone_higher");
			gameData->setBoolForKey(music_on, true);
			musicOnOff->ChangeMenu("On", 0, false);
			musicOnOff->ChangeTextColor1(cocos2d::Color4B::GRAY);
			musicOnOff->SetBottom(musicText->GetBottom());
			musicOnOff->setPositionX(SCREEN_WIDTH * 2 / 3);
		}
	}

	if (soundOnOff->GetConfirmedSelection() == 0)
	{
		if (gameData->getBoolForKey(sound_on, true))
		{
			gameData->setBoolForKey(sound_on, false);
			audio->MuteSound();
			soundOnOff->ChangeMenu("Off", 0, false);
			soundOnOff->ChangeTextColor1(cocos2d::Color4B(225, 225, 225, 255));
			soundOnOff->SetBottom(soundText->GetBottom());
			soundOnOff->setPositionX(SCREEN_WIDTH * 2 / 3);
		}

		else
		{
			audio->PlayClip("double_tone_higher");
			gameData->setBoolForKey(sound_on, true);
			audio->UnmuteSound();
			soundOnOff->ChangeMenu("On", 0, false);
			soundOnOff->ChangeTextColor1(cocos2d::Color4B::GRAY);
			soundOnOff->SetBottom(soundText->GetBottom());
			soundOnOff->setPositionX(SCREEN_WIDTH * 2 / 3);
		}
	}

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	if (removeAds->GetConfirmedSelection() == 0)
		sdkbox::IAP::purchase("remove_ads");
#endif

	if (howToPlay->GetConfirmedSelection() == 0)
	{
		audio->PlayClip("double_tone_higher");
		director->pushScene(Tutorial_Matching::CreateScene());
	}
	
	if (usingBonusItems->GetConfirmedSelection() == 0)
	{
		audio->PlayClip("double_tone_higher");
		director->pushScene(Tutorial_Bonus::CreateScene());
	}

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	if (restorePurchases->GetConfirmedSelection() == 0)
		sdkbox::IAP::restore();
#endif

	if (backButton->WasPressed())
	{
		audio->PlayClip("low_tone");
		director->popScene();
	}

	return;
}

void Options::onEnter()
{
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    audio->SetMusicVolume(0.35);
#endif
    
#if(CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    audio->SetMusicVolume(Game::GetiOSMinVolumeBG());
#endif
    
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	sdkbox::PluginAdMob::hide("home");
    sdkbox::IAP::setListener(this);
    sdkbox::IAP::refresh();
#endif
	backButton->ToggleEnabled(true);
	soundOnOff->ToggleOn(true);
	musicOnOff->ToggleOn(true);
	removeAds->ToggleOn(true);
	howToPlay->ToggleOn(true);
	usingBonusItems->ToggleOn(true);
	restorePurchases->ToggleOn(true);

	cocos2d::Layer::onEnter();

	return;
}

void Options::onExit()
{    
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    sdkbox::IAP::removeListener();
#endif
    
    backButton->ToggleEnabled(false);
	soundOnOff->ToggleOn(false);
	musicOnOff->ToggleOn(false);
	removeAds->ToggleOn(false);
	howToPlay->ToggleOn(false);
	usingBonusItems->ToggleOn(false);
	restorePurchases->ToggleOn(false);

	cocos2d::Layer::onExit();

	return;
}

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
void Options::onSuccess(sdkbox::Product const & p)
{
	if (p.name == "remove_ads")
	{
		audio->PlayClip("triple_tone_high");
		removeAds->ChangeMenu("");
		gameData->setBoolForKey(ads_removed, true);

		howToPlay->SetTop(soundText->GetBottom() - 30);
		usingBonusItems->SetTop(howToPlay->GetBottom() - 50);
		restorePurchases->SetTop(usingBonusItems->GetBottom() - 50);
	}

	return;
}

void Options::onFailure(sdkbox::Product const & p, const std::string & msg)
{
	CCLOG("Purchase Failed: %s", msg.c_str());
	audio->PlayClip("low_tone");
	return;
}

void Options::onCanceled(sdkbox::Product const & p)
{
	CCLOG("Purchase Canceled: %s", p.id.c_str());
	audio->PlayClip("low_tone");
	return;
}

void Options::onRestored(sdkbox::Product const & p)
{
	CCLOG("Purchase Restored: %s", p.name.c_str());
	audio->PlayClip("triple_tone_high");
	return;
}

void Options::onProductRequestSuccess(std::vector<sdkbox::Product> const & products)
{
	return;
}

void Options::onProductRequestFailure(const std::string & msg)
{
	CCLOG("Fail to load products");
	return;
}

void Options::onInitialized(bool ok)
{
	CCLOG("%s : %d", __func__, ok);
	return;
}

void Options::onRestoreComplete(bool ok, const std::string & msg)
{
	CCLOG("%s:%d:%s", __func__, ok, msg.data());
	return;
}
#endif