#pragma once
#include "Start.h"

cocos2d::Scene* Start::CreateScene()
{
	//Create scene
	cocos2d::Scene* scene = cocos2d::Scene::create();

	//Create Layer for scene and autorelease it
	cocos2d::Layer* layer = Start::create();

	//Add layer to scene
	scene->addChild(layer);

	return scene;
}

void Start::Init()
{
	SetInit(true);
}

bool Start::init()
{
	//Stop any previoiusly playing music for new scene
	audio->StopMusic();

    if(!gameData)
        gameData = cocos2d::UserDefault::getInstance();

	if(!gameData->getBoolForKey("completed_initial_load", false))
	{
		gameData->setBoolForKey(sound_on, true);
		gameData->setBoolForKey(music_on, true);
		gameData->setBoolForKey("completed_initial_load", true);
	}
	
	//Initialize origin
	SCREEN_WIDTH = glview->getDesignResolutionSize().width;
	SCREEN_HEIGHT = glview->getDesignResolutionSize().height; //Make letterbox 2.4 aspect ratio
	center.x = SCREEN_WIDTH / 2;
	center.y = SCREEN_HEIGHT / 2;

	//Set clear color to white
	director->setClearColor(cocos2d::Color4F(0, 0, 0, 0));
	
	CCLOG("Loading Background Image");

	//Set Sprites for nrgLogo animation
	SetBG("Out-Of-Orbit-Games-Logo.png");

	if (bg)
		CCLOGINFO("Background image loaded successfully");
	else
		CCLOGINFO("Background image failed to load");

	bg->setOpacity(0);

	cocos2d::DelayTime* delay1 = cocos2d::DelayTime::create(0.5);
	cocos2d::FadeIn* action1 = cocos2d::FadeIn::create(0.75);
	cocos2d::DelayTime* delay2 = cocos2d::DelayTime::create(0.75);
	cocos2d::FadeOut* action2 = cocos2d::FadeOut::create(0.75);
	cocos2d::DelayTime* delay3 = cocos2d::DelayTime::create(0.25);
	cocos2d::Sequence* sequence = cocos2d::Sequence::create(delay1, action1, delay2, action2, delay3, NULL);
	bg->runAction(sequence);
	
	//Once animation is over, wait 1 second, then load next level
	scheduleUpdate();
	
	return true;
}

void Start::update(float fl)
{
	//Once animation is over, then load next level
	if (bg->getNumberOfRunningActions() == 0)
		director->replaceScene(MainMenu::CreateScene());
		return;
}

void Start::onExit()
{
	//Remove theme from cache after done playing
	removeAllChildren();
	cocos2d::Layer::onExit();

	return;
}
